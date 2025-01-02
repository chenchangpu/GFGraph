#pragma once
#include <string>
#include <vector>
#include <array>

template <typename T>
std::string ToString(const T& x) {
    if constexpr (std::is_pointer_v<T>) {
        return "&" + ToString(*x);
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(x);
    } else {
        return std::string(x);
    }
}

template <typename K, typename V>
std::string ToString(const std::pair<K, V>& p) {
    return "(" + ToString(p.first) + "," + ToString(p.second) + ")";
}

template <template<typename, typename, typename...>class hashmap_t,
          typename K, typename V, typename... args_t,
          typename str_func_t = decltype(ToString<K, V>)>
std::string HashmapToString(const hashmap_t<K, V, args_t...>& hashmap,
                        str_func_t str_func = ToString<K, V>) {
    std::string str = "[";
    size_t sz = hashmap.size();
    for (const auto& p: hashmap) {
        str += str_func(p);
        if (--sz != 0) {
            str += ",";
        }
    }
    return str += "]";
}

template <typename T, typename str_func_t = decltype(ToString<T>)>
std::string VecToString(const std::vector<T> &vec,
                        str_func_t str_func = ToString<T>) {
    std::string str = "[";
    size_t sz = vec.size();
    for (size_t i = 0; i < sz; ++i) {
        str += str_func(vec[i]);
        if (i < sz - 1) {
            str += ",";
        }
    }

    return str += "]";
}

template <typename T, size_t N, typename str_func_t = decltype(ToString<T>)>
std::string ArrayToString(const std::array<T, N> &vec,
                        str_func_t str_func = ToString<T>) {
    std::string str = "[";
    size_t sz = vec.size();
    for (size_t i = 0; i < N; ++i) {
        str += str_func(vec[i]);
        if (i < sz - 1) {
            str += ",";
        }
    }

    return str += "]";
}