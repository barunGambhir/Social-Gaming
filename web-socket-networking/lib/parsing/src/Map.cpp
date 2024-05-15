#include "Map.h"

Map::Map() {}

Map::Map(std::string_view ident, std::vector<MapEntry> map)
    : identifier(ident), mapEntries(map) {}

Map::Map(std::string_view ident, std::vector<std::string_view> primValue)
    : identifier(ident), primitiveValues(primValue) {}   

Map::~Map() {}

Map::MapType Map::getMapType() {
      return mapType;
}

void Map::setMapType(MapType mapType) {
      this->mapType = mapType;
}

void Map::setIdentifier(std::string_view identifier) {
      this->identifier = identifier;
}

void Map::addMapEntry(MapEntry mapEntry) {
      mapEntries.push_back(mapEntry);
}

void Map::addPrimitiveValue(std::string_view value) {
      primitiveValues.push_back(value);
}

void Map::clearPrimitiveValues() {
      primitiveValues.clear();
}

std::string_view& Map::getIdentifier() {
      return identifier;
}

std::vector<MapEntry>& Map::getMapEntries() {
      return mapEntries;
}

std::vector<std::string_view>& Map::getPrimitiveValues() {
      return primitiveValues;
}

std::string Map::toString() {
      std::string result;
      result += identifier;
      result += ": ";

      if (primitiveValues.size() > 0) {
        for (int i = 0; i < primitiveValues.size(); i++) {
          if(primitiveValues.size() == 1) {
          result += primitiveValues.at(i);
          result += ",";
          return result;
          } else if (i == 0) {
            result += "[";
          }
          result += primitiveValues.at(i);
          if (i == primitiveValues.size() - 1) {
            result += "],";
          } else {
            result += ", ";
          }
        }
      } else {
          if (mapEntries.size() == 1) {
            result += "{ ";
            for (int j = 0; j < mapEntries.at(0).getMapEntryIdentifier().size(); j++) {  
              result += mapEntries.at(0).getMapEntryIdentifier().at(j);
              result += ": ";
              result += mapEntries.at(0).getMapEntryValue().at(j);

              if (j == mapEntries.at(0).getMapEntryIdentifier().size() - 1) {
                result += " ";
              } else {
                result += ", ";
              }
            }
             result += " }";
             return result;
          }
          result += "[\n";
          for (int i = 0; i < mapEntries.size(); i++) {
            result += "{ ";
            for (int j = 0; j < mapEntries.at(i).getMapEntryIdentifier().size(); j++) {
              result += mapEntries.at(i).getMapEntryIdentifier().at(j);
              result += ": ";
              result += mapEntries.at(i).getMapEntryValue().at(j);

              if (j == mapEntries.at(i).getMapEntryIdentifier().size() - 1) {
                result += " ";
              } else {
                result += ", ";
              }
            }
            if (i == mapEntries.size() - 1) {
              result += " }";
              result += "\n";
            } else {
              result += " },";
              result += "\n";
            }
          }
        result += "],";
      }
    return result;  
}   


std::string Map::getMapTypeString(MapType mapType) {
  switch (mapType) {
    case MapType::PRIMITIVE_VALUES:
      return "PRIMITIVE_VALUES";
    case MapType::MAP_OF_VALUES:
      return "MAP_OF_VALUES";
    case MapType::LIST_OF_MAPS:
      return "LIST_OF_MAPS";
    case MapType::LIST_OF_PRIMITIVE_VALUES:
      return "LIST_OF_PRIMITIVE_VALUES";
    default:
      return "UNKNOWN";
  }
}

void Map::setNewPrimitiveValues(std::vector<std::string_view> &primitiveValues, std::string_view newValues) {
  primitiveValues.at(0) = newValues;
}
