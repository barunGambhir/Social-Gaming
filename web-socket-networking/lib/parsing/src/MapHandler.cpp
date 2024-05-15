#include "MapHandler.h"

MapHandler::MapHandler() {}

MapHandler::MapHandler(const ts::Node &node, const std::string_view &sourceText) {
        extractIntoMaps(node, sourceText, 0);
}

MapHandler::~MapHandler() {}

void MapHandler::extractIntoMaps(const ts::Node &node, const std::string_view &sourceText, int depth = 0) {
    for (uint32_t i = 0; i < node.getNumChildren(); i++) {
        ts::Node childNode = node.getChild(i);
        std::string_view fieldType = childNode.getType();
        auto byteRange = childNode.getByteRange();
        std::string_view fieldValue(sourceText.substr(byteRange.start, byteRange.end - byteRange.start));

        setIdentifier(fieldType, fieldValue);

        if (isListOfMaps(fieldType)) {
            addListOfMaps(fieldType);
        } else if (isListOfPrimitiveValues(fieldType)) {
            addListOfPrimitiveValues();
        } else if (isMapOfValues(fieldType)) {
            addMapOfValues();
        } else if (isListLiteral(fieldType)) {
            hasListLiteral = true;
        } else if (isValueMap(fieldType, depth)) {
            hasValueMap = true;
        } else if (isPrimitiveValue(fieldType, fieldValue, hasListLiteral, hasValueMap)) {
            setPrimitiveValues(fieldValue);
        } else if (isListOfPrimitiveValues(fieldType, fieldValue, hasListLiteral, hasValueMap)) {
            setListOfPrimitiveValues(fieldType, fieldValue);
        } else if (isMapOfValues(fieldType, fieldValue, hasListLiteral, hasValueMap)) {
            setMapOfValues(fieldType, fieldValue);
        } else if (isListOfMaps(fieldType, fieldValue, hasListLiteral, hasValueMap)) {
            setListOfMaps(fieldType, fieldValue);
        }

        extractIntoMaps(childNode, sourceText, depth + 1);
    }
}

bool MapHandler::doesNotContainObjectAndList(std::string_view fieldValue) {
    return fieldValue.find("[") == std::string_view::npos && fieldValue.find("{") == std::string_view::npos;
}

bool MapHandler::isListOfMaps(const std::string_view &fieldType) {
    return (fieldType == "}" || fieldType == "]") && hasValueMap && hasListLiteral;
}

bool MapHandler::isListOfPrimitiveValues(const std::string_view &fieldType) {
    return fieldType == "]" && hasListLiteral && !hasValueMap;
}

bool MapHandler::isMapOfValues(const std::string_view &fieldType) {
    return fieldType == "}" && hasValueMap && !hasListLiteral;
}

bool MapHandler::isListLiteral(const std::string_view &fieldType) {
    return fieldType == "list_literal";
}

bool MapHandler::isValueMap(const std::string_view &fieldType, int depth) {
    return fieldType == "value_map" && depth != 0;
}

bool MapHandler::isPrimitiveValue(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap) {
    return fieldType == "expression" && !hasListLiteral && !hasValueMap && doesNotContainObjectAndList(fieldValue);
}

bool MapHandler::isListOfPrimitiveValues(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap) {
    return fieldType == "expression" && hasListLiteral && !hasValueMap && doesNotContainObjectAndList(fieldValue);
}

bool MapHandler::isMapOfValues(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap) {
    return (fieldType == "identifier" || fieldType == "expression") && !hasListLiteral && hasValueMap && doesNotContainObjectAndList(fieldValue);
}

bool MapHandler::isListOfMaps(const std::string_view &fieldType, const std::string_view &fieldValue, bool hasListLiteral, bool hasValueMap) {
    return (fieldType == "identifier" || fieldType == "expression") && hasListLiteral && hasValueMap && doesNotContainObjectAndList(fieldValue);
}


void MapHandler::setIdentifier(std::string_view fieldType, std::string_view fieldValue) {
    if (fieldType == "identifier" && hasIdentifier == false && hasValueMap == false && hasMapEntry == false) { 
        hasIdentifier = true;
        map.setIdentifier(fieldValue);
    }
 }
 
void MapHandler::resetIdentifier() {
    hasIdentifier = false;
}

void MapHandler::addListOfMaps(std::string_view fieldType) {
    if (fieldType == "}") {
        MapEntry mapEntry(mapEntryIdentifier, mapEntryValue);
        map.addMapEntry(mapEntry);
        mapEntryIdentifier.clear();
        mapEntryValue.clear();
    } else if (fieldType == "]") {
        hasListLiteral = false;
        hasValueMap = false;
        map.setMapType(Map::MapType::LIST_OF_MAPS);
        addMap(map);
        map = Map();
        resetIdentifier();
    }
}

void MapHandler::setListOfMaps(std::string_view fieldType, std::string_view fieldValue) {
    if (fieldType == "identifier") {
        mapEntryIdentifier.push_back(fieldValue);
    } else {
        mapEntryValue.push_back(fieldValue);
    }
}

void MapHandler::addListOfPrimitiveValues() {
    hasListLiteral = false;
    map.setMapType(Map::MapType::LIST_OF_PRIMITIVE_VALUES);
    addMap(map);
    map.clearPrimitiveValues();
    map = Map();
    resetIdentifier();
 }

void MapHandler::setListOfPrimitiveValues(std::string_view fieldType, std::string_view fieldValue) {
    map.addPrimitiveValue(fieldValue);
}

void MapHandler::addMapOfValues() {
    hasValueMap = false;
    if(mapEntryIdentifier.size() != 0 && mapEntryValue.size() != 0) {
        MapEntry mapEntry(mapEntryIdentifier, mapEntryValue);
        map.addMapEntry(mapEntry);
        map.setMapType(Map::MapType::MAP_OF_VALUES);
        addMap(map);
        map.clearPrimitiveValues();
        mapEntryIdentifier.clear();
        mapEntryValue.clear();
        map = Map();
    }
    resetIdentifier();
}

void MapHandler::setMapOfValues(std::string_view fieldType, std::string_view fieldValue) {
    if (fieldType == "identifier") {
        mapEntryIdentifier.push_back(fieldValue);
    } else {
        mapEntryValue.push_back(fieldValue);
    }
}

void MapHandler::addPrimitiveValues(std::string_view fieldValue) {
    map.addPrimitiveValue(fieldValue);
    map = Map(map.getIdentifier(), map.getPrimitiveValues());
    map.setMapType(Map::MapType::PRIMITIVE_VALUES);
    addMap(map);
    map.clearPrimitiveValues();
    map = Map();
    resetIdentifier();
}

void MapHandler::setPrimitiveValues(std::string_view fieldValue) {
    addPrimitiveValues(fieldValue);
}

void MapHandler::addMap(Map constant) {
    maps.push_back(constant);
}

std::vector<Map> MapHandler::getMaps() {
    return maps;
}

std::string MapHandler::toString() {
    std::string result;
    result += "{\n";
    for (long unsigned int i = 0; i < maps.size(); i++) {
        result += maps.at(i).toString();
        if (i != maps.size() - 1) {
            result += "\n";
        }
    }
    result += "\n}\n";
    return result;
}

std::variant<std::string_view, std::vector<std::string_view>> MapHandler::get(std::string_view category, std::string_view key) {
    for (long unsigned int i = 0; i < maps.size(); i++) {
        if (maps.at(i).getIdentifier() == category) {
            if (key == "") {
                if (maps.at(i).getPrimitiveValues().size() == 1) {
                    return maps.at(i).getPrimitiveValues().at(0);
                } 
                return maps.at(i).getPrimitiveValues();
            }
           for (long unsigned int j = 0; j < maps.at(i).getMapEntries().size(); j++) {
                std::string_view currentKey = maps.at(i).getMapEntries().at(j).getMapEntryValue()[0];
                truncate(currentKey);
                 if (currentKey == key) {
                    return maps.at(i).getMapEntries().at(j).getMapEntryValue()[1];
                }
           }
        }
    }
    return "Not found";
}

void MapHandler::set(std::string_view category, std::string value, std::string_view key) {
    for (long unsigned int i = 0; i < maps.size(); i++) {
        if (maps.at(i).getIdentifier() == category) {
            if (maps.at(i).mapType == Map::MapType::PRIMITIVE_VALUES) {
                this->maps.at(i).getPrimitiveValues().at(0) = value;        
                return;
            } else if (maps.at(i).mapType == Map::MapType::LIST_OF_PRIMITIVE_VALUES) {
                setNewPrimitiveListVal(key, value, i);
            } else if (maps.at(i).mapType == Map::MapType::MAP_OF_VALUES || maps.at(i).mapType == Map::MapType::LIST_OF_MAPS) {
                setNewMapAndListOfMapVal(key, value, i);
            } 
        }
    }
}

void MapHandler::add(std::string_view category, std::string_view value) {
    for (long unsigned int i = 0; i < maps.size(); i++) {
        if (maps.at(i).getIdentifier() == category) {
            if (maps.at(i).mapType == Map::MapType::LIST_OF_PRIMITIVE_VALUES) {
                this->maps.at(i).addPrimitiveValue(value);
                return;
            } 
        }
    }
}

 void MapHandler::removeOne(std::string_view category, std::string_view key) {
    std::string str(key);
    int index = std::stoi(str); 
    for (long unsigned int i = 0; i < maps.size(); i++) {
        if (maps.at(i).getIdentifier() == category) {
            if (maps.at(i).mapType == Map::MapType::LIST_OF_PRIMITIVE_VALUES) {
                // remove the element at index index
                maps.at(i).getPrimitiveValues().erase(maps.at(i).getPrimitiveValues().begin() + index);
            } 
        }
    }
 }

void MapHandler::removeAll(std::string_view category) {
    for (long unsigned int i = 0; i < maps.size(); i++) {
        if (maps.at(i).getIdentifier() == category) {
            if (maps.at(i).mapType == Map::MapType::LIST_OF_PRIMITIVE_VALUES) {
                maps.at(i).getPrimitiveValues().clear();
                return;
            } 
        }
    }
}

void MapHandler::truncate(std::string_view &fieldValue) {
    size_t start = fieldValue.find_first_of('"');
    size_t end = fieldValue.find_last_of('"');
    fieldValue = fieldValue.substr(start + 1, end - start - 1);
}

void MapHandler::setNewPrimitiveListVal(std::string_view key, std::string_view value, int i) {
    std::string str(key);
    long unsigned int index = std::stoi(str); // convert string to int
    if (index >= maps.at(i).getPrimitiveValues().size()) {
        std::cout << "Index out of bounds" << std::endl;
        return;
    }
    for (long unsigned int j = 0; j < maps.at(i).getPrimitiveValues().size(); j++) {
        if (j == index) {
            this->maps.at(i).getPrimitiveValues().at(j) = value;
            return;
        }
    }   
}

 void MapHandler::setNewMapAndListOfMapVal(std::string_view key, std::string_view value, int i) {
    for (long unsigned int j = 0; j < maps.at(i).getMapEntries().size(); j++) {
        std::string_view currentKey = maps.at(i).getMapEntries().at(j).getMapEntryValue()[0];
        truncate(currentKey);
        if (currentKey == key) {
            this->maps.at(i).getMapEntries().at(j).getMapEntryValue()[1] = value;
            return;
        }
    }
 }