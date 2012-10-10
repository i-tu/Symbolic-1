#include <typeinfo>
#include <map>
#include <sstream>
 
#include "functions.hh"
#include "treeFunctions.hh"

int Constant::getFunctionType() const { return CONSTANT; }
bool Constant::isFunctionType(int x) const { return x == CONSTANT; }

// Constants don't simplify
Function* Constant::simplify() const
{
    return const_cast <Constant*> (this);
}

bool Constant::operator==(const Function& other) const
{
 return false;
}

std::string Constant::print() const
{
    std::ostringstream oss;
	oss << getNumber();
	return oss.str();
}

Function* Constant::derive(const std::string var) const
{
    return new Constant(0);
}

Function* Constant::evaluate(std::map<Variable, Function*> parameters) const
{
    return clone();
} 
