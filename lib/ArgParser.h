#pragma once

#include <vector>
#include <string>
#include <deque>
#include <variant>
#include <optional>
#include <unordered_map>
#include <memory>

#include "ArgumentClass.h"
#include "HashFunctions.h"

namespace ArgumentParser {

class ArgParser {
    
    const char* ParserName;

public:
    std::unordered_map<std::string, std::shared_ptr<Argument<int>>, LongKeyHashFunction<int>> IntsByLongKey;
    std::unordered_map<char, std::shared_ptr<Argument<int>>, ShortKeyHashFunction<int>> IntsByShortKey;
    std::unordered_map<std::string, std::shared_ptr<Argument<std::string>>, LongKeyHashFunction<std::string>> StringsByLongKey;
    std::unordered_map<char, std::shared_ptr<Argument<std::string>>, ShortKeyHashFunction<std::string>> StringsByShortKey;
    std::unordered_map<std::string, std::shared_ptr<Argument<bool>>, LongKeyHashFunction<bool>> BoolsByLongKey;
    std::unordered_map<char, std::shared_ptr<Argument<bool>>, ShortKeyHashFunction<bool>> BoolsByShortKey;

    std::variant<std::shared_ptr<Argument<std::string>>, std::shared_ptr<Argument<int>>, std::shared_ptr<Argument<bool>>> LastElement;
    std::variant<std::shared_ptr<int>, std::shared_ptr<bool>, std::shared_ptr<std::string>> DefaultValue;
    std::size_t LeastAmountOfValues;




    explicit ArgParser(const char* name);

    explicit ArgParser(const std::string& name);

    const char* GetName();

    //TODO
    bool Parse(const std::vector<std::string>& argv);

    //TODO
    bool Parse(int argc, char** argv);

    ArgParser& MultiValue(std::size_t value = 0);

    ~ArgParser();

    ArgParser& AddIntArgument(char s, const std::string& str, const std::string& description = "");

    ArgParser& AddIntArgument(const std::string& str, const std::string& description = "");

    ArgParser& AddStringArgument(char s, const std::string& str, const std::string& description = "");

    ArgParser& AddStringArgument(const std::string& str, const std::string& description = "");

    ArgParser& AddFlag(char s, const std::string& str, const std::string& description = "");

    ArgParser& AddFlag(const std::string& str, const std::string& description = "");

    //TODO
    ArgParser& Positional();

    ArgParser& StoreValue(std::string& tmp);

    ArgParser& StoreValue(int& tmp);

    ArgParser& StoreValue(bool& tmp);

    ArgParser& StoreValues(std::vector<std::string>& tmp);

    ArgParser& StoreValues(std::deque<bool>& tmp);

    ArgParser& StoreValues(std::vector<int>& tmp);

    ArgParser& Default(std::variant<std::string, int, bool> default_value);

    std::optional<int> GetIntValue(const std::string& str, std::size_t index = 0);

    std::optional<std::string> GetStringValue(const std::string& str, std::size_t index = 0);

    std::optional<bool> GetFlag(const std::string& str, std::size_t index = 0);

    std::optional<int> GetIntValue(char str, std::size_t index = 0);

    std::optional<std::string> GetStringValue(char str, std::size_t index = 0);

    std::optional<bool> GetFlag(char str, std::size_t index = 0);

    void AddHelp(char s, const std::string& str, const std::string& description);

    std::string HelpDescription();

    bool Help() const;



};

} // namespace ArgumentParser