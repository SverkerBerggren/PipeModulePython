// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2023 Adam Wulkiewicz, Lodz, Poland.

// Copyright (c) 2014-2021, Oracle and/or its affiliates.
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_DISTANCE_GEOMETRY_TO_SEGMENT_OR_BOX_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_DISTANCE_GEOMETRY_TO_SEGMENT_OR_BOX_HPP

#include <iterator>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <boost/geometry/algorithms/assign.hpp>
#include <boost/geometry/algorithms/detail/closest_feature/geometry_to_range.hpp>
#include <boost/geometry/algorithms/detail/closest_feature/point_to_range.hpp>
#include <boost/geometry/algorithms/detail/distance/is_comparable.hpp>
#include <boost/geometry/algorithms/detail/distance/strategy_utils.hpp>
#include <boost/geometry/algorithms/dispatch/distance.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/algorithms/num_points.hpp>

#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/tags.hpp>

#include <boost/geometry/iterators/point_iterator.hpp>
#include <boost/geometry/iterators/segment_iterator.hpp>

#include <boost/geometry/strategies/distance.hpp>
#include <boost/geometry/strategies/tags.hpp>

#include <boost/geometry/util/constexpr.hpp>


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace distance
{


// closure of segment or box point range
template
<
    typename SegmentOrBox,
    typename Tag = tag_t<SegmentOrBox>
>
struct segment_or_box_point_range_closure
    : not_implemented<SegmentOrBox>
{};

template <typename Segment>
struct segment_or_box_point_range_closure<Segment, segment_tag>
{
    static const closure_selector value = closed;
};

template <typename Box>
struct segment_or_box_point_range_closure<Box, box_tag>
{
    static const closure_selector value = open;
};



template
<
    typename Geometry,
    typename SegmentOrBox,
    typename Strategies,
    typename Tag = tag_t<Geometry>
>
class geometry_to_segment_or_box
{
private:
    using segment_or_box_point = point_type_t<SegmentOrBox>;
    using strategy_type = distance::strategy_t<Geometry, SegmentOrBox, Strategies>;
    using point_to_point_range = detail::closest_feature::point_to_point_range
        <
            point_type_t<Geometry>,
            std::vector<segment_or_box_point>,
            segment_or_box_point_range_closure<SegmentOrBox>::value
        >;
    using geometry_to_range = detail::closest_feature::geometry_to_range;
    using comparable_return_type = distance::creturn_t<Geometry, SegmentOrBox, Strategies>;

    // assign the new minimum value for an iterator of the point range
    // of a segment or a box
    template
    <
        typename SegOrBox,
        typename SegOrBoxTag = tag_t<SegOrBox>
    >
    struct assign_new_min_iterator
        : not_implemented<SegOrBox>
    {};

    template <typename Segment>
    struct assign_new_min_iterator<Segment, segment_tag>
    {
        template <typename Iterator>
        static inline void apply(Iterator&, Iterator)
        {
        }
    };

    template <typename Box>
    struct assign_new_min_iterator<Box, box_tag>
    {
        template <typename Iterator>
        static inline void apply(Iterator& it_min, Iterator it)
        {
            it_min = it;
        }
    };


    // assign the points of a segment or a box to a range
    template
    <
        typename SegOrBox,
        typename PointRange,
        typename SegOrBoxTag = tag_t<SegOrBox>
    >
    struct assign_segment_or_box_points
    {};

    template <typename Segment, typename PointRange>
    struct assign_segment_or_box_points<Segment, PointRange, segment_tag>
    {
        static inline void apply(Segment const& segment, PointRange& range)
        {
            detail::assign_point_from_index<0>(segment, range[0]);
            detail::assign_point_from_index<1>(segment, range[1]);
        }
    };

    template <typename Box, typename PointRange>
    struct assign_segment_or_box_points<Box, PointRange, box_tag>
    {
        static inline void apply(Box const& box, PointRange& range)
        {
            detail::assign_box_corners_oriented<true>(box, range);
        }
    };

public:
    typedef distance::return_t<Geometry, SegmentOrBox, Strategies> return_type;

    static inline return_type apply(Geometry const& geometry,
                                    SegmentOrBox const& segment_or_box,
                                    Strategies const& strategies,
                                    bool check_intersection = true)
    {
        typedef geometry::segment_iterator
            <
                Geometry const
            > segment_iterator_type;

        typedef assign_new_min_iterator<SegmentOrBox> assign_new_value;


        if (check_intersection
            && geometry::intersects(geometry, segment_or_box, strategies))
        {
            return return_type(0);
        }

        strategy_type const strategy = strategies.distance(geometry, segment_or_box);

        auto const cstrategy = strategy::distance::services::get_comparable
                                <
                                    strategy_type
                                >::apply(strategy);

        // get all points of the segment or the box
        std::vector<segment_or_box_point>
            seg_or_box_points(geometry::num_points(segment_or_box));

        assign_segment_or_box_points
            <
                SegmentOrBox,
                std::vector<segment_or_box_point>
            >::apply(segment_or_box, seg_or_box_points);

        // consider all distances of the points in the geometry to the
        // segment or box
        comparable_return_type cd_min1(0);
        auto pit_min = points_begin(geometry);
        auto it_min1 = boost::const_begin(seg_or_box_points);
        auto it_min2 = it_min1 + 1;
        bool first = true;

        for (auto pit = pit_min;
             pit != points_end(geometry); ++pit, first = false)
        {
            comparable_return_type cd;
            auto it_pair = point_to_point_range::apply(*pit,
                boost::const_begin(seg_or_box_points),
                boost::const_end(seg_or_box_points),
                cstrategy,
                cd);

            if (first || cd < cd_min1)
            {
                cd_min1 = cd;
                pit_min = pit;
                assign_new_value::apply(it_min1, it_pair.first);
                assign_new_value::apply(it_min2, it_pair.second);
            }
        }

        // consider all distances of the points in the segment or box to the
        // segments of the geometry
        comparable_return_type cd_min2(0);
        segment_iterator_type sit_min;
        auto it_min = boost::const_begin(seg_or_box_points);

        first = true;
        for (auto it = boost::const_begin(seg_or_box_points);
             it != boost::const_end(seg_or_box_points); ++it, first = false)
        {
            comparable_return_type cd;
            segment_iterator_type sit
                = geometry_to_range::apply(*it,
                                           segments_begin(geometry),
                                           segments_end(geometry),
                                           cstrategy,
                                           cd);

            if (first || cd < cd_min2)
            {
                cd_min2 = cd;
                it_min = it;
                sit_min = sit;
            }
        }

        if BOOST_GEOMETRY_CONSTEXPR (is_comparable<strategy_type>::value)
        {
            return (std::min)(cd_min1, cd_min2);
        }
        else // else prevents unreachable code warning
        {
            if (cd_min1 < cd_min2)
            {
                return strategy.apply(*pit_min, *it_min1, *it_min2);
            }
            else
            {
                return dispatch::distance
                    <
                        segment_or_box_point,
                        typename std::iterator_traits
                            <
                                segment_iterator_type
                            >::value_type,
                        Strategies
                    >::apply(*it_min, *sit_min, strategies);
            }
        }
    }


    static inline return_type apply(SegmentOrBox const& segment_or_box, Geometry const& geometry,
                                    Strategies const& strategies, bool check_intersection = true)
    {
        return apply(geometry, segment_or_box, strategies, check_intersection);
    }
};



template <typename MultiPoint, typename SegmentOrBox, typename Strategies>
class geometry_to_segment_or_box
    <
        MultiPoint, SegmentOrBox, Strategies, multi_point_tag
    >
{
private:
    typedef detail::closest_feature::geometry_to_range base_type;
    typedef detail::closest_feature::geometry_to_range geometry_to_range;
    typedef distance::strategy_t<MultiPoint, SegmentOrBox, Strategies> strategy_type;

public:
    typedef distance::return_t<MultiPoint, SegmentOrBox, Strategies> return_type;

    static inline return_type apply(MultiPoint const& multipoint,
                                    SegmentOrBox const& segment_or_box,
                                    Strategies const& strategies)
    {
        distance::creturn_t<MultiPoint, SegmentOrBox, Strategies> cd_min;

        auto const it_min
            = geometry_to_range::apply(segment_or_box,
                                       boost::begin(multipoint),
                                       boost::end(multipoint),
                                       strategy::distance::services::get_comparable
                                           <
                                               strategy_type
                                           >::apply(strategies.distance(multipoint, segment_or_box)),
                                       cd_min);

        return
            is_comparable<strategy_type>::value
            ?
            cd_min
            :
            dispatch::distance
                <
                    point_type_t<MultiPoint>,
                    SegmentOrBox,
                    Strategies
                >::apply(*it_min, segment_or_box, strategies);
    }
};



}} // namespace detail::distance
#endif // DOXYGEN_NO_DETAIL



#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template <typename Linear, typename Segment, typename Strategy>
struct distance
    <
        Linear, Segment, Strategy, linear_tag, segment_tag,
        strategy_tag_distance_point_segment, false
    > : detail::distance::geometry_to_segment_or_box<Linear, Segment, Strategy>
{};


template <typename Areal, typename Segment, typename Strategy>
struct distance
    <
        Areal, Segment, Strategy, areal_tag, segment_tag,
        strategy_tag_distance_point_segment, false
    > : detail::distance::geometry_to_segment_or_box<Areal, Segment, Strategy>
{};


template <typename Segment, typename Areal, typename Strategy>
struct distance
    <
        Segment, Areal, Strategy, segment_tag, areal_tag,
        strategy_tag_distance_point_segment, false
    > : detail::distance::geometry_to_segment_or_box<Areal, Segment, Strategy>
{};


template <typename Linear, typename Box, typename Strategy>
struct distance
    <
        Linear, Box, Strategy, linear_tag, box_tag,
        strategy_tag_distance_point_segment, false
    > : detail::distance::geometry_to_segment_or_box
        <
            Linear, Box, Strategy
        >
{};


template <typename Areal, typename Box, typename Strategy>
struct distance
    <
        Areal, Box, Strategy, areal_tag, box_tag,
        strategy_tag_distance_point_segment, false
    > : detail::distance::geometry_to_segment_or_box<Areal, Box, Strategy>
{};


template <typename MultiPoint, typename Segment, typename Strategy>
struct distance
    <
        MultiPoint, Segment, Strategy,
        multi_point_tag, segment_tag,
        strategy_tag_distance_point_segment, false
    > : detail::distance::geometry_to_segment_or_box
        <
            MultiPoint, Segment, Strategy
        >
{};


template <typename MultiPoint, typename Box, typename Strategy>
struct distance
    <
        MultiPoint, Box, Strategy,
        multi_point_tag, box_tag,
        strategy_tag_distance_point_box, false
    > : detail::distance::geometry_to_segment_or_box
        <
            MultiPoint, Box, Strategy
        >
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_DISTANCE_GEOMETRY_TO_SEGMENT_OR_BOX_HPP
