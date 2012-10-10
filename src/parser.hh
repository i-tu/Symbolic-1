#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <deque>

class Function;

void infixToPrefix(std::deque<std::string>&);
Function* parsePrefix(std::deque<std::string>&);
int precedence(const std::string);
std::deque<std::string> split(const std::string&);
bool isAlphabetic(const std::string&);
bool isNumber(const std::string&);
bool isOperator(const std::string&);
bool isOpNum(const std::string&);
bool isValid(const std::string&);
bool isClean(const std::string&);
std::deque<std::string> split(const std::string&);

#endif 
