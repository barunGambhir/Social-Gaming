#include "MapEntry.h"

MapEntry::MapEntry(std::vector<std::string_view> identifier, std::vector<std::string_view> value)
    : mapEntryIdentifier(identifier), mapEntryValue(value) {}

MapEntry::~MapEntry() {}   

void MapEntry::addMapEntryIdentifier(std::string_view identifier) {
      mapEntryIdentifier.push_back(identifier);
}

void MapEntry::addMapEntryValue(std::string_view value) {
      mapEntryValue.push_back(value);
}

std::vector<std::string_view>& MapEntry::getMapEntryIdentifier() {
      return mapEntryIdentifier;
}

std::vector<std::string_view>& MapEntry::getMapEntryValue() {
      return mapEntryValue;
}