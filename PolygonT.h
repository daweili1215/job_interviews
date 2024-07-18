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

enum class PolygonTestResult {
#define ENUM_ITEM(x) x,
    POLYGONINOUTSTATUS(ENUM_ITEM)
#undef ENUM_ITEM
    STATECOUNT
};
const std::array<std::string,static_cast<int>(PolygonTestResult::STATECOUNT)> _enumItemStrings = {{
    #define ITEM_STRING(x) #x,
    POLYGONINOUTSTATUS(ITEM_STRING)
    #undef ITEM_STRING
}};

template<typename POINTARRAY>
auto PrintPolygon( const POINTARRAY &polygon) ->void
{
    using POINTTYPE = typename POINTARRAY::value_type;
    std::for_each( polygon.cbegin(), polygon.cend(), [](const POINTTYPE & point ) { std::cout << point << ", ";});
}
// check if point q is on line segment p to r
template< typename T>
auto OnSegment(const T& p, const T& q, const T& r) {
    if (q.get_x() <= std::max(p.get_x(), r.get_x()) && q.get_x() >= std::min(p.get_x(), r.get_x()) &&
        q.get_y() <= std::max(p.get_y(), r.get_y()) && q.get_y() >= std::min(p.get_y(), r.get_y())) {
        return true;
    }
    return false;
}

// get orientation of ordered triplet (p, q, r)
template< typename T >
auto orientation(const T& p, const T& q, const T& r) {
    auto val = (q.get_y() - p.get_y()) * (r.get_x() - q.get_x()) - (q.get_x() - p.get_x()) * (r.get_y() - q.get_y());
    if (val == 0) return 0;  // collinear
    return (val > 0) ? 1 : 2;  // clock or counterclock wise
}

template< typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
typename std::enable_if<
    std::is_same<T, int>::value || std::is_same<T, uint64_t>::value, void>::type
SegmentIntersection( const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE>& intersectons)
{
    // Find the four orientations needed for general and special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4) {
        // Compute the intersection point
        auto a1 = static_cast<int64_t>(q1.get_y()) - static_cast<int64_t>(p1.get_y());
        auto b1 = static_cast<int64_t>(p1.get_x()) - static_cast<int64_t>(q1.get_x());
        auto c1 = a1 * static_cast<int64_t>(p1.get_x()) + b1 * static_cast<int64_t>(p1.get_y());

        auto a2 = static_cast<int64_t>(q2.get_y()) - static_cast<int64_t>(p2.get_y());
        auto b2 = static_cast<int64_t>(p2.get_x()) - static_cast<int64_t>(q2.get_x());
        auto c2 = a2 * static_cast<int64_t>(p2.get_x()) + b2 * static_cast<int64_t>(p2.get_y());

        auto determinant = a1 * b2 - a2 * b1;
        if (determinant == 0) {
            // The lines are parallel. This case is handled by the collinear case later.
            return;
        } else {
            auto x = (b2 * c1 - b1 * c2) / determinant;
            auto y = (a1 * c2 - a2 * c1) / determinant;
            POINTTYPE intersection = {static_cast<T>(x), static_cast<T>(y)};
            if (OnSegment(p1, intersection, q1) && OnSegment(p2, intersection, q2)) {
                intersectons.push_back(intersection);
            }
            return;
        }
    }

    // Special cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && OnSegment(p1, p2, q1)) return intersectons.push_back(p2);

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && OnSegment(p1, q2, q1)) return intersectons.push_back(q2);

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && OnSegment(p2, p1, q2)) return intersectons.push_back(p1);

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && OnSegment(p2, q1, q2)) return intersectons.push_back(q1);

    // Check for overlapping segments
    if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0) {
        std::vector<POINTTYPE> overlap_points;

        if (OnSegment(p1, p2, q1)) overlap_points.push_back(p2);
        if (OnSegment(p1, q2, q1)) overlap_points.push_back(q2);
        if (OnSegment(p2, p1, q2)) overlap_points.push_back(p1);
        if (OnSegment(p2, q1, q2)) overlap_points.push_back(q1);

        std::sort(overlap_points.begin(), overlap_points.end(), [](const POINTTYPE& a, const POINTTYPE& b) {
            return (a.get_x() < b.get_x()) || (a.get_x() == b.get_x() && a.get_y() < b.get_y());
        });

        auto it = std::unique(overlap_points.begin(), overlap_points.end());
        overlap_points.resize(std::distance(overlap_points.begin(), it));

        intersectons = overlap_points;
        return;
    }
}

template< typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
typename std::enable_if<
    std::is_same<T, double>::value, void>::type
SegmentIntersection( const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2, std::vector<POINTTYPE>& intersectons)
{
    // Find the four orientations needed for general and special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);



    // General case
    if (o1 != o2 && o3 != o4) {
        // Compute the intersection point
        auto a1 = q1.get_y() - p1.get_y();
        auto b1 = p1.get_x() - q1.get_x();
        auto c1 = a1 * p1.get_x() + b1 * p1.get_y();

        auto a2 = q2.get_y() - p2.get_y();
        auto b2 = p2.get_x() - q2.get_x();
        auto c2 = a2 * p2.get_x() + b2 * p2.get_y();

        auto determinant = a1 * b2 - a2 * b1;
        if (std::fabs(determinant) < 1e-5) {
            // The lines are parallel. This case is handled by the collinear case later.
            return;
        } else {
            auto x = (b2 * c1 - b1 * c2) / determinant;
            auto y = (a1 * c2 - a2 * c1) / determinant;
            POINTTYPE intersection = {x, y};
            if (OnSegment(p1, intersection, q1) && OnSegment(p2, intersection, q2)) {
                intersectons.push_back(intersection);
            }
            return;
        }
    }

    // Special cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && OnSegment(p1, p2, q1)) return intersectons.push_back(p2);

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && OnSegment(p1, q2, q1)) return intersectons.push_back(q2);

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && OnSegment(p2, p1, q2)) return intersectons.push_back(p1);

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && OnSegment(p2, q1, q2)) return intersectons.push_back(q1);

    // Check for overlapping segments
    if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0) {
        std::vector<POINTTYPE> overlap_points;

        if (OnSegment(p1, p2, q1)) overlap_points.push_back(p2);
        if (OnSegment(p1, q2, q1)) overlap_points.push_back(q2);
        if (OnSegment(p2, p1, q2)) overlap_points.push_back(p1);
        if (OnSegment(p2, q1, q2)) overlap_points.push_back(q1);

        std::sort(overlap_points.begin(), overlap_points.end(), [](const POINTTYPE& a, const POINTTYPE& b) {
            return (a.get_x() < b.get_x()) || (a.get_x() == b.get_x() && a.get_y() < b.get_y());
        });

        auto it = std::unique(overlap_points.begin(), overlap_points.end());
        overlap_points.resize(std::distance(overlap_points.begin(), it));

        intersectons = overlap_points;
        return;
    }
}

template <typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
typename std::enable_if<
    std::is_same<T, int>::value || std::is_same<T, uint64_t>::value, void>::type
get_sum(POINTTYPE p1, POINTTYPE p2) {
    T sum_x = p1.x + p2.x;
    T sum_y = p1.y + p2.y;
    std::cout << "Sum of x coordinates: " << sum_x << std::endl;
    std::cout << "Sum of y coordinates: " << sum_y << std::endl;
}
/*
// Potencially needed function
template< typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
typename std::enable_if<
    std::is_same<T, int>::value || std::is_same<T, uint64_t>::value, void>::type
SegmentIntersection( const POINTTYPE &p1, const POINTTYPE &q1, const POINTTYPE &p2, const POINTTYPE &q2,
                            std::vector<POINTTYPE>& intersectons)
{  
    // Calculate the intersection point
    auto a1 = static_cast<int64_t>(q1.get_y()) - static_cast<int64_t>(p1.get_y());
    auto b1 = static_cast<int64_t>(p1.get_x()) - static_cast<int64_t>(q1.get_x());
    auto c1 = a1 * static_cast<int64_t>(p1.get_x()) + b1 * static_cast<int64_t>(p1.get_y());

    auto a2 = static_cast<int64_t>(q2.get_y()) - static_cast<int64_t>(p2.get_y());
    auto b2 = static_cast<int64_t>(p2.get_x()) - static_cast<int64_t>(q2.get_x());
    auto c2 = a2 * static_cast<int64_t>(p2.get_x()) + b2 * static_cast<int64_t>(p2.get_y());

    auto determinant = a1 * b2 - a2 * b1;

    if (determinant != 0) {
        // The lines are not parallel, so they intersect at a single point
        //std::cout << "The lines intersect at a single point" << std::endl;
        auto x = (b2 * c1 - b1 * c2) / determinant;
        auto y = (a1 * c2 - a2 * c1) / determinant;
        POINTTYPE intersection(static_cast<T>(x), static_cast<T>(y));
        if (OnSegment(p1, intersection, q1) && OnSegment(p2, intersection, q2)) {
            intersectons.push_back(intersection);
        }
    } else {
        // The lines are parallel or collinear
        if (orientation(p1, q1, p2) == 0 && orientation(p1, q1, q2) == 0) {
            // The lines are collinear, check for overlap
            if (OnSegment(p1, p2, q1)) intersectons.push_back(p2);
            if (OnSegment(p1, q2, q1)) intersectons.push_back(q2);
            if (OnSegment(p2, p1, q2)) intersectons.push_back(p1);
            if (OnSegment(p2, q1, q2)) intersectons.push_back(q1);

            // Sort and remove duplicates
            std::sort(intersectons.begin(), intersectons.end(), [](const auto& a, const auto& b) {
                return a.get_x() < b.get_x() || (a.get_x() == b.get_x() && a.get_y() < b.get_y());
            });
                intersectons.erase(std::unique(intersectons.begin(), intersectons.end()), intersectons.end());

            // If we have more than 2 points, we have an overlap
            if (intersectons.size() > 2) {
                intersectons = {intersectons.front(), intersectons.back()};
            }
        }
    }
}

template <typename POINTTYPE, typename T = get_coordinate_type_t<POINTTYPE>>
typename std::enable_if<
std::is_same<T, double>::value, void>::type
SegmentIntersection( const POINTTYPE& p1, const POINTTYPE& q1, const POINTTYPE& p2, const POINTTYPE& q2, std::vector<POINTTYPE>& intersectons) {
    // Calculate the intersection point
    auto a1 = q1.get_y() - p1.get_y();
    auto b1 = p1.get_x() - q1.get_x();
    auto c1 = a1 * p1.get_x() + b1 * p1.get_y();

    auto a2 = q2.get_y() - p2.get_y();
    auto b2 = p2.get_x() - q2.get_x();
    auto c2 = a2 * p2.get_x() + b2 * p2.get_y();

    auto determinant = a1 * b2 - a2 * b1;

    if (determinant != 0) {
        //std::cout << "The lines intersect at a single point" << std::endl;
        //std::cout << "determ: " << determinant << std::endl;
        // The lines are not parallel, so they intersect at a single point
        auto x = (b2 * c1 - b1 * c2) / determinant;
        auto y = (a1 * c2 - a2 * c1) / determinant;
        //std::cout << "x: " << x << " y: " << y << std::endl;
        POINTTYPE intersection(x, y);
        if (OnSegment(p1, intersection, q1) && OnSegment(p2, intersection, q2)) {
            intersectons.push_back(intersection);
        }
    } else {
        // The lines are parallel or collinear
        if (orientation(p1, q1, p2) == 0 && orientation(p1, q1, q2) == 0) {
            // The lines are collinear, check for overlap
            if (OnSegment(p1, p2, q1)) intersectons.push_back(p2);
            if (OnSegment(p1, q2, q1)) intersectons.push_back(q2);
            if (OnSegment(p2, p1, q2)) intersectons.push_back(p1);
            if (OnSegment(p2, q1, q2)) intersectons.push_back(q1);

            // Sort and remove duplicates
            std::sort(intersectons.begin(), intersectons.end(), [](const auto& a, const auto& b) {
                return a.get_x() < b.get_x() || (a.get_x() == b.get_x() && a.get_y() < b.get_y());
            });
                intersectons.erase(std::unique(intersectons.begin(), intersectons.end()), intersectons.end());

            // If we have more than 2 points, we have an overlap
            if (intersectons.size() > 2) {
                    intersectons = {intersectons.front(), intersectons.back()};
            }
        }
    }
}
*/

template< typename POINTARRAY >
class PolygonT
{
public:
    using POINTTYPE = typename POINTARRAY::value_type;
    PolygonT( POINTARRAY &points ) : _pointArray( points) {}
    virtual ~PolygonT() = default;
    auto InPolygonTest( const POINTTYPE &point ) const -> std::string
    {
        PolygonTestResult ret = PolygonTestResult::UNKNOWN;
        // TODO: implement this function from here and changed the ret to the correct status

        //////////////////////////////////////////
        int n = _pointArray.size();
        // If there are less than 3 points, it is not a polygon, return OutsidePolygon for simplicity
        if (n < 3) return _enumItemStrings[static_cast<int>(PolygonTestResult::OutsidePolygon)];

        std::cout << point << " in polygon ";
        PrintPolygon( _pointArray );
        std::cout << " is ";

        auto mx = point.get_x();
        for (auto i = 0; i < n; ++i) {
            mx = std::max(mx, _pointArray[i].get_x());
        }
        POINTTYPE extreme(10 * mx, point.get_y()); // A point far away to the right

        // Count intersections of the above line with polygon sides
        auto st = std::set<POINTTYPE>{};
        for (auto i = 0; i < n; ++i) {
            auto next = _pointArray[(i + 1) % n];
            auto intersections = std::vector<POINTTYPE>();
            SegmentIntersection(_pointArray[i], _pointArray[(i + 1) % n], point, extreme, intersections);
            if (intersections.size() == 1) {
                if (point == intersections[0]) {
                    std::cout << "OnPolygonEdge" << std::endl;
                    return _enumItemStrings[static_cast<int>(PolygonTestResult::OnPolygonEdge)];
                }
                std::cout << _pointArray[i] << " -> " << _pointArray[(i + 1) % n] << std::endl;
                std::cout << "intersections: " << intersections[0] << std::endl;
                st.insert(intersections[0]);
            } else if (intersections.size() == 2) {
                // overlaps with the ray, which means point is on the polygon edge
                std::cout << "OnPolygonEdge" << std::endl;
                return _enumItemStrings[static_cast<int>(PolygonTestResult::OnPolygonEdge)];
            }
        }
        std::cout << "count: " << st.size() << std::endl;
        ret = (st.size() % 2 == 1) ? PolygonTestResult::InPolygon : PolygonTestResult::OutsidePolygon;
        std::cout << _enumItemStrings[static_cast<int>(ret)] << std::endl;
        return _enumItemStrings[static_cast<int>(ret)];
    }
    auto ClipSegments( const POINTARRAY &tobeclippedpath, POINTARRAY &clipped ) -> void
    {   // tobeclippedpath is the line segment list that adjacent points forms a line segment
        std::cout << "use ";
        PrintPolygon( _pointArray );
        std::cout << " to clip ";
        PrintPolygon(tobeclippedpath);
        // TODO: implement a function to clip segments and return the clipped segments with 'clipped'

        std::cout << std::endl << " clipped is";
        PrintPolygon(clipped);
        std::cout << std::endl;
    }
private:
    POINTARRAY &_pointArray;
};