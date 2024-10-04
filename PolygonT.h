#pragma once

#include <array>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <limits>
#include <cmath>
#include <set>
#include "traits.h"

#include "traits.h"

#define POLYGONINOUTSTATUS(code) \
    code(UNKNOWN) code(InPolygon) code(OnPolygonEdge) code(OutsidePolygon)

enum class PolygonTestResult
{
#define ENUM_ITEM(x) x,
    POLYGONINOUTSTATUS(ENUM_ITEM)
#undef ENUM_ITEM
        STATECOUNT
};
const std::array<std::string, static_cast<int>(PolygonTestResult::STATECOUNT)> _enumItemStrings = {{
#define ITEM_STRING(x) #x,
    POLYGONINOUTSTATUS(ITEM_STRING)
#undef ITEM_STRING
}};

template <typename POINTARRAY>
auto PrintPolygon(const POINTARRAY &polygon) -> void
{
    using POINTTYPE = typename POINTARRAY::value_type;
    std::for_each(polygon.cbegin(), polygon.cend(), [](const POINTTYPE &point)
                  { std::cout << point << ", "; });
}

const double EPSILON = 1e-6;
/**
 * @brief Checks if a given point lies on a line segment.
 *
 * This function determines if the point `p` lies on the line segment
 * defined by the points `start` and `end`.
 *
 * @tparam POINTTYPE The type representing a point.
 * @param p The point to check.
 * @param start The start point of the line segment.
 * @param end The end point of the line segment.
 * @return True if `p` lies on the segment, false otherwise.
 */
template <typename POINTTYPE>
auto OnSegment(const POINTTYPE &p, const POINTTYPE &start, const POINTTYPE &end)
{
    // distance of two points(x1, y1) and (x2, y2) is sqrt((x1-x2)^2 + (y1-y2)^2)
    auto d1 = sqrt((fix_x(p) - fix_x(start)) * (fix_x(p) - fix_x(start)) + (fix_y(p) - fix_y(start)) * (fix_y(p) -fix_y(start)));
    auto d2 = sqrt((fix_x(p) - fix_x(end)) * (fix_x(p) - fix_x(end)) + (fix_y(p) - fix_y(end)) * (fix_y(p) - fix_y(end)));
    auto d3 = sqrt((fix_x(start) - fix_x(end)) * (fix_x(start) - fix_x(end)) + (fix_y(start) - fix_y(end)) * (fix_y(start) - fix_y(end)));
    
    if (fabs(d1 + d2 - d3) <= EPSILON)
    {
        return true;
    }
    return false;
}

// Specialized function for when the first parameter is a std::vector<double>
template <typename POINTTYPE>
typename std::enable_if<!std::is_same<POINTTYPE, std::vector<double>>::value, bool>::type
OnSegment(const std::vector<double> &p, const POINTTYPE &start, const POINTTYPE &end)
{
    auto d1 = sqrt((p[0] - fix_x(start))*(p[0] - fix_x(start)) + (p[1] - fix_y(start))*(p[1] - fix_y(start))); 
    auto d2 = sqrt((p[0] - fix_x(end))*(p[0] - fix_x(end)) + (p[1] - fix_y(end))*(p[1] - fix_y(end))); 
    auto d3 = sqrt((fix_x(start) - fix_x(end))*(fix_x(start) - fix_x(end)) + (fix_y(start) - fix_y(end))*(fix_y(start) - fix_y(end)));
    
    if(fabs(d1 + d2 - d3) <= EPSILON) { 
        return true;
    }
    return false;
}

/**
 * @brief Calculates the intersection of two parallel line segments.
 *
 * This function computes the intersection of two parallel line segments defined by
 * their endpoints and stores the intersection points in the given vector. If the segments
 * overlap, it returns the overlapping segment as a vector of points.
 *
 * @tparam POINTTYPE The type of the point
 * @param p1 The first endpoint of the first line segment.
 * @param q1 The second endpoint of the first line segment.
 * @param p2 The first endpoint of the second line segment.
 * @param q2 The second endpoint of the second line segment.
 * @param intersections A vector to store the intersection points if any.
 * @return A vector of vectors of doubles representing the intersection points. 
 */
template <typename POINTTYPE>
std::vector<std::vector<double>>
ParallelSegmentIntersection(const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE> &intersectons)
{
    auto unique_points = std::set<std::vector<double>>{};
    auto unique_intersectons = std::set<POINTTYPE>{};
    if (OnSegment(p1, p2, q2)) {
        unique_intersectons.insert(p1);
        unique_points.insert({double(fix_x(p1)), double(fix_y(p1))});
    }
    if (OnSegment(q1, p2, q2)) {
        unique_intersectons.insert(q1);
        unique_points.insert({double(fix_x(q1)), double(fix_y(q1))});
    }
    if (OnSegment(p2, p1, q1)) {
        unique_intersectons.insert(p2);
        unique_points.insert({double(fix_x(p2)), double(fix_y(p2))});
    }
    if (OnSegment(q2, p1, q1)) {
        unique_intersectons.insert(q2);
        unique_points.insert({double(fix_x(q2)), double(fix_y(q2))});
    }
    for (auto interaction : unique_intersectons) {
        intersectons.push_back(interaction);
    }
    return std::vector<std::vector<double>>(unique_points.begin(), unique_points.end());
}

// given two points, get the A, B, C of the linear equation Ax + By + C = 0
// formed by the two points
template <typename POINTTYPE>
std::vector<double> GetLineParameter(const POINTTYPE &point1, const POINTTYPE &point2)
{
    // A = y2-y1, B = x1-x2, C = x2y1-x1y2
    double a = fix_y(point2) - fix_y(point1);
    double b = fix_x(point1) - fix_x(point2);
    double c = fix_x(point2) * fix_y(point1) - fix_x(point1) * fix_y(point2);
    return std::vector<double>{a, b, c};
}

/**
 * @brief Calculates the intersection of two line segments.
 *
 * This function computes the intersection of two line segments defined by
 * their endpoints and stores the intersection points in the given vector.
 *
 * @tparam POINTTYPE The type of the point
 * @param p1 The first endpoint of the first line segment.
 * @param q1 The second endpoint of the first line segment.
 * @param p2 The first endpoint of the second line segment.
 * @param q2 The second endpoint of the second line segment.
 * @param intersections A vector to store the intersection points if any.
 * @return A vector of vectors of doubles representing the intersection points. 
 */
template <typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
std::vector<std::vector<double>>
SegmentIntersection(const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE> &intersectons)
{
    // Find the four orientations needed for general and special cases
    auto dx1 = fix_x(q1) - fix_x(p1);
    auto dy1 = fix_y(q1) - fix_y(p1);
    auto dx2 = fix_x(q2) - fix_x(p2);
    auto dy2 = fix_y(q2) - fix_y(p2);

    // two lines are parallel
    if (dx1 * dy2 == dx2 * dy1)
    {
        return ParallelSegmentIntersection(p1, q1, p2, q2, intersectons);
    }

    auto v1 = GetLineParameter(p1, q1);
    auto v2 = GetLineParameter(p2, q2);

    auto x = (v2[2] * v1[1] - v1[2] * v2[1]) / (v1[0] * v2[1] - v2[0] * v1[1]);
    auto y = (v1[2] * v2[0] - v2[2] * v1[0]) / (v1[0] * v2[1] - v2[0] * v1[1]);

    auto result = std::vector<std::vector<double>>{};
    auto point = std::vector<double>{x, y};
    POINTTYPE intersection = {static_cast<T>(x), static_cast<T>(y)};

    if (OnSegment(point, p1, q1) && OnSegment(point, p2, q2))
    {
        intersectons.push_back(intersection);
        result.push_back({x, y});
        return result;
    }
    return {};
}

template <typename POINTARRAY>
class PolygonT
{
public:
    using POINTTYPE = typename POINTARRAY::value_type;
    PolygonT(POINTARRAY &points) : _pointArray(points)
    {
        static_assert(has_indexer_v<POINTARRAY>,
                      "Template argument must have an indexer");
    }
    virtual ~PolygonT() = default;
    auto InPolygonTest(const POINTTYPE &point) const -> std::string
    {
        PolygonTestResult ret = PolygonTestResult::UNKNOWN;
        // TODO: implement this function from here and changed the ret to the
        // correct status

        //////////////////////////////////////////
        // If there are less than 3 points, it is not a polygon, return UNKNOWN for
        // simplicity
        if (_pointArray.size() < 3)
            return _enumItemStrings[static_cast<int>(PolygonTestResult::UNKNOWN)];

        std::cout << point << " in polygon ";
        PrintPolygon(_pointArray);
        std::cout << " is ";

        // record rightmost, leftmost, topmost, bottommost points, these will be
        // used to create extreme horizontal and vertical rays
        auto max_x = get_x(point), min_x = get_x(point), max_y = get_y(point), min_y = get_y(point);
        // loop through all adjacent segments
        for (auto i = 0; i < _pointArray.size(); ++i)
        {
            max_x = std::max(max_x, get_x(_pointArray[i]));
            min_x = std::min(min_x, get_x(_pointArray[i]));
            max_y = std::max(max_y, get_y(_pointArray[i]));
            min_y = std::min(min_y, get_y(_pointArray[i]));

            auto intersections = std::vector<POINTTYPE>();
            SegmentIntersection(_pointArray[i], _pointArray[(i + 1) % _pointArray.size()], _pointArray[(i + 1) % _pointArray.size()], _pointArray[(i + 2) % _pointArray.size()], intersections);
            // if two adjacent segments overlap, return UNKNOWN as this is not a standard polygon
            if (intersections.size() != 1)
            {
                std::cout << "UNKNOWN" << " size: " << intersections.size() << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::UNKNOWN)];
            }
            // if the point is on the segment, return OnPolygonEdge
            if (OnSegment(point, _pointArray[i], _pointArray[(i + 1) % _pointArray.size()]))
            {
                std::cout << "OnPolygonEdge" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::OnPolygonEdge)];
            }
        }

        POINTTYPE positive_x_extreme(create_max(max_x), get_y(point)); // positive x direction ray
        POINTTYPE positive_y_extreme(get_x(point), create_max(max_y)); // positive y direction ray
        POINTTYPE negative_x_extreme(create_min(min_x), get_y(point)); // negative x direction ray
        POINTTYPE negative_y_extreme(get_x(point), create_min(min_y)); // negative y direction ray
        
        std::vector<POINTTYPE> extreme_points{positive_x_extreme, positive_y_extreme, negative_x_extreme, negative_y_extreme};

        // a flag to keep track if there is no intersection for a raoy
        bool no_intersections = false;
        // loop through 4 rays
        for (auto i = 0; i < extreme_points.size(); ++i)
        {
            auto intersections_with_a_ray = std::set<POINTTYPE>();
            for (auto j = 0; j < _pointArray.size(); ++j)
            {
                auto intersections = std::vector<POINTTYPE>();
                SegmentIntersection(_pointArray[j], _pointArray[(j + 1) % _pointArray.size()], point, extreme_points[i], intersections);
                for (auto intersection : intersections)
                {
                    intersections_with_a_ray.insert(intersection);
                }
            }
            if (intersections_with_a_ray.empty())
            {
                no_intersections = true;
            }
        }
        
        if (no_intersections) {
            ret = PolygonTestResult::OutsidePolygon;
        } else {
            ret = PolygonTestResult::InPolygon;
        }
        std::cout << _enumItemStrings[static_cast<int>(ret)] << std::endl;
        return _enumItemStrings[static_cast<int>(ret)];
    }


    auto ClipSegments(const POINTARRAY &tobeclippedpath, POINTARRAY &clipped)
        -> void
    { // tobeclippedpath is the line segment list that adjacent points
      // forms a line segment
        std::cout << "use ";
        PrintPolygon(_pointArray);
        std::cout << " to clip ";
        PrintPolygon(tobeclippedpath);
        // TODO: implement a function to clip segments and return the clipped
        // segments with 'clipped'

        // note that the returned size of clipped should be a multiple of 2.
        // clipped[i] and clipped[i + 1] determines a line segment, where i = 0, 2, 4, ...
        if (_pointArray.size() < 3)
        {
            std::cout << "invalid polygon, return now" << std::endl;
            return;
        }
        if (tobeclippedpath.size() < 2)
        {
            std::cout << "invalid tobeclippedpath input, need to have at least two points, return now" << std::endl;
            return;
        }

        // loop through all the segments in tobeclippedpath
        for (auto i = 0; i < tobeclippedpath.size() - 1; ++i)
        {
            auto start_point = tobeclippedpath[i], end_point = tobeclippedpath[i + 1];
            auto start_status = InPolygonTest(start_point),
                 end_status = InPolygonTest(end_point);

            if (start_status == "UNKNOWN" || end_status == "UNKNOWN")
            {
                std::cout << "invalid polygon provided, continue";
                continue;
            }

            if (start_status == "InPolygon" && end_status == "InPolygon" ||
                start_status == "OnPolygonEdge" && end_status == "OnPolygonEdge")
            {
                clipped.push_back(start_point);
                clipped.push_back(end_point);
                continue;
            }

            auto all_intersections = std::set<POINTTYPE>{};
            for (auto j = 0; j < _pointArray.size(); ++j)
            {
                auto p = _pointArray[j], q = _pointArray[(j + 1) % _pointArray.size()];
                auto intersections = std::vector<POINTTYPE>{};
                SegmentIntersection(p, q, start_point, end_point, intersections);
                for (auto point : intersections)
                {
                    all_intersections.insert(point);
                }
            }

            // if one point is inside the polygon and another is outside, we need to
            // add in the inside point
            if (start_status == "InPolygon")
                clipped.push_back(start_point);
            if (end_status == "InPolygon")
                clipped.push_back(end_point);
            for (auto point : all_intersections)
            {
                clipped.push_back(point);
            }
        }
        std::cout << std::endl << " clipped is ";
        PrintPolygon(clipped);
        std::cout << std::endl;
    }

private:
    POINTARRAY &_pointArray;
};