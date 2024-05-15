/*
An aliased variant meant to store data used by a Social Gaming program.
*/

#pragma once
#include <variant>
#include <vector>
#include <memory>
#include <string>

using numberRange = std::pair<int, int>;
using SGDataWrapperUptr = std::unique_ptr<struct SGDataWrapper>;
using SGVector = std::vector<SGDataWrapperUptr>;
using SGMap = std::vector<std::pair<std::string, SGDataWrapperUptr>>;

using SGData = std::variant<
    int,
    bool,
    numberRange,
    std::string,
    SGVector,
    SGMap
>;

// This is done to avoid having to reference SGData in its declaration
struct SGDataWrapper {
    SGDataWrapper(SGData val) : val(std::move(val)) {}
    SGData val;
    ~SGDataWrapper() = default;
};