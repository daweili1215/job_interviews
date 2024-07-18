#pragma once

#include <iostream>
#include <type_traits>
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
