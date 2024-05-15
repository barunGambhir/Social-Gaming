#include "Helper.h"

std::string_view Helper::extractFieldValue(const ts::Node& node, const std::string_view& sourcecode) {
    auto byteRange = node.getByteRange();
    std::string_view fieldValue(sourcecode.substr(byteRange.start, byteRange.end - byteRange.start));
    return fieldValue;
}

std::string Helper::covertViewToString(const std::string_view& strView) {
    std::string str(strView.begin(), strView.end());
    return str;
}

std::string Helper::trimLeadingTrailingWhitespace(const std::string& str) {
    std::string removedLeading = trimLeadingWhitespace(str); 
    std::string removedTrailing = trimTrailingWhitespace(removedLeading); 
    return removedTrailing; 
}

std::string Helper::trimLeadingWhitespace(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string Helper::trimTrailingWhitespace(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
} 

std::string Helper::removeWrappingQuotes(const std::string& str) {
    if (str.size() >= 2 && 
    ((str.front() == '"' && str.back() == '"') || (str.front() == '\'' && str.back() == '\''))) {
        return str.substr(1, str.size() - 2); 
    } else {
        return str;
    }
}

std::string Helper::removeWrappingParentheses(const std::string& str) {
    if (str.size() >= 2 && str.front() == '(' && str.back() == ')') {
        return str.substr(1, str.size() - 2);
    } else {
        return str;
    }
}