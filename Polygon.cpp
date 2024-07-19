#include <array>
#include <vector>
#include <deque>
#include <iostream>
#include <stack>
#include "PolygonT.h"
#include "traits.h"
// To compile with g++:
//     g++ -o Polygon.exe Polygon.cpp
// The functions to implement are:
// PolygonT.h
//  SegmentIntersection,
//  PolygonT::InPolygonTest, and
//  PolygonT::ClipSegments
//
// SegmentIntersection:
//  calculate the intersection(s) of two line segments
// PolygonT::InPolygonTest:
//  return the status of a point with respect to the polygon. It can be inside, outside or on the polygon edge.
// PolygonT::ClipSegments
//  use the polygon to clip a path, or a consecutive set of points which define a set of line segments by each pair of
//  adjacent points of the path
//
// Polygon.cpp
//  Enhance the code in the testing classes that specialize PolygonT as needed

auto PolygonTest() -> void
{
     {
          using UIntPoint = Point2DT<uint64_t>;
          std::vector<UIntPoint> pointArray = {{0, 0}, {3, 0}, {1, 4}, {1, 5}, {0, 2}};
          PolygonT<std::vector<UIntPoint>> polygon(pointArray);
          polygon.InPolygonTest(UIntPoint(2, 2));
     }
     {
          using doublePoint = Point2DT<double>;
          std::deque<doublePoint> pointArray = {{-1, -1}, {3, -2}, {2, 1}, {4, 5}, {2, 3}};
          PolygonT<std::deque<doublePoint>> polygon(pointArray);
          polygon.InPolygonTest(doublePoint(2, 3));
     }
     {
          using IntPoint = Point2DT<int>;
          std::array<IntPoint, 5> pointArray = {{{-1, -1}, {3, -2}, {2, 1}, {4, 5}, {-2, 2}}};
          PolygonT<std::array<IntPoint, 5>> polygon(pointArray);
          polygon.InPolygonTest(IntPoint(2, 3));
     }
     {
          using doublePoint = PointXYT<double>;
          std::deque<doublePoint> pointArray = {{-2.0, -2.0}, {3.0, -2.0}, {2, 1}, {4, 5}, {3, 3}};
          PolygonT<std::deque<doublePoint>> polygon(pointArray);
          polygon.InPolygonTest(doublePoint(2, 3));
     }
     {
          using IntPoint = Point2DT<int>;
          std::vector<IntPoint> pointArray = {{-1, -1}, {3, -2}, {2, 1}, {4, 5}, {-2, 2}};
          PolygonT<std::vector<IntPoint>> polygon(pointArray);
          std::vector<IntPoint> toclip = {{-4, 3}, {0, 0}, {1, 0}, {4, 3}, {1, 5}}, clipped;
          polygon.ClipSegments(toclip, clipped);
     }
}

int main()
{
     PolygonTest();
     return 0;
}
