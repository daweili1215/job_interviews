#pragma once

#include <algorithm>
#include <iostream>
#include <type_traits>
#include <limits>
#include "points.h"

// Type trait to check if T is of type Point2DT
template <typename T>
struct is_Point2DT : std::false_type
{
};

template <typename T>
struct is_Point2DT<Point2DT<T>> : std::true_type
{
};

template <typename T>
static constexpr bool is_Point2DT_v = is_Point2DT<T>::value;

// Type trait to check if T is of type PointXYT
template <typename T>
struct is_PointXYT : std::false_type
{
};

template <typename T>
struct is_PointXYT<PointXYT<T>> : std::true_type
{
};

template <typename T>
static constexpr bool is_PointXYT_v = is_PointXYT<T>::value;

// Type trait to check if T has an index operator
template <typename T, typename = std::void_t<>>
struct has_indexer : std::false_type
{
};

template <typename T>
struct has_indexer<T, std::void_t<decltype(std::declval<T>()[std::declval<std::size_t>()])>> : std::true_type
{
};

template <typename T>
constexpr auto has_indexer_v = has_indexer<T>::value;

// Define the is_one_of type trait
template <typename T, typename... Types>
struct is_one_of : std::false_type
{
};

template <typename T, typename First, typename... Rest>
struct is_one_of<T, First, Rest...>
    : std::conditional<std::is_same<T, First>::value, std::true_type, is_one_of<T, Rest...>>::type
{
};

template <typename T, typename... Types>
constexpr bool is_one_of_v = is_one_of<T, Types...>::value;

// Define a type trait to extract the type T from POINTTYPE<T>
template <typename T>
struct get_coordinate_type;

template <typename T>
struct get_coordinate_type<Point2DT<T>>
{
  using type = T;
};

template <typename T>
struct get_coordinate_type<PointXYT<T>>
{
  using type = T;
};

template <typename T>
using get_coordinate_type_t = typename get_coordinate_type<T>::type;

// Define a type trait to extract x, y coordiates from Point
template <typename Point>
std::enable_if_t<is_Point2DT_v<Point>, get_coordinate_type_t<Point>> get_x(const Point &p)
{
  return p.X();
};

template <typename Point>
std::enable_if_t<is_PointXYT_v<Point>, get_coordinate_type_t<Point>> get_x(const Point &p)
{
  return p.x;
};

template <typename Point>
std::enable_if_t<is_Point2DT_v<Point>, get_coordinate_type_t<Point>> get_y(const Point &p)
{
  return p.Y();
};

template <typename Point>
std::enable_if_t<is_PointXYT_v<Point>, get_coordinate_type_t<Point>> get_y(const Point &p)
{
  return p.y;
};

// type traits to fix the type of the coordinates to either double or int64_t
template <typename Point, typename T = get_coordinate_type_t<Point>>
typename std::enable_if_t<!std::is_integral<T>::value, double> fix_x(const Point &p)
{
  return get_x(p);
};

template <typename Point, typename T = get_coordinate_type_t<Point>>
typename std::enable_if_t<!std::is_integral<T>::value, double> fix_y(const Point &p)
{
  return get_y(p);
};

template <typename Point, typename T = get_coordinate_type_t<Point>>
typename std::enable_if_t<std::is_integral<T>::value, int64_t> fix_x(const Point &p)
{
  return static_cast<int64_t>(get_x(p));
};

template <typename Point, typename T = get_coordinate_type_t<Point>>
typename std::enable_if_t<std::is_integral<T>::value, int64_t> fix_y(const Point &p)
{
  return static_cast<int64_t>(get_y(p));
};

// type trait to check if a type is an unsigned integer
template<typename T>
struct is_unsigned_integer
    : std::integral_constant<bool, std::is_integral<T>::value && std::is_unsigned<T>::value> {};

template<typename T>
constexpr bool is_unsigned_integer_v = is_unsigned_integer<T>::value;

// type traits to create a positve or negative extreme value
template <typename T>
typename std::enable_if_t<!is_unsigned_integer_v<T>, T> create_max(const T &p)
{
  return std::min(p < 0 ? -p : p, std::numeric_limits<T>::max());
}

template <typename T>
typename std::enable_if_t<is_unsigned_integer_v<T>, T> create_max(const T &p)
{
  return std::min(p, std::numeric_limits<T>::max());
}

template <typename T>
typename std::enable_if_t<is_unsigned_integer_v<T>, T> create_min(const T &p)
{
  return 0;
}

template <typename T>
typename std::enable_if_t<std::is_same<T, double>::value, T> create_min(const T &p)
{
  return std::max(p < 0 ? p : -p, -std::numeric_limits<T>::max());
}

template <typename T>
typename std::enable_if_t<std::is_same<T, int>::value, T> create_min(const T &p)
{
  return std::max(p < 0 ? p : -p, std::numeric_limits<T>::min());
}
