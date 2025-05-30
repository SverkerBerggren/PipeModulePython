// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2015 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2015 Mateusz Loskot, London, UK.
// Copyright (c) 2013-2022 Adam Wulkiewicz, Lodz, Poland.

// This file was modified by Oracle on 2013-2024.
// Modifications copyright (c) 2013-2024, Oracle and/or its affiliates.
// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_TOUCHES_IMPLEMENTATION_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_TOUCHES_IMPLEMENTATION_HPP


#include <type_traits>

#include <boost/geometry/algorithms/detail/for_each_range.hpp>
#include <boost/geometry/algorithms/detail/gc_topological_dimension.hpp>
#include <boost/geometry/algorithms/detail/overlay/overlay.hpp>
#include <boost/geometry/algorithms/detail/overlay/self_turn_points.hpp>
#include <boost/geometry/algorithms/detail/sub_range.hpp>
#include <boost/geometry/algorithms/detail/relate/implementation.hpp>
#include <boost/geometry/algorithms/detail/relate/implementation_gc.hpp>
#include <boost/geometry/algorithms/detail/relate/relate_impl.hpp>
#include <boost/geometry/algorithms/detail/touches/interface.hpp>
#include <boost/geometry/algorithms/detail/visit.hpp>
#include <boost/geometry/algorithms/disjoint.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/num_geometries.hpp>

#include <boost/geometry/geometries/helper_geometry.hpp>

#include <boost/geometry/strategies/relate/cartesian.hpp>
#include <boost/geometry/strategies/relate/geographic.hpp>
#include <boost/geometry/strategies/relate/spherical.hpp>

#include <boost/geometry/views/detail/geometry_collection_view.hpp>


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace touches
{

// Box/Box

template
<
    std::size_t Dimension,
    std::size_t DimensionCount
>
struct box_box_loop
{
    template <typename Box1, typename Box2>
    static inline bool apply(Box1 const& b1, Box2 const& b2, bool & touch)
    {
        using coordinate_type1 = coordinate_type_t<Box1>;
        using coordinate_type2 = coordinate_type_t<Box2>;

        coordinate_type1 const& min1 = get<min_corner, Dimension>(b1);
        coordinate_type1 const& max1 = get<max_corner, Dimension>(b1);
        coordinate_type2 const& min2 = get<min_corner, Dimension>(b2);
        coordinate_type2 const& max2 = get<max_corner, Dimension>(b2);

        // TODO assert or exception?
        //BOOST_GEOMETRY_ASSERT(min1 <= max1 && min2 <= max2);

        if (max1 < min2 || max2 < min1)
        {
            return false;
        }

        if (max1 == min2 || max2 == min1)
        {
            touch = true;
        }

        return box_box_loop
                <
                    Dimension + 1,
                    DimensionCount
                >::apply(b1, b2, touch);
    }
};

template
<
    std::size_t DimensionCount
>
struct box_box_loop<DimensionCount, DimensionCount>
{
    template <typename Box1, typename Box2>
    static inline bool apply(Box1 const& , Box2 const&, bool &)
    {
        return true;
    }
};

struct box_box
{
    template <typename Box1, typename Box2, typename Strategy>
    static inline bool apply(Box1 const& b1, Box2 const& b2, Strategy const& /*strategy*/)
    {
        BOOST_STATIC_ASSERT((std::is_same
                                <
                                    geometry::coordinate_system_t<Box1>,
                                    geometry::coordinate_system_t<Box2>
                                >::value
                           ));
        assert_dimension_equal<Box1, Box2>();

        bool touches = false;
        bool ok = box_box_loop
                    <
                        0,
                        dimension<Box1>::type::value
                    >::apply(b1, b2, touches);

        return ok && touches;
    }
};

// Areal/Areal

struct areal_interrupt_policy
{
    static bool const enabled = true;
    bool found_touch;
    bool found_not_touch;

    // dummy variable required by self_get_turn_points::get_turns
    static bool const has_intersections = false;

    inline bool result()
    {
        return found_touch && !found_not_touch;
    }

    inline areal_interrupt_policy()
        : found_touch(false), found_not_touch(false)
    {}

    template <typename Range>
    inline bool apply(Range const& range)
    {
        // if already rejected (temp workaround?)
        if (found_not_touch)
        {
            return true;
        }

        for (auto it = boost::begin(range); it != boost::end(range); ++it)
        {
            if (it->has(overlay::operation_intersection))
            {
                found_not_touch = true;
                return true;
            }

            switch (it->method)
            {
                case overlay::method_crosses:
                    found_not_touch = true;
                    return true;
                case overlay::method_equal:
                    // Segment spatially equal means: at the right side
                    // the polygon internally overlaps. So return false.
                    found_not_touch = true;
                    return true;
                case overlay::method_touch:
                case overlay::method_touch_interior:
                case overlay::method_collinear:
                    if ( ok_for_touch(*it) )
                    {
                        found_touch = true;
                    }
                    else
                    {
                        found_not_touch = true;
                        return true;
                    }
                    break;
                case overlay::method_start :
                case overlay::method_none :
                case overlay::method_disjoint :
                case overlay::method_error :
                    break;
            }
        }

        return false;
    }

    template <typename Turn>
    inline bool ok_for_touch(Turn const& turn)
    {
        return turn.both(overlay::operation_union)
            || turn.both(overlay::operation_blocked)
            || turn.combination(overlay::operation_union, overlay::operation_blocked)
            ;
    }
};

template <typename Geometry1, typename Geometry2, typename Strategy>
inline bool point_on_border_within(Geometry1 const& geometry1,
                                   Geometry2 const& geometry2,
                                   Strategy const& strategy)
{
    using point_type = geometry::point_type_t<Geometry1>;
    typename helper_geometry<point_type>::type pt;
    return geometry::point_on_border(pt, geometry1)
        && geometry::within(pt, geometry2, strategy);
}

template <typename FirstGeometry, typename SecondGeometry, typename Strategy>
inline bool rings_containing(FirstGeometry const& geometry1,
                             SecondGeometry const& geometry2,
                             Strategy const& strategy)
{
    return geometry::detail::any_range_of(geometry2, [&](auto const& range)
    {
        return point_on_border_within(range, geometry1, strategy);
    });
}

template <typename Geometry1, typename Geometry2>
struct areal_areal
{
    template <typename Strategy>
    static inline bool apply(Geometry1 const& geometry1,
                             Geometry2 const& geometry2,
                             Strategy const& strategy)
    {
        using point_type = geometry::point_type_t<Geometry1>;
        using mutable_point_type = typename helper_geometry<point_type>::type;
        using turn_info = detail::overlay::turn_info<mutable_point_type>;

        std::deque<turn_info> turns;
        detail::touches::areal_interrupt_policy policy;
        geometry::get_turns
            <
                detail::overlay::do_reverse<geometry::point_order<Geometry1>::value>::value,
                detail::overlay::do_reverse<geometry::point_order<Geometry2>::value>::value,
                detail::overlay::assign_null_policy
            >(geometry1, geometry2, strategy, turns, policy);

        return policy.result()
            && ! geometry::detail::touches::rings_containing(geometry1, geometry2, strategy)
            && ! geometry::detail::touches::rings_containing(geometry2, geometry1, strategy);
    }
};

// P/*

struct use_point_in_geometry
{
    template <typename Point, typename Geometry, typename Strategy>
    static inline bool apply(Point const& point, Geometry const& geometry, Strategy const& strategy)
    {
        return detail::within::point_in_geometry(point, geometry, strategy) == 0;
    }
};

// GC

struct gc_gc
{
    template <typename Geometry1, typename Geometry2, typename Strategy>
    static inline bool apply(Geometry1 const& geometry1, Geometry2 const& geometry2,
                             Strategy const& strategy)
    {
        int const dimension1 = detail::gc_topological_dimension(geometry1);
        int const dimension2 = detail::gc_topological_dimension(geometry2);

        if (dimension1 == 0 && dimension2 == 0)
        {
            return false;
        }
        else
        {
            return detail::relate::relate_impl
                <
                    detail::de9im::static_mask_touches_not_pp_type,
                    Geometry1,
                    Geometry2
                >::apply(geometry1, geometry2, strategy);
        }
    }
};

struct notgc_gc
{
    template <typename Geometry1, typename Geometry2, typename Strategy>
    static inline bool apply(Geometry1 const& geometry1, Geometry2 const& geometry2,
                             Strategy const& strategy)
    {
        using gc1_view_t = detail::geometry_collection_view<Geometry1>;
        return gc_gc::apply(gc1_view_t(geometry1), geometry2, strategy);
    }
};

struct gc_notgc
{
    template <typename Geometry1, typename Geometry2, typename Strategy>
    static inline bool apply(Geometry1 const& geometry1, Geometry2 const& geometry2,
                             Strategy const& strategy)
    {
        using gc2_view_t = detail::geometry_collection_view<Geometry2>;
        return gc_gc::apply(geometry1, gc2_view_t(geometry2), strategy);
    }
};


}}
#endif // DOXYGEN_NO_DETAIL

#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch {

// P/P

template <typename Geometry1, typename Geometry2>
struct touches<Geometry1, Geometry2, point_tag, point_tag, pointlike_tag, pointlike_tag, false>
{
    template <typename Strategy>
    static inline bool apply(Geometry1 const& , Geometry2 const& , Strategy const&)
    {
        return false;
    }
};

template <typename Geometry1, typename Geometry2>
struct touches<Geometry1, Geometry2, point_tag, multi_point_tag, pointlike_tag, pointlike_tag, false>
{
    template <typename Strategy>
    static inline bool apply(Geometry1 const& , Geometry2 const& , Strategy const&)
    {
        return false;
    }
};

template <typename Geometry1, typename Geometry2>
struct touches<Geometry1, Geometry2, multi_point_tag, multi_point_tag, pointlike_tag, pointlike_tag, false>
{
    template <typename Strategy>
    static inline bool apply(Geometry1 const&, Geometry2 const&, Strategy const&)
    {
        return false;
    }
};

// P/L P/A

template <typename Point, typename Geometry, typename Tag2, typename CastedTag2>
struct touches<Point, Geometry, point_tag, Tag2, pointlike_tag, CastedTag2, false>
    : detail::touches::use_point_in_geometry
{};

template <typename MultiPoint, typename MultiGeometry, typename Tag2, typename CastedTag2>
struct touches<MultiPoint, MultiGeometry, multi_point_tag, Tag2, pointlike_tag, CastedTag2, false>
    : detail::relate::relate_impl
        <
            detail::de9im::static_mask_touches_type,
            MultiPoint,
            MultiGeometry
        >
{};

// L/P A/P

template <typename Geometry, typename MultiPoint, typename Tag1, typename CastedTag1>
struct touches<Geometry, MultiPoint, Tag1, multi_point_tag, CastedTag1, pointlike_tag, false>
    : detail::relate::relate_impl
        <
            detail::de9im::static_mask_touches_type,
            Geometry,
            MultiPoint
        >
{};

// Box/Box

template <typename Box1, typename Box2, typename CastedTag1, typename CastedTag2>
struct touches<Box1, Box2, box_tag, box_tag, CastedTag1, CastedTag2, false>
    : detail::touches::box_box
{};

template <typename Box1, typename Box2>
struct touches<Box1, Box2, box_tag, box_tag, areal_tag, areal_tag, false>
    : detail::touches::box_box
{};

// L/L

template <typename Linear1, typename Linear2, typename Tag1, typename Tag2>
struct touches<Linear1, Linear2, Tag1, Tag2, linear_tag, linear_tag, false>
    : detail::relate::relate_impl
    <
        detail::de9im::static_mask_touches_type,
        Linear1,
        Linear2
    >
{};

// L/A

template <typename Linear, typename Areal, typename Tag1, typename Tag2>
struct touches<Linear, Areal, Tag1, Tag2, linear_tag, areal_tag, false>
    : detail::relate::relate_impl
    <
        detail::de9im::static_mask_touches_type,
        Linear,
        Areal
    >
{};

// A/L
template <typename Linear, typename Areal, typename Tag1, typename Tag2>
struct touches<Areal, Linear, Tag1, Tag2, areal_tag, linear_tag, false>
    : detail::relate::relate_impl
    <
        detail::de9im::static_mask_touches_type,
        Areal,
        Linear
    >
{};

// A/A

template <typename Areal1, typename Areal2, typename Tag1, typename Tag2>
struct touches<Areal1, Areal2, Tag1, Tag2, areal_tag, areal_tag, false>
    : detail::relate::relate_impl
        <
            detail::de9im::static_mask_touches_type,
            Areal1,
            Areal2
        >
{};

template <typename Areal1, typename Areal2>
struct touches<Areal1, Areal2, ring_tag, ring_tag, areal_tag, areal_tag, false>
    : detail::touches::areal_areal<Areal1, Areal2>
{};

// GC

template <typename Geometry1, typename Geometry2>
struct touches
    <
        Geometry1, Geometry2,
        geometry_collection_tag, geometry_collection_tag,
        geometry_collection_tag, geometry_collection_tag,
        false
    >
    : detail::touches::gc_gc
{};


template <typename Geometry1, typename Geometry2, typename Tag1, typename CastedTag1>
struct touches
    <
        Geometry1, Geometry2,
        Tag1, geometry_collection_tag,
        CastedTag1, geometry_collection_tag,
        false
    >
    : detail::touches::notgc_gc
{};


template <typename Geometry1, typename Geometry2, typename Tag2, typename CastedTag2>
struct touches
    <
        Geometry1, Geometry2,
        geometry_collection_tag, Tag2,
        geometry_collection_tag, CastedTag2,
        false
    >
    : detail::touches::gc_notgc
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


namespace resolve_dynamic
{

template <typename Geometry, typename Tag>
struct self_touches
{
    static bool apply(Geometry const& geometry)
    {
        concepts::check<Geometry const>();

        using strategy_type = typename strategies::relate::services::default_strategy
            <
                Geometry, Geometry
            >::type;
        using point_type = geometry::point_type_t<Geometry>;
        using turn_info = detail::overlay::turn_info<point_type>;

        using policy_type = detail::overlay::get_turn_info
            <
                detail::overlay::assign_null_policy
            >;

        std::deque<turn_info> turns;
        detail::touches::areal_interrupt_policy policy;
        strategy_type strategy;
        // TODO: skip_adjacent should be set to false
        detail::self_get_turn_points::get_turns
            <
                false, policy_type
            >::apply(geometry, strategy,
                     turns, policy, 0, true);

        return policy.result();
    }
};

} // namespace resolve_dynamic

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_TOUCHES_IMPLEMENTATION_HPP
