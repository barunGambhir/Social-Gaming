#pragma once

#include <cpp-tree-sitter.h>
#include <string>
#include <string_view>
#include <iostream>

class Helper {
public:
    static std::string_view extractFieldValue(const ts::Node& node, const std::string_view& sourcecode);

    static std::string covertViewToString(const std::string_view& strView);

    static std::string trimLeadingTrailingWhitespace(const std::string& str); 
    
    static std::string removeWrappingQuotes(const std::string& str);

    static std::string removeWrappingParentheses(const std::string& str);

private:
    static std::string trimLeadingWhitespace(const std::string& str); 

    static std::string trimTrailingWhitespace(const std::string& str); 
};

class ExceptionNodeWithSpecifiedTypeNotFound: public std::exception {
    virtual const char* what() const throw() {
        return "Node with specified type not found.";
    }
};