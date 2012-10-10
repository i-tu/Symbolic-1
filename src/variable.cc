#include <typeinfo>
#include <list>

#include "functions.hh"

int Variable::getFunctionType() const { return VARIABLE; }
bool Variable::isFunctionType(int x) const { return x == VARIABLE; }

// Variables don't simplify
Function* Variable::simplify() const
{
    return const_cast<Variable*>(this);
}

std::string Variable::getSymbol() const
{
	return _symbol;
}

std::string Variable::getNamespace() const
{
	return _namespace;
}

bool Variable::operator==(const Function& other) const
{
	try {
		const Variable& var = dynamic_cast<const Variable&>(other);
		return (_symbol == var.getSymbol()) && (_namespace == var.getNamespace());
	} catch (const std::bad_cast& bce) {
		return false;
	}
}

bool Variable::operator<(const Variable& other) const
{
	if (_symbol == other._symbol)
		return _namespace < other._namespace;
	else
		return _symbol < other._symbol;
}

Function* Variable::evaluate(std::map<Variable, Function*> parameters) const
{
    for(std::map<Variable, Function*>::iterator it = parameters.begin();it!=parameters.end();it++)
    {
        if(it->first.getSymbol() == getSymbol())
            return it->second->clone();
    }

	return clone();
}

Function* Variable::clone() const
{
	return new Variable(*this);
}

std::string Variable::print() const
{
	return getSymbol();
}

Function* Variable::derive(const std::string var) const
{
    if(getSymbol() == var)
        return new Constant(1);
    else
        return new Constant(0);
}
 
