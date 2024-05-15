#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "MapEntry.h"

class Map {
  private:
    std::string_view identifier;
    std::vector<MapEntry> mapEntries;
    std::vector<std::string_view> primitiveValues;
  public:
    Map();

    Map(std::string_view ident, std::vector<MapEntry> map);

    Map(std::string_view ident, std::vector<std::string_view> primValues);

    ~Map();

    enum class MapType {
      LIST_OF_MAPS,
      LIST_OF_PRIMITIVE_VALUES,
      MAP_OF_VALUES,
      PRIMITIVE_VALUES,
    };

    enum MapType mapType;

    MapType getMapType();

    void setMapType(MapType mapType);

    void setIdentifier(std::string_view identifier);

    void addMapEntry(MapEntry mapEntry);

    void addPrimitiveValue(std::string_view value);

    void clearPrimitiveValues();

    std::string_view &getIdentifier();

    std::vector<MapEntry> &getMapEntries();

    std::vector<std::string_view> &getPrimitiveValues();

    std::string toString();

    std::string getMapTypeString(MapType mapType);

    void setNewPrimitiveValues(std::vector<std::string_view> &primitiveValues, std::string_view newValues);
    
};