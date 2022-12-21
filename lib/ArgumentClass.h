#pragma once
#include <vector>
#include <deque>
#include <variant>
#include <string>
#include <string_view>
#include <typeinfo>

template <typename T>
class Argument {
public:
    T* Storage;
    std::variant<std::shared_ptr<std::vector<T>>, std::shared_ptr<std::deque<T>>, nullptr_t> Storehouse;
    std::variant<std::vector<T>, std::deque<T>> Values;
    std::string LongKey;
    char ShortKey;

    Argument()
            : Storage(nullptr)
            , Storehouse(nullptr)
            , LongKey("")
            , ShortKey('\0')
    {
        if (typeid(T) == typeid(bool)) {
            Values = std::deque<T>();
        } else {
            Values = std::vector<T>();
        }
    }



    ~Argument() = default;
};
