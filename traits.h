#pragma once

#include <iostream>
#include <type_traits>
#include "points.h"

// Type trait to check if T is of type Point2DT
template<typename T>
struct is_Point2DT : std::false_type {};

template<typename T>
struct is_Point2DT<Point2DT<T>> : std::true_type {
  using type = T;
};

// Type trait to check if T is of type PointXYT
template<typename T>
struct is_PointXYT : std::false_type {};

template<typename T>
struct is_PointXYT<PointXYT<T>> : std::true_type {
  using type = T;
};

// Define the is_one_of type trait
template <typename T, typename... Types>
struct is_one_of : std::false_type {};

template <typename T, typename First, typename... Rest>
struct is_one_of<T, First, Rest...>
    : std::conditional<std::is_same<T, First>::value, std::true_type, is_one_of<T, Rest...>>::type {};

// Helper variable template for ease of use
template <typename T, typename... Types>
constexpr bool is_one_of_v = is_one_of<T, Types...>::value;

// Define a type trait to extract the type T from point<T>
template <typename T>
struct get_coordinate_type;

template <typename T>
struct get_coordinate_type<Point2DT<T>> {
    using type = T;
};

template <typename T>
struct get_coordinate_type<PointXYT<T>> {
    using type = T;
};
// Alias template for convenience
template <typename T>
using get_coordinate_type_t = typename get_coordinate_type<T>::type;


