#include <string>
#include <iostream>

#include <nlohmann/json.hpp>

int main() {
    nlohmann::json json = {
        {"happy", true},
        {"pi", 3.141},
    };

    std::cout << "JSON object" << json << std::endl;

    nlohmann::json ex1 = nlohmann::json::parse(R"(
        {
            "key1": "value1",
            "key2": false
        }
    )");

    std::cout << "JSON object parsed" << json << std::endl;
    std::cout << "JSON object parsed" << ex1 << std::endl;
    std::cout << "JSON object accesing " << json["happy"] << std::endl;
    std::cout << "JSON object accesing " << json["pi"] << std::endl;
    std::cout << "JSON object accesing " << ex1["key1"] << std::endl;
    std::cout << "JSON object accesing " << ex1["key2"] << std::endl;

    if (json["happy"]){
        std::cout << "TRUE - bool" << std::endl;
    }

    if (json["pi"] == 3.142){
        std::cout << "TRUE - number" << std::endl;
    }

    if (ex1["key1"] == "value2"){
        std::cout << "TRUE - string " << std::endl;
    }

    if (!ex1["key2"]){
        std::cout << "FALSE - bool " << std::endl;
    }

    return 0;
}
