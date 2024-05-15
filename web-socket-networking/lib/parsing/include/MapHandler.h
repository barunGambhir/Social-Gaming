#pragma once
#include <vector>
#include <string_view>
#include <string>
#include <cpp-tree-sitter.h>
#include <iostream>
#include <variant>

#include "Map.h"

class MapHandler {
private:
    std::vector<Map> maps;
    std::vector<std::string_view> mapEntryIdentifier;
    std::vector<std::string_view> mapEntryValue;
   
    bool hasIdentifier = false;
    bool hasValueMap = false;
    bool hasListLiteral = false;
    bool hasMapEntry = false;

    Map map;

    void addMap(Map map);

    void extractIntoMaps(const ts::Node &node, const std::string_view &sourceText, int depth);

    void setIdentifier(std::string_view fieldType, std::string_view fieldValue);

    void resetIdentifier();

    void addListOfMaps(std::string_view fieldType);

    void setListOfMaps(std::string_view fieldType, std::string_view fieldValue);

    void addListOfPrimitiveValues();

    void setListOfPrimitiveValues(std::string_view fieldType, std::string_view fieldValue);

    void addMapOfValues();

    void setMapOfValues(std::string_view fieldType, std::string_view fieldValue);

    void addPrimitiveValues(std::string_view fieldValue);

    void setPrimitiveValues(std::string_view fieldValue);

    void truncate(std::string_view &fieldValue);

    void setNewPrimitiveListVal(std::string_view key, std::string_view value, int i);

    void setNewMapAndListOfMapVal(std::string_view key, std::string_view value, int i);

    bool doesNotContainObjectAndList(std::string_view fieldValue);

    bool isListOfMaps(const std::string_view &fieldType);

    bool isListOfPrimitiveValues(const std::string_view &fieldType);

    bool isMapOfValues(const std::string_view &fieldType);

    bool isListLiteral(const std::string_view &fieldType);

    bool isValueMap(const std::string_view &fieldType, int depth);

    bool isPrimitiveValue(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap);

    bool isListOfPrimitiveValues(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap);

    bool isMapOfValues(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap);

    bool isListOfMaps(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap);

public:
    MapHandler();

    MapHandler(const ts::Node &node, const std::string_view &sourceText);

    ~MapHandler();

    std::vector<Map> getMaps();

    std::string toString();

    std::variant<std::string_view, std::vector<std::string_view>> get(std::string_view category, std::string_view key = "");

    void set(std::string_view category, std::string value, std::string_view key = "");

    void add(std::string_view category, std::string_view value);

    void removeOne(std::string_view category, std::string_view key);

    void removeAll(std::string_view category);
};