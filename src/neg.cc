#include <typeinfo>
#include <list>
#include <map>
	
#include "functions.hh"

int Neg::getFunctionType() const { return NEG; }
bool Neg::isFunctionType(int x) const { return x == NEG; }

Function* Neg::simplify() const
{
	if(getChild()->isFunctionType(CONSTANT))
	    return new Constant( - dynamic_cast<const Constant*>(getChild())->getNumber() );
	if(getChild()->isFunctionType(NEG))
    	return dynamic_cast<const Neg*>(getChild())->getChild()->clone();
	    
    Neg* simplified = dynamic_cast<Neg*>(clone());

    Function* simplifiedChild  = simplified->getChild()->simplify();

	bool modified = false;

	if(simplifiedChild != simplified->getChild())
    {
        simplified->addChild(simplifiedChild);
	    modified = true;
	}
    
    if(!modified)
        return const_cast<Neg*>(this);
	
    return simplified;
}

Function* Neg::clone() const
{
	return new Neg(getChild()->clone());
}

std::string Neg::print() const
{
	return "(-" + getChild()->print() + ")";
}

Function* Neg::derive(const std::string var) const
{
    return new Neg(getChild()->derive(var));
}

Function* Neg::evaluate(std::map<Variable, Function*> parameters) const
{
    return getChild()->evaluate(parameters);
}
