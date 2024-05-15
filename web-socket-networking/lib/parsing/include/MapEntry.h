#pragma once

#include <vector>
#include <string_view>

class MapEntry {
  private:
  std::vector<std::string_view> mapEntryIdentifier;
  std::vector<std::string_view> mapEntryValue;
  public:
    MapEntry(std::vector<std::string_view> identifier, std::vector<std::string_view> value);

    ~MapEntry();

    void addMapEntryIdentifier(std::string_view identifier);

    void addMapEntryValue(std::string_view value);

    std::vector<std::string_view>& getMapEntryIdentifier();

    std::vector<std::string_view>& getMapEntryValue();
};