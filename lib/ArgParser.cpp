#include "ArgParser.h"
#include "ArgumentClass.h"
#include <cstring>
#include <exception>
#include <algorithm>
#include <stdexcept>
#include <ranges>
#include <iostream>


namespace ArgumentParser {

    //TODO
    bool ArgParser::Parse(const std::vector<std::string> &argv) {
        const char *delimiter = "=";
        for (int i = 1; i < argv.size(); i++) {
            char* str = strcpy(new char[argv[i].length() + 1], argv[i].c_str());
            char *token = std::strtok(str, delimiter);
            std::string key = token;
            token = std::strtok(nullptr, delimiter);
            std::string value = token;
            if (!key.empty()) {
                // if there are some arguments left
                if (!value.empty()) {
                    // if there is a value
                    if (key[0] == '-') {
                        if (key[1] == '-') {
                            // if it's a long key
                            key = key.substr(2, key.size());
                            if (std::count_if(value.begin(), value.end(),
                                              [](unsigned char c) { return std::isalpha(c); }) != 0
                                    ) {
                                // if there are some alphabetic characters
                                if (this->StringsByLongKey.contains(key)) {
                                    std::get<std::vector<std::string>>(this->StringsByLongKey[key]->Values).push_back(
                                            value);
                                } else {
                                    return false;
                                }
                            } else if (std::count_if(value.begin(), value.end(),
                                                     [](unsigned char c) { return std::isdigit(c); }) != 0) {
                                // if there are only decimal digits
                                if (this->IntsByLongKey.contains(key)) {
                                    std::get<std::vector<int>>(this->IntsByLongKey[key]->Values).push_back(
                                            std::stoi(value));
                                } else {
                                    return false;
                                }
                            } else {
                                throw std::invalid_argument("Invalid value");
                            }
                        } else {
                            // if it's a short key
                            if (std::count_if(value.begin(), value.end(),
                                              [](unsigned char c) { return std::isalpha(c); }) != 0
                                    ) {
                                // if there are some alphabetic characters
                                if (this->StringsByShortKey.contains(key[1])) {
                                    std::get<std::vector<std::string>>(
                                            this->StringsByShortKey[key[1]]->Values).push_back(value);
                                    if (this->StringsByShortKey[key[1]]->Storage != nullptr) {
                                        *this->StringsByShortKey[key[1]]->Storage = value;
                                    } else if (std::holds_alternative<nullptr_t>(this->StringsByShortKey[key[1]]->Storehouse)){
                                        this->StringsByShortKey[key[1]]->Storehouse = std::make_shared<std::vector<std::string>>();
                                    }
                                    if (std::holds_alternative<std::shared_ptr<std::vector<std::string>>>(this->StringsByShortKey[key[1]]->Storehouse)) {
                                        (std::get<std::shared_ptr<std::vector<std::string>>>(this->StringsByShortKey[key[1]]->Storehouse))->push_back(value);
                                    }
                                } else {
                                    return false;
                                }
                            } else if (std::count_if(value.begin(), value.end(),
                                                     [](unsigned char c) { return std::isdigit(c); }) != 0) {
                                // if there are only decimal digits
                                if (this->IntsByShortKey.contains(key[1])) {
                                    std::get<std::vector<int>>(this->IntsByShortKey[key[1]]->Values).push_back(
                                            std::stoi(value));
                                    if (this->IntsByShortKey[key[1]]->Storage != nullptr) {
                                        *this->IntsByShortKey[key[1]]->Storage = std::stoi(value);
                                    } else if (std::holds_alternative<nullptr_t>(this->IntsByShortKey[key[1]]->Storehouse)) {
                                        this->IntsByShortKey[key[1]]->Storehouse = std::make_shared<std::vector<int>>();
                                    }
                                    if (std::holds_alternative<std::shared_ptr<std::vector<int>>>(this->IntsByShortKey[key[1]]->Storehouse)) {
                                        (std::get<std::shared_ptr<std::vector<int>>>(this->IntsByShortKey[key[1]]->Storehouse))->push_back(std::stoi(value));
                                    }
                                } else {
                                    return false;
                                }
                            } else {
                                // there is some garbage
                                return false;
                            }
                        }
                    } else {
                        throw std::invalid_argument("Invalid key");
                    }
                } else {
                    if (key[0] == '-') {
                        if (key[1] == '-') {
                            // long key
                            key = key.substr(2, key.size());

                            // Check if this argument exists
                            if (this->IntsByLongKey.contains(key)) {
                                if (std::holds_alternative<std::vector<int>>(IntsByLongKey[key]->Values)) {
                                    if (std::get<std::vector<int>>(IntsByLongKey[key]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }

                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<int>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        IntsByLongKey[key]->Values = std::vector<int>();
                                        std::get<std::vector<int>>(IntsByLongKey[key]->Values).push_back(*std::get<std::shared_ptr<int>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else if (this->StringsByLongKey.contains(key)) {
                                if (std::holds_alternative<std::vector<std::string>>(StringsByLongKey[key]->Values)) {
                                    if (std::get<std::vector<std::string>>(StringsByLongKey[key]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<std::string>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        StringsByLongKey[key]->Values = std::vector<std::string>();
                                        std::get<std::vector<std::string>>(StringsByLongKey[key]->Values).push_back(*std::get<std::shared_ptr<std::string>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else if (this->BoolsByLongKey.contains(key)) {
                                if (std::holds_alternative<std::deque<bool>>(BoolsByLongKey[key]->Values)) {
                                    if (std::get<std::deque<bool>>(BoolsByLongKey[key]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<bool>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        BoolsByLongKey[key]->Values = std::deque<bool>();
                                        std::get<std::deque<bool>>(BoolsByLongKey[key]->Values).push_back(*std::get<std::shared_ptr<bool>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else {
                                return false;
                            }

                        } else {
                            // short key

                            // Check if this argument exists
                            if (this->IntsByShortKey.contains(key[1])) {
                                if (std::holds_alternative<std::vector<int>>(IntsByShortKey[key[1]]->Values)) {
                                    if (std::get<std::vector<int>>(IntsByShortKey[key[1]]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<int>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        IntsByShortKey[key[1]]->Values = std::vector<int>();
                                        std::get<std::vector<int>>(IntsByShortKey[key[1]]->Values).push_back(*std::get<std::shared_ptr<int>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }

                            } else if (this->StringsByShortKey.contains(key[1])) {
                                if (std::holds_alternative<std::vector<std::string>>(StringsByShortKey[key[1]]->Values)) {
                                    if (std::get<std::vector<std::string>>(StringsByShortKey[key[1]]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<std::string>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        StringsByShortKey[key[1]]->Values = std::vector<std::string>();
                                        std::get<std::vector<std::string>>(StringsByShortKey[key[1]]->Values).push_back(*std::get<std::shared_ptr<std::string>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else if (this->BoolsByShortKey.contains(key[1])) {
                                if (std::holds_alternative<std::deque<bool>>(BoolsByShortKey[key[1]]->Values)) {
                                    if (std::get<std::deque<bool>>(BoolsByShortKey[key[1]]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<bool>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        BoolsByShortKey[key[1]]->Values = std::deque<bool>();
                                        std::get<std::deque<bool>>(BoolsByShortKey[key[1]]->Values).push_back(*std::get<std::shared_ptr<bool>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else {
                                return false;
                            }
                        }
                    }  else {
                        //  invalid key
                        throw std::invalid_argument("Invalid key");
                    }
                }
            } else {
                // it should have some characters except "="
                throw std::invalid_argument("Invalid input");
            }
            delete[] str;
        }
        for (auto it = IntsByLongKey.begin(); it != IntsByLongKey.end(); it++) {
            if (std::holds_alternative<std::vector<int>>(it->second->Values)) {
                if (std::get<std::vector<int>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        for (auto it = IntsByShortKey.begin(); it != IntsByShortKey.end(); it++) {
            if (std::holds_alternative<std::vector<int>>(it->second->Values)) {
                if (std::get<std::vector<int>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }

        for (auto it = StringsByLongKey.begin(); it != StringsByLongKey.end(); it++) {
            if (std::holds_alternative<std::vector<std::string>>(it->second->Values)) {
                if (std::get<std::vector<std::string>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        for (auto it = StringsByShortKey.begin(); it != StringsByShortKey.end(); it++) {
            if (std::holds_alternative<std::vector<std::string>>(it->second->Values)) {
                if (std::get<std::vector<std::string>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }

        for (auto it = BoolsByLongKey.begin(); it != BoolsByLongKey.end(); it++) {
            if (std::holds_alternative<std::deque<bool>>(it->second->Values)) {
                if (std::get<std::deque<bool>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        for (auto it = BoolsByShortKey.begin(); it != BoolsByShortKey.end(); it++) {
            if (std::holds_alternative<std::deque<bool>>(it->second->Values)) {
                if (std::get<std::deque<bool>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        return true;

    }


    bool ArgParser::Parse(int argc, char **argv) {
        const char *delimiter = "=";
        for (int i = 1; i < argc; i++) {
            char *token = std::strtok(argv[i], delimiter);
            std::string key = token;
            token = std::strtok(nullptr, delimiter);
            std::string value = token;
            if (!key.empty()) {
                // if there are some arguments left
                if (!value.empty()) {
                    // if there is a value
                    if (key[0] == '-') {
                        if (key[1] == '-') {
                            // if it's a long key
                            key = key.substr(2, key.size());
                            if (std::count_if(value.begin(), value.end(),
                                              [](unsigned char c) { return std::isalpha(c); }) != 0
                                    ) {
                                // if there are some alphabetic characters
                                if (this->StringsByLongKey.contains(key)) {
                                    std::get<std::vector<std::string>>(this->StringsByLongKey[key]->Values).push_back(
                                            value);
                                } else {
                                    return false;
                                }
                            } else if (std::count_if(value.begin(), value.end(),
                                                     [](unsigned char c) { return std::isdigit(c); }) != 0) {
                                // if there are only decimal digits
                                if (this->IntsByLongKey.contains(key)) {
                                    std::get<std::vector<int>>(this->IntsByLongKey[key]->Values).push_back(
                                            std::stoi(value));
                                } else {
                                    return false;
                                }
                            } else {
                                throw std::invalid_argument("Invalid value");
                            }
                        } else {
                            // if it's a short key
                            if (std::count_if(value.begin(), value.end(),
                                              [](unsigned char c) { return std::isalpha(c); }) != 0
                                    ) {
                                // if there are some alphabetic characters
                                if (this->StringsByShortKey.contains(key[1])) {
                                    std::get<std::vector<std::string>>(
                                            this->StringsByShortKey[key[1]]->Values).push_back(value);
                                    if (this->StringsByShortKey[key[1]]->Storage != nullptr) {
                                        *this->StringsByShortKey[key[1]]->Storage = value;
                                    } else if (std::holds_alternative<nullptr_t>(this->StringsByShortKey[key[1]]->Storehouse)){
                                        this->StringsByShortKey[key[1]]->Storehouse = std::make_shared<std::vector<std::string>>();
                                    }
                                    if (std::holds_alternative<std::shared_ptr<std::vector<std::string>>>(this->StringsByShortKey[key[1]]->Storehouse)) {
                                        (std::get<std::shared_ptr<std::vector<std::string>>>(this->StringsByShortKey[key[1]]->Storehouse))->push_back(value);
                                    }
                                } else {
                                    return false;
                                }
                            } else if (std::count_if(value.begin(), value.end(),
                                                     [](unsigned char c) { return std::isdigit(c); }) != 0) {
                                // if there are only decimal digits
                                if (this->IntsByShortKey.contains(key[1])) {
                                    std::get<std::vector<int>>(this->IntsByShortKey[key[1]]->Values).push_back(
                                            std::stoi(value));
                                    if (this->IntsByShortKey[key[1]]->Storage != nullptr) {
                                        *this->IntsByShortKey[key[1]]->Storage = std::stoi(value);
                                    } else if (std::holds_alternative<nullptr_t>(this->IntsByShortKey[key[1]]->Storehouse)) {
                                        this->IntsByShortKey[key[1]]->Storehouse = std::make_shared<std::vector<int>>();
                                    }
                                    if (std::holds_alternative<std::shared_ptr<std::vector<int>>>(this->IntsByShortKey[key[1]]->Storehouse)) {
                                        (std::get<std::shared_ptr<std::vector<int>>>(this->IntsByShortKey[key[1]]->Storehouse))->push_back(std::stoi(value));
                                    }
                                } else {
                                    return false;
                                }
                            } else {
                                // there is some garbage
                                return false;
                            }
                        }
                    } else {
                        throw std::invalid_argument("Invalid key");
                    }
                } else {
                    if (key[0] == '-') {
                        if (key[1] == '-') {
                            // long key
                            key = key.substr(2, key.size());

                            // Check if this argument exists
                            if (this->IntsByLongKey.contains(key)) {
                                if (std::holds_alternative<std::vector<int>>(IntsByLongKey[key]->Values)) {
                                    if (std::get<std::vector<int>>(IntsByLongKey[key]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }

                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<int>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        IntsByLongKey[key]->Values = std::vector<int>();
                                        std::get<std::vector<int>>(IntsByLongKey[key]->Values).push_back(*std::get<std::shared_ptr<int>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else if (this->StringsByLongKey.contains(key)) {
                                if (std::holds_alternative<std::vector<std::string>>(StringsByLongKey[key]->Values)) {
                                    if (std::get<std::vector<std::string>>(StringsByLongKey[key]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<std::string>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        StringsByLongKey[key]->Values = std::vector<std::string>();
                                        std::get<std::vector<std::string>>(StringsByLongKey[key]->Values).push_back(*std::get<std::shared_ptr<std::string>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else if (this->BoolsByLongKey.contains(key)) {
                                if (std::holds_alternative<std::deque<bool>>(BoolsByLongKey[key]->Values)) {
                                    if (std::get<std::deque<bool>>(BoolsByLongKey[key]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<bool>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        BoolsByLongKey[key]->Values = std::deque<bool>();
                                        std::get<std::deque<bool>>(BoolsByLongKey[key]->Values).push_back(*std::get<std::shared_ptr<bool>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else {
                                return false;
                            }

                        } else {
                            // short key

                            // Check if this argument exists
                            if (this->IntsByShortKey.contains(key[1])) {
                                if (std::holds_alternative<std::vector<int>>(IntsByShortKey[key[1]]->Values)) {
                                    if (std::get<std::vector<int>>(IntsByShortKey[key[1]]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<int>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        IntsByShortKey[key[1]]->Values = std::vector<int>();
                                        std::get<std::vector<int>>(IntsByShortKey[key[1]]->Values).push_back(*std::get<std::shared_ptr<int>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }

                            } else if (this->StringsByShortKey.contains(key[1])) {
                                if (std::holds_alternative<std::vector<std::string>>(StringsByShortKey[key[1]]->Values)) {
                                    if (std::get<std::vector<std::string>>(StringsByShortKey[key[1]]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<std::string>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        StringsByShortKey[key[1]]->Values = std::vector<std::string>();
                                        std::get<std::vector<std::string>>(StringsByShortKey[key[1]]->Values).push_back(*std::get<std::shared_ptr<std::string>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else if (this->BoolsByShortKey.contains(key[1])) {
                                if (std::holds_alternative<std::deque<bool>>(BoolsByShortKey[key[1]]->Values)) {
                                    if (std::get<std::deque<bool>>(BoolsByShortKey[key[1]]->Values).size() < this->LeastAmountOfValues) {
                                        // This argument doesn't satisfy Multivalue requirements
                                        return false;
                                    } else {
                                        // it's got some values already
                                        continue;
                                    }
                                } else {
                                    // It doesn't have a value
                                    if (std::holds_alternative<std::shared_ptr<bool>>(this->DefaultValue)) {
                                        // Check if it has a default value
                                        BoolsByShortKey[key[1]]->Values = std::deque<bool>();
                                        std::get<std::deque<bool>>(BoolsByShortKey[key[1]]->Values).push_back(*std::get<std::shared_ptr<bool>>(this->DefaultValue));
                                    } else {
                                        return false;
                                    }
                                }
                            } else {
                                return false;
                            }
                        }
                    }  else {
                        //  invalid key
                        throw std::invalid_argument("Invalid key");
                    }
                }
            } else {
                // it should have some characters except "="
                throw std::invalid_argument("Invalid input");
            }
        }
        // check if all added argument are defined by iterating through maps
        for (auto it = IntsByLongKey.begin(); it != IntsByLongKey.end(); it++) {
            if (std::holds_alternative<std::vector<int>>(it->second->Values)) {
                if (std::get<std::vector<int>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        for (auto it = IntsByShortKey.begin(); it != IntsByShortKey.end(); it++) {
            if (std::holds_alternative<std::vector<int>>(it->second->Values)) {
                if (std::get<std::vector<int>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }

        for (auto it = StringsByLongKey.begin(); it != StringsByLongKey.end(); it++) {
            if (std::holds_alternative<std::vector<std::string>>(it->second->Values)) {
                if (std::get<std::vector<std::string>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        for (auto it = StringsByShortKey.begin(); it != StringsByShortKey.end(); it++) {
            if (std::holds_alternative<std::vector<std::string>>(it->second->Values)) {
                if (std::get<std::vector<std::string>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }

        for (auto it = BoolsByLongKey.begin(); it != BoolsByLongKey.end(); it++) {
            if (std::holds_alternative<std::deque<bool>>(it->second->Values)) {
                if (std::get<std::deque<bool>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        for (auto it = BoolsByShortKey.begin(); it != BoolsByShortKey.end(); it++) {
            if (std::holds_alternative<std::deque<bool>>(it->second->Values)) {
                if (std::get<std::deque<bool>>(it->second->Values).size() < LeastAmountOfValues) {
                    return false;
                } else {
                    continue;
                }
            } else {
                // WHAT?? HOW CAN IT BE WITHOUT VALUES?
                throw std::invalid_argument("WHAT?? HOW CAN IT BE WITHOUT VALUES?");
            }
        }
        return true;

    }

    ArgParser::ArgParser(
            const char *name)
            : ParserName(name), LeastAmountOfValues(1) {}

    ArgParser::ArgParser(
            const std::string &name)
            : ParserName(name.c_str()), LeastAmountOfValues(1) {}

    ArgParser::~ArgParser() {
        IntsByShortKey.clear();
        IntsByLongKey.clear();
        BoolsByShortKey.clear();
        BoolsByLongKey.clear();
        StringsByShortKey.clear();
        StringsByLongKey.clear();
    }

    const char *ArgumentParser::ArgParser::GetName() {
        return this->ParserName;
    }


    ArgParser& ArgParser::Default(std::variant<std::string, int, bool> default_value) {
        if (std::holds_alternative<std::string>(default_value)) {
            this->DefaultValue = std::make_shared<std::string>(std::get<std::string>(default_value));
        } else if (std::holds_alternative<int>(default_value)) {
            this->DefaultValue = std::make_shared<int>(std::get<int>(default_value));
        } else if (std::holds_alternative<bool>(default_value)) {
            this->DefaultValue = std::make_shared<bool>(std::get<bool>(default_value));
        } else {
            throw std::bad_variant_access();
        }
        return *this;
    }

    ArgParser& ArgParser::MultiValue(std::size_t value) {
        this->LeastAmountOfValues = value;
        return *this;
    }

    ArgParser& ArgParser::AddIntArgument(char s, const std::string &str, const std::string &description) {
        auto argument_sample = std::make_shared<Argument<int>>();

        LastElement = argument_sample;
        std::get<nullptr_t>(argument_sample->Storehouse) = nullptr;
        argument_sample->Storage = nullptr;
        argument_sample->LongKey = str;
        argument_sample->ShortKey = s;
        IntsByLongKey.insert(std::make_pair(str, argument_sample));
        IntsByShortKey.insert(std::make_pair(s, argument_sample));
        return *this;
    }

    ArgParser& ArgParser::AddIntArgument(const std::string &str, const std::string &description) {
        auto argument_sample = std::make_shared<Argument<int>>();
        LastElement = argument_sample;
        std::get<nullptr_t>(argument_sample->Storehouse) = nullptr;
        argument_sample->Storage = nullptr;
        argument_sample->LongKey = str;
        IntsByLongKey.insert(std::make_pair(str, argument_sample));
        return *this;
    }

    ArgParser& ArgParser::AddStringArgument(char s, const std::string &str, const std::string &description) {
        auto argument_sample = std::make_shared<Argument<std::string>>();
        LastElement = argument_sample;
        std::get<nullptr_t>(argument_sample->Storehouse) = nullptr;
        argument_sample->Storage = nullptr;
        argument_sample->LongKey = str;
        argument_sample->ShortKey = s;
        StringsByLongKey.insert(std::make_pair(str, argument_sample));
        StringsByShortKey.insert(std::make_pair(s, argument_sample));
        return *this;
    }

    ArgParser& ArgParser::AddStringArgument(const std::string &str, const std::string &description) {
        auto argument_sample = std::make_shared<Argument<std::string>>();
        LastElement = argument_sample;
        std::get<nullptr_t>(argument_sample->Storehouse) = nullptr;
        argument_sample->Storage = nullptr;
        argument_sample->LongKey = str;
        StringsByLongKey.insert(std::make_pair(str, argument_sample));
        return *this;
    }

    ArgParser& ArgParser::AddFlag(char s, const std::string &str, const std::string &description) {
        auto argument_sample = std::make_shared<Argument<bool>>();
        argument_sample->Storehouse = std::make_shared<std::deque<bool>>();
        LastElement = argument_sample;
        std::get<std::shared_ptr<std::deque<bool>>>(argument_sample->Storehouse) = nullptr;
        argument_sample->Storage = nullptr;
        argument_sample->LongKey = str;
        argument_sample->ShortKey = s;
        BoolsByLongKey.insert(std::make_pair(str, argument_sample));
        BoolsByShortKey.insert(std::make_pair(s, argument_sample));
        return *this;
    }

    ArgParser& ArgParser::AddFlag(const std::string &str, const std::string &description) {
        auto argument_sample = std::make_shared<Argument<bool>>();
        LastElement = argument_sample;
        std::get<nullptr_t>(argument_sample->Storehouse) = nullptr;
        argument_sample->Storage = nullptr;
        argument_sample->LongKey = str;
        BoolsByLongKey.insert(std::make_pair(str, argument_sample));
        return *this;
    }

    std::optional<int> ArgParser::GetIntValue(const std::string &str, std::size_t index) {
        std::shared_ptr<Argument<int>> argument_pointer = IntsByLongKey[str];
        if (argument_pointer != nullptr) {
            auto argument_values = std::get<std::vector<int>>(argument_pointer.get()->Values);
            if (index < argument_values.size()) {
                return argument_values[0];
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string> ArgParser::GetStringValue(const std::string &str, std::size_t index) {
        std::shared_ptr<Argument<std::string>> argument_pointer = StringsByLongKey[str];
        if (argument_pointer != nullptr) {
            auto argument_values = std::get<std::vector<std::string>>(argument_pointer.get()->Values);
            if (index < argument_values.size()) {
                return argument_values[index];
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    std::optional<bool> ArgParser::GetFlag(const std::string &str, std::size_t index) {
        std::shared_ptr<Argument<bool>> argument_pointer = BoolsByLongKey[str];
        if (argument_pointer != nullptr) {
            auto argument_values = std::get<std::deque<bool>>(argument_pointer.get()->Values);
            if (index < argument_values.size()) {
                return argument_values[index];
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    std::optional<int> ArgParser::GetIntValue(char str, std::size_t index) {
        std::shared_ptr<Argument<int>> argument_pointer = IntsByShortKey[str];
        if (argument_pointer != nullptr) {
            auto argument_values = std::get<std::vector<int>>(argument_pointer.get()->Values);
            if (index < argument_values.size()) {
                return argument_values[0];
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string> ArgParser::GetStringValue(char str, std::size_t index) {
        std::shared_ptr<Argument<std::string>> argument_pointer = StringsByShortKey[str];
        if (argument_pointer != nullptr) {
            auto argument_values = std::get<std::vector<std::string>>(argument_pointer.get()->Values);
            if (index < argument_values.size()) {
                return argument_values[index];
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    std::optional<bool> ArgParser::GetFlag(char str, std::size_t index) {
        std::shared_ptr<Argument<bool>> argument_pointer = BoolsByShortKey[str];
        if (argument_pointer != nullptr) {
            auto argument_values = std::get<std::deque<bool>>(argument_pointer.get()->Values);
            if (index < argument_values.size()) {
                return argument_values[index];
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    ArgParser& ArgParser::StoreValue(std::string &tmp) {
        if (std::holds_alternative<std::shared_ptr<Argument<std::string>>>(LastElement)) {
            std::get<std::shared_ptr<Argument<std::string>>>(LastElement)->Storage = &tmp;
        } else {
            throw std::invalid_argument("invalid function call placement: There were no argument before function call");
        }
        return *this;
    }

    ArgParser& ArgParser::StoreValue(int &tmp) {
        if (std::holds_alternative<std::shared_ptr<Argument<std::string>>>(LastElement)) {
            std::get<std::shared_ptr<Argument<int>>>(LastElement)->Storage = &tmp;
        } else {
            throw std::invalid_argument("invalid function call placement: There were no argument before function call");
        }
        return *this;
    }

    ArgParser& ArgParser::StoreValue(bool &tmp) {
        if (std::holds_alternative<std::shared_ptr<Argument<std::string>>>(LastElement)) {
            std::get<std::shared_ptr<Argument<bool>>>(LastElement)->Storage = &tmp;
        } else {
            throw std::invalid_argument("invalid function call placement: There were no argument before function call");
        }
        return *this;
    }

    ArgParser& ArgParser::StoreValues(std::vector<std::string> &tmp) {
        if (std::holds_alternative<std::shared_ptr<Argument<std::string>>>(LastElement)) {
            std::get<std::shared_ptr<Argument<std::string>>>(LastElement)->Storehouse = std::make_shared<std::vector<std::string>>(tmp);
        } else {
            throw std::invalid_argument("invalid function call placement: There were no argument before function call");
        }
        return *this;
    }

    ArgParser& ArgParser::StoreValues(std::vector<int> &tmp) {
        if (std::holds_alternative<std::shared_ptr<Argument<int>>>(LastElement)) {
            std::get<std::shared_ptr<Argument<int>>>(LastElement)->Storehouse = std::make_shared<std::vector<int>>(tmp);
        } else {
            throw std::invalid_argument("invalid function call placement: There were no argument before function call");
        }
        return *this;
    }

    ArgParser& ArgParser::StoreValues(std::deque<bool> &tmp) {
        if (std::holds_alternative<std::shared_ptr<Argument<bool>>>(LastElement)) {
            std::get<std::shared_ptr<Argument<bool>>>(LastElement)->Storehouse = std::make_shared<std::deque<bool>>(tmp);
        } else {
            throw std::invalid_argument("invalid function call placement: There were no argument before function call");
        }
        return *this;
    }

    ArgParser& ArgParser::Positional(){
        return *this;
    }


    void ArgParser::AddHelp(char s, const std::string &str, const std::string &description) {
        return;
    }


    std::string ArgParser::HelpDescription() {
        return "aboba";
    }


    bool ArgParser::Help() const {
        return true;
    }

};