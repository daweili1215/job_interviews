#include <array>
#include <vector>
#include <deque>
#include <iostream>
#include "PolygonT.h"
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
template< typename COORDTYPE >
class Point2DT
{
public:
     Point2DT() = default;
     Point2DT(COORDTYPE x, COORDTYPE y ) { _point={x,y};}
     Point2DT &operator=( const Point2DT& pt ) {
          if ( this != &pt ) {
               std::memcpy( _point.get(), pt._point.get(), 2*sizeof(COORDTYPE));
          }
     }
     virtual ~Point2DT() = default;
     friend std::ostream& operator<<(std::ostream& stream, const Point2DT& obj)
    {
        stream << obj.X() << "," << obj.Y();
        return stream;
    }
     auto X() const -> COORDTYPE { return _point[0];}
     auto Y() const -> COORDTYPE { return _point[1];}
private:
     std::array<COORDTYPE,2> _point;
};

template< typename COORDTYPE >
class PointXYT
{
public:
     PointXYT() = default;
     PointXYT(COORDTYPE ptx, COORDTYPE pty ) : x(ptx), y(pty) {}
     PointXYT &operator=( const PointXYT& pt ) {
          if ( this != &pt ) {
               x = pt.x;
               y = pt.y;
          }
     }
     virtual ~PointXYT() = default;
     friend std::ostream& operator<<(std::ostream& stream, const PointXYT& obj)
    {
        stream << obj.x << "," << obj.y;
        return stream;
    }

     COORDTYPE x,y;
};

auto PolygonTest() ->void 
{
    {
    using UIntPoint = Point2DT<uint64_t>;
    std::vector<UIntPoint> pointArray= { {0,0}, {3, 0}, {1,4}, {1, 5}, {0, 2} };
    PolygonT<std::vector<UIntPoint>> polygon(pointArray);
    polygon.InPolygonTest( UIntPoint(2,2));
    }
    {
    using doublePoint = Point2DT<double>;
    std::deque<doublePoint> pointArray ={ {-1,-1}, {3, -2}, {2, 1}, {4, 5}, {2, 3} };
    PolygonT<std::deque<doublePoint>> polygon(pointArray);
    polygon.InPolygonTest( doublePoint(2,3));
    }
    {
    using IntPoint = Point2DT<int>;
    std::array<IntPoint,5> pointArray={ { {-1,-1}, {3, -2}, {2, 1}, {4, 5}, {-2, 2} } };
    PolygonT<std::array<IntPoint,5>> polygon(pointArray);
    polygon.InPolygonTest( IntPoint(2,3));
    }
    {
    using doublePoint = PointXYT<double>;
    std::deque<doublePoint> pointArray ={ {-2,-2}, {3, -2}, {2, 1}, {4, 5}, {3, 3} };
    PolygonT<std::deque<doublePoint>> polygon(pointArray);
    polygon.InPolygonTest( doublePoint(2,3));
    }
    {
    using IntPoint = Point2DT<int>;
    std::vector<IntPoint> pointArray={ {-1,-1}, {3, -2}, {2, 1}, {4, 5}, {-2, 2} };
    PolygonT<std::vector<IntPoint>> polygon(pointArray);
    std::vector<IntPoint> toclip={ {-4,3}, {0, 0}, {1, 0}, {4, 3}, {1, 5} }, clipped;
    polygon.ClipSegments( toclip, clipped);
    }
}

int main()
{
    PolygonTest();
}