//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_HTTP_VECTOR_BODY_HPP
#define BOOST_BEAST_HTTP_VECTOR_BODY_HPP

#include <boost/beast/http/vector_body_fwd.hpp>

#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/detail/clamp.hpp>
#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/http/error.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/optional.hpp>
#include <cstdint>
#include <utility>
#include <vector>

namespace boost {
namespace beast {
namespace http {

/** A <em>Body</em> using `std::vector`

    This body uses `std::vector` as a memory-based container
    for holding message payloads. Messages using this body type
    may be serialized and parsed.
*/
#if BOOST_BEAST_DOXYGEN
template<class T, class Allocator = std::allocator<T>>
#else
template<class T, class Allocator>
#endif
struct vector_body
{
private:
    static_assert(sizeof(T) == 1,
        "T requirements not met");

public:
    /** The type of container used for the body

        This determines the type of @ref message::body
        when this body type is used with a message container.
    */
    using value_type = std::vector<T, Allocator>;

    /** Returns the payload size of the body

        When this body is used with @ref message::prepare_payload,
        the Content-Length will be set to the payload size, and
        any chunked Transfer-Encoding will be removed.
    */
    static
    std::uint64_t
    size(value_type const& body)
    {
        return body.size();
    }

    /** The algorithm for parsing the body

        Meets the requirements of <em>BodyReader</em>.
    */
#if BOOST_BEAST_DOXYGEN
    using reader = __implementation_defined__;
#else
    class reader
    {
        value_type& body_;

    public:
        template<bool isRequest, class Fields>
        explicit
        reader(header<isRequest, Fields>&, value_type& b)
            : body_(b)
        {
        }

        void
        init(boost::optional<
            std::uint64_t> const& length, error_code& ec)
        {
            if(length)
            {
                if(*length > body_.max_size())
                {
                    BOOST_BEAST_ASSIGN_EC(ec, error::buffer_overflow);
                    return;
                }
                body_.reserve(beast::detail::clamp(*length));
            }
            ec = {};
        }

        template<class ConstBufferSequence>
        std::size_t
        put(ConstBufferSequence const& buffers,
            error_code& ec)
        {
            auto const n = buffer_bytes(buffers);
            auto const len = body_.size();
            if (n > body_.max_size() - len)
            {
                BOOST_BEAST_ASSIGN_EC(ec, error::buffer_overflow);
                return 0;
            }

            body_.resize(len + n);
            ec = {};
            return net::buffer_copy(net::buffer(
                &body_[0] + len, n), buffers);
        }

        void
        finish(error_code& ec)
        {
            ec = {};
        }
    };
#endif

    /** The algorithm for serializing the body

        Meets the requirements of <em>BodyWriter</em>.
    */
#if BOOST_BEAST_DOXYGEN
    using writer = __implementation_defined__;
#else
    class writer
    {
        value_type const& body_;

    public:
        using const_buffers_type =
            net::const_buffer;

        template<bool isRequest, class Fields>
        explicit
        writer(header<isRequest, Fields> const&, value_type const& b)
            : body_(b)
        {
        }

        void
        init(error_code& ec)
        {
            ec = {};
        }

        boost::optional<std::pair<const_buffers_type, bool>>
        get(error_code& ec)
        {
            ec = {};
            return {{const_buffers_type{
                body_.data(), body_.size()}, false}};
        }
    };
#endif
};

} // http
} // beast
} // boost

#endif
