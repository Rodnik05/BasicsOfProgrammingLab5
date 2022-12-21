#pragma once
#include <string>
#include "ArgumentClass.h"

template <typename T>
struct LongKeyHashFunction {
    std::size_t operator() (std::string LongName) const {
        std::size_t Hash1 = std::hash<std::string>()(LongName);
        return Hash1;
    }
};


template <typename T>
struct ShortKeyHashFunction {
    std::size_t operator() (char ShortName) const {
        std::size_t Hash1 = std::hash<char>()(ShortName);
        return Hash1;
    }
};
