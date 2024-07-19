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
// check if point q is on line segment p to r
template <typename T>
auto OnSegment(const T &p, const T &q, const T &r)
{
    if (get_x(q) <= std::max(get_x(p), get_x(r)) && get_x(q) >= std::min(get_x(p), get_x(r)) &&
        get_y(q) <= std::max(get_y(p), get_y(r)) && get_y(q) >= std::min(get_y(p), get_y(r)))
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
        auto x = (b2 * c1 - b1 * c2) / determinant;
        auto y = (a1 * c2 - a2 * c1) / determinant;
        POINTTYPE intersection = {static_cast<T>(x), static_cast<T>(y)};
        if (OnSegment(p1, intersection, q1) && OnSegment(p2, intersection, q2))
        {
            intersectons.push_back(intersection);
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
        if (OnSegment(p1, intersection, q1) && OnSegment(p2, intersection, q2))
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

        if (OnSegment(p1, p2, q1))
            overlap_points.push_back(p2);
        if (OnSegment(p1, q2, q1))
            overlap_points.push_back(q2);
        if (OnSegment(p2, p1, q2))
            overlap_points.push_back(p1);
        if (OnSegment(p2, q1, q2))
            overlap_points.push_back(q1);

        std::sort(overlap_points.begin(), overlap_points.end(), [](const POINTTYPE &a, const POINTTYPE &b)
                  { return (get_x(a) < get_x(b)) || (get_x(a) == get_x(b) && get_y(a) < get_y(b)); });

        auto it = std::unique(overlap_points.begin(), overlap_points.end());
        overlap_points.resize(std::distance(overlap_points.begin(), it));

        intersectons = overlap_points;
        return;
    }
    // Special cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && OnSegment(p1, p2, q1))
        return intersectons.push_back(p2);

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && OnSegment(p1, q2, q1))
        return intersectons.push_back(q2);

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && OnSegment(p2, p1, q2))
        return intersectons.push_back(p1);

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && OnSegment(p2, q1, q2))
        return intersectons.push_back(q1);
}

// Function to check if point p is inside a given polygon
template <typename T>
auto IsInside(const T &p, const T &a, const T &b)
{
    return (get_x(b) - get_x(a)) * (get_y(p) - get_y(a)) - (get_y(b) - get_y(a)) * (get_x(p) - get_x(a)) >= 0;
}

// TODO: this part needs to be revisited
template <typename POINTARRAY>
auto SutherlandHodgmanClip(const POINTARRAY &polygon, const POINTARRAY &clipper)
{
    auto clipped_polygon = polygon;
    for (auto i = 0; i < clipper.size(); ++i)
    {
        auto input = clipped_polygon;
        clipped_polygon.clear();

        auto a = clipper[i];
        auto b = clipper[(i + 1) % clipper.size()];

        for (auto j = 0; j < input.size(); ++j)
        {
            auto p = input[j];
            auto q = input[(j + 1) % input.size()];
            if (IsInside(q, a, b))
            {
                if (!IsInside(p, a, b))
                {
                    // TODO: check return value of SegmenIntersection
                    // SegmentIntersection(a, b, p, q);
                }
                clipped_polygon.push_back(q);
            }
            else if (IsInside(p, a, b))
            {
                // TODO: check return value of SegmenIntersection
                // clipped_polygon.push_back(SegmentIntersection(a, b, p, q));
            }
        }
    }
    return clipped_polygon;
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
        int n = _pointArray.size();
        // If there are less than 3 points, it is not a polygon, return OutsidePolygon for simplicity
        if (n < 3)
            return _enumItemStrings[static_cast<int>(PolygonTestResult::OutsidePolygon)];

        std::cout << point << " in polygon ";
        PrintPolygon(_pointArray);
        std::cout << " is ";

        // if two adjacent segments overlap, return UNKNOWN as this is not a polygon
        for (auto i = 0; i < n; ++i)
        {
            auto intersections = std::vector<POINTTYPE>();
            SegmentIntersection(_pointArray[i], _pointArray[(i + 1) % n], _pointArray[(i + 1) % n], _pointArray[(i + 2) % n], intersections);
            if (intersections.size() != 1)
            {
                std::cout << "UNKNOWN" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::UNKNOWN)];
            }
        }

        auto mx = get_x(point);
        for (auto i = 0; i < n; ++i)
        {
            mx = std::max(mx, get_x(_pointArray[i]));
        }
        POINTTYPE extreme(10 * mx, get_y(point)); // A point far away to the right

        // Count unique intersections of the above line with polygon edges
        auto st = std::set<POINTTYPE>{};
        for (auto i = 0; i < n; ++i)
        {
            auto intersections = std::vector<POINTTYPE>();
            SegmentIntersection(_pointArray[i], _pointArray[(i + 1) % n], point, extreme, intersections);

            if (intersections.size() == 1)
            {
                if (point == intersections[0])
                {
                    std::cout << "OnPolygonEdge" << std::endl;
                    return _enumItemStrings[static_cast<int>(PolygonTestResult::OnPolygonEdge)];
                }
                // std::cout << _pointArray[i] << " -> " << _pointArray[(i + 1) % n] << std::endl;
                // std::cout << "intersections: " << intersections[0] << std::endl;
                st.insert(intersections[0]);
            }
            else if (intersections.size() == 2)
            {
                // overlaps with the ray, which means point is on the polygon edge
                std::cout << "OnPolygonEdge" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::OnPolygonEdge)];
            }
        }
        ret = (st.size() % 2 == 1) ? PolygonTestResult::InPolygon : PolygonTestResult::OutsidePolygon;
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

        std::cout << std::endl
                  << " clipped is";
        PrintPolygon(clipped);
        std::cout << std::endl;
    }

private:
    POINTARRAY &_pointArray;
};