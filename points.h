#pragma once

#include <iostream>

template <typename COORDTYPE>
class Point2DT
{
public:
     Point2DT() = default;
     Point2DT(COORDTYPE x, COORDTYPE y) : _point{x, y} {}
     Point2DT(const Point2DT &pt)
     {
          std::memcpy(_point.data(), pt._point.data(), 2 * sizeof(COORDTYPE));
     }
     Point2DT &operator=(const Point2DT &pt)
     {
          if (this != &pt)
          {
               std::memcpy(_point.data(), pt._point.data(), 2 * sizeof(COORDTYPE));
          }
          return *this;
     }

     bool operator==(const Point2DT &other) const
     {
          return _point[0] == other.X() && _point[1] == other.Y();
     }
     bool operator<(const Point2DT &other) const
     {
          return _point[0] < other.X() || _point[0] == other.X() && _point[1] < other.Y();
     }
     virtual ~Point2DT() = default;
     friend std::ostream &operator<<(std::ostream &stream, const Point2DT &obj)
     {
          stream << obj.X() << "," << obj.Y();
          return stream;
     }
     auto X() const -> COORDTYPE { return _point[0]; }
     auto Y() const -> COORDTYPE { return _point[1]; }

private:
     std::array<COORDTYPE, 2> _point;
};

template <typename COORDTYPE>
class PointXYT
{
public:
     PointXYT() = default;
     PointXYT(COORDTYPE ptx, COORDTYPE pty) : x{ptx}, y{pty} {}
     PointXYT(const PointXYT<COORDTYPE> &pt)
     {
          x = pt.x;
          y = pt.y;

     }
     PointXYT &operator=(const PointXYT &pt)
     {
          if (this != &pt)
          {
               x = pt.x;
               y = pt.y;
          }
          return *this;
     }
     bool operator==(const PointXYT &other) const
     {
          return x == other.x && y == other.y;
     }
     bool operator<(const PointXYT &other) const
     {
          return x < other.x || x == other.x && y < other.y;
     }
     virtual ~PointXYT() = default;
     friend std::ostream &operator<<(std::ostream &stream, const PointXYT &obj)
     {
          stream << obj.x << "," << obj.y;
          return stream;
     }

     COORDTYPE x, y;
};
