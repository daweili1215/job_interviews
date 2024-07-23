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
// check if a given point p is on line segment that is from start to end
template <typename POINTTYPE>
auto OnSegment(const POINTTYPE &p, const POINTTYPE &start, const POINTTYPE &end)
{
    // distance of two points(x1, y1) and (x2, y2) is sqrt((x1-x2)^2 + (y1-y2)^2)
    auto d1 = sqrt((get_x(p) - get_x(start)) * (get_x(p) - get_x(start)) + (get_y(p) - get_y(start)) * (get_y(p) - get_y(start)));
    auto d2 = sqrt((get_x(p) - get_x(end)) * (get_x(p) - get_x(end)) + (get_y(p) - get_y(end)) * (get_y(p) - get_y(end)));
    auto d3 = sqrt((get_x(start) - get_x(end)) * (get_x(start) - get_x(end)) + (get_y(start) - get_y(end)) * (get_y(start) - get_y(end)));
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
    // distance of two points(x1, y1) and (x2, y2) is sqrt((x1-x2)^2 + (y1-y2)^2)
    auto d1 = sqrt((p[0] - get_x(start)) * (p[0] - get_x(start)) + (p[1] - get_y(start)) * (p[1] - get_y(start)));
    auto d2 = sqrt((p[0] - get_x(end)) * (p[0] - get_x(end)) + (p[1] - get_y(end)) * (p[1] - get_y(end)));
    auto d3 = sqrt((get_x(start) - get_x(end)) * (get_x(start) - get_x(end)) + (get_y(start) - get_y(end)) * (get_y(start) - get_y(end)));
    if (fabs(d1 + d2 - d3) <= EPSILON)
    {
        return true;
    }
    return false;
}
// get orientation of ordered triplet (p, q, r)
template <typename T>
auto orientation(const T &p, const T &q, const T &r)
{
    auto val = (get_y(q) - get_y(p)) * (get_x(r) - get_x(q)) - (get_x(q) - get_x(p)) * (get_y(r) - get_y(q));
    if (val == 0)
        return 0;             // collinear
    return (val > 0) ? 1 : 2; // clock or counterclock wise
}
// handle general case with integral coordinates
template <typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
std::enable_if_t<std::is_integral<T>::value, void> GeneralCaseSegmentIntersection(const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE> &intersectons)
{
    // Compute the intersection point
    auto a1 = static_cast<int64_t>(get_y(q1)) - static_cast<int64_t>(get_y(p1));
    auto b1 = static_cast<int64_t>(get_x(p1)) - static_cast<int64_t>(get_x(q1));
    auto c1 = a1 * static_cast<int64_t>(get_x(p1)) + b1 * static_cast<int64_t>(get_y(p1));

    auto a2 = static_cast<int64_t>(get_y(q2)) - static_cast<int64_t>(get_y(p2));
    auto b2 = static_cast<int64_t>(get_x(p2)) - static_cast<int64_t>(get_x(q2));
    auto c2 = a2 * static_cast<int64_t>(get_x(p2)) + b2 * static_cast<int64_t>(get_y(p2));

    auto determinant = a1 * b2 - a2 * b1;
    if (determinant == 0)
    {
        // The lines are parallel. This case is handled by the collinear case later.
        return;
    }
    else
    {
        auto x = double(b2 * c1 - b1 * c2) / double(determinant);
        auto y = double(a1 * c2 - a2 * c1) / double(determinant);
        POINTTYPE intersection = {static_cast<T>(x), static_cast<T>(y)};
        std::vector<double> point{x, y};
        if (OnSegment(point, p1, q1) && OnSegment(point, p2, q2))
        {
            intersectons.push_back({static_cast<T>(x), static_cast<T>(y)});
        }
        return;
    }
}

template <typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
std::enable_if_t<!std::is_integral<T>::value, void> GeneralCaseSegmentIntersection(const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE> &intersectons)
{
    // Compute the intersection point
    auto a1 = get_y(q1) - get_y(p1);
    auto b1 = get_x(p1) - get_x(q1);
    auto c1 = a1 * get_x(p1) + b1 * get_y(p1);

    auto a2 = get_y(q2) - get_y(p2);
    auto b2 = get_x(p2) - get_x(q2);
    auto c2 = a2 * get_x(p2) + b2 * get_y(p2);

    auto determinant = a1 * b2 - a2 * b1;
    if (std::fabs(determinant) < 1e-5)
    {
        // The lines are parallel. This case is handled by the collinear case later.
        return;
    }
    else
    {
        auto x = (b2 * c1 - b1 * c2) / determinant;
        auto y = (a1 * c2 - a2 * c1) / determinant;
        POINTTYPE intersection = {x, y};
        if (OnSegment(intersection, p1, q1) && OnSegment(intersection, p2, q2))
        {
            intersectons.push_back(intersection);
        }
        return;
    }
}

template <typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
auto SegmentIntersection(const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE> &intersectons)
{
    // Find the four orientations needed for general and special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
    {
        GeneralCaseSegmentIntersection(p1, q1, p2, q2, intersectons);
        return;
    }

    // Check for overlapping segments
    if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0)
    {
        std::vector<POINTTYPE> overlap_points;

        if (OnSegment(p2, p1, q1))
            overlap_points.push_back(p2);
        if (OnSegment(q2, p1, q1))
            overlap_points.push_back(q2);
        if (OnSegment(p1, p2, q2))
            overlap_points.push_back(p1);
        if (OnSegment(q1, p2, q2))
            overlap_points.push_back(q1);

        std::sort(overlap_points.begin(), overlap_points.end(), [](const POINTTYPE &a, const POINTTYPE &b)
                  { return (get_x(a) < get_x(b)) || (get_x(a) == get_x(b) && get_y(a) < get_y(b)); });

        auto it = std::unique(overlap_points.begin(), overlap_points.end());
        overlap_points.resize(std::distance(overlap_points.begin(), it));

        for (auto point : overlap_points)
            intersectons.push_back(point);
        return;
    }
    // Special cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && OnSegment(p2, p1, q1))
        return intersectons.push_back(p2);

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && OnSegment(q2, p1, q1))
        return intersectons.push_back(q2);

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && OnSegment(p1, p2, q2))
        return intersectons.push_back(p1);

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && OnSegment(q1, p2, q2))
        return intersectons.push_back(q1);
}

template <typename POINTARRAY>
class PolygonT
{
public:
    using POINTTYPE = typename POINTARRAY::value_type;
    PolygonT(POINTARRAY &points) : _pointArray(points)
    {
        static_assert(has_indexer_v<POINTARRAY>, "Template argument must have an indexer");
    }
    virtual ~PolygonT() = default;
    auto InPolygonTest(const POINTTYPE &point) const -> std::string
    {
        PolygonTestResult ret = PolygonTestResult::UNKNOWN;
        // TODO: implement this function from here and changed the ret to the correct status

        //////////////////////////////////////////
        // If there are less than 3 points, it is not a polygon, return UNKNOWN for simplicity
        if (_pointArray.size() < 3)
            return _enumItemStrings[static_cast<int>(PolygonTestResult::UNKNOWN)];

        std::cout << point << " in polygon ";
        PrintPolygon(_pointArray);
        std::cout << " is ";

        // record rightmost, leftmost, topmost, bottommost points, these will be used to create extreme horizontal and vertical rays
        auto max_x = get_x(point), min_x = get_x(point), max_y = get_y(point), min_y = get_y(point);
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
                std::cout << "UNKNOWN" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::UNKNOWN)];
            }

            if (OnSegment(point, _pointArray[i], _pointArray[(i + 1) % _pointArray.size()]))
            {
                std::cout << "OnPolygonEdge" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::OnPolygonEdge)];
            }
        }

        POINTTYPE positive_x_extreme(std::min(std::max(static_cast<get_coordinate_type_t<POINTTYPE>>(0), 2 * max_x), std::numeric_limits<get_coordinate_type_t<POINTTYPE>>::max()), get_y(point)); // positive x direction ray
        POINTTYPE positive_y_extreme(get_x(point), std::min(std::max(static_cast<get_coordinate_type_t<POINTTYPE>>(0), 2 * max_y), std::numeric_limits<get_coordinate_type_t<POINTTYPE>>::max())); // positive y direction ray
        POINTTYPE negative_x_extreme(std::max(std::min(static_cast<get_coordinate_type_t<POINTTYPE>>(0), 2 * min_x), std::numeric_limits<get_coordinate_type_t<POINTTYPE>>::min()), get_y(point)); // negative x direction ray
        POINTTYPE negative_y_extreme(get_x(point), std::max(std::min(static_cast<get_coordinate_type_t<POINTTYPE>>(0), 2 * min_y), std::numeric_limits<get_coordinate_type_t<POINTTYPE>>::min())); // negative y direction ray
        std::vector<POINTTYPE> extreme_points{positive_x_extreme, positive_y_extreme, negative_x_extreme, negative_y_extreme};

        for (auto i = 0; i < extreme_points.size(); ++i)
        {
            auto all_intersections = std::set<POINTTYPE>();
            for (auto j = 0; j < _pointArray.size(); ++j)
            {
                auto intersections = std::vector<POINTTYPE>();
                SegmentIntersection(_pointArray[j], _pointArray[(j + 1) % _pointArray.size()], point, extreme_points[i], intersections);
                if (!intersections.empty())
                {
                    for (auto intersection : intersections)
                    {
                        all_intersections.insert(intersection);
                    }
                }
            }
            // if there is no intersection with one of the 4 rays, or there are 2 unique ones, then it must be outside
            if (all_intersections.empty() || all_intersections.size() == 2)
            {
                std::cout << "OutsidePolygon" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::OutsidePolygon)];
            }
        }
        ret = PolygonTestResult::InPolygon;
        std::cout << _enumItemStrings[static_cast<int>(ret)] << std::endl;
        return _enumItemStrings[static_cast<int>(ret)];
    }

    auto ClipSegments(const POINTARRAY &tobeclippedpath, POINTARRAY &clipped) -> void
    { // tobeclippedpath is the line segment list that adjacent points forms a line segment
        std::cout << "use ";
        PrintPolygon(_pointArray);
        std::cout << " to clip ";
        PrintPolygon(tobeclippedpath);
        // TODO: implement a function to clip segments and return the clipped segments with 'clipped'

        // note that the returned size of clipped should be a multiple of 2. clipped[i] and clipped[i + 1] determines a line segment, where i = 0, 2, 4, ...
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
            auto start_status = InPolygonTest(start_point), end_status = InPolygonTest(end_point);

            if (start_status == "UNKNOWN" || end_status == "UNKNOWN")
            {
                std::cout << "invalid polygon provided, continue";
                continue;
            }

            if (start_status == "InPolygon" && end_status == "InPolygon" || start_status == "OnPolygonEdge" && end_status == "OnPolygonEdge")
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

            // if one point is inside the polygon and another is outside, we need to add in the inside point
            if (start_status == "InPolygon")
                clipped.push_back(start_point);
            if (end_status == "InPolygon")
                clipped.push_back(end_point);
            for (auto point : all_intersections)
            {
                clipped.push_back(point);
            }
        }
        std::cout << std::endl
                  << " clipped is ";
        PrintPolygon(clipped);
        std::cout << std::endl;
    }

private:
    POINTARRAY &_pointArray;
};