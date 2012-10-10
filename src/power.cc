#include <typeinfo>
#include <map>
#include <iostream>
#include "functions.hh"
#include <cmath>

int Power::getFunctionType() const { return POWER; }
bool Power::isFunctionType(int x) const { return x == POWER; }

// Returns simplified function or NULL if simplification was not found...
Function* Power::simplify() const
{
    if(getBase()->isFunctionType(CONSTANT) && getExponent()->isFunctionType(CONSTANT))
    {
        const Constant* base = dynamic_cast<const Constant*>(getBase());
        const Constant* power = dynamic_cast<const Constant*>(getExponent());
        
		return new Constant(Rational::rationalPow(base->getNumber(), power->getNumber()));
    }
    else if(getBase()->isFunctionType(CONSTANT))
    {
        const Constant* base = dynamic_cast<const Constant*>(getBase());
        if(base->getNumber() == 0)
            return new Constant(0);
        if(base->getNumber() == 1)
            return new Constant(1);
    }
    // (x^m)^n = x^(m*n)
    else if(getBase()->isFunctionType(POWER))
    {
        const Power* base = dynamic_cast<const Power*>(getBase());
        
        return new Power(base->getBase()->clone(),
                         new Multiplication(getExponent()->clone(), base->getExponent()->clone() ) );
    }
    else if(getExponent()->isFunctionType(CONSTANT))
    {
        const Constant* power = dynamic_cast<const Constant*>(getExponent());
        
        if(power->getNumber() == 0)
            return new Constant(1);
                
        else if(power->getNumber() == 1)
            return getBase()->clone();
    }

    
    bool modified = false;
    
    // A solution was not found. We must go deeper...
    Power* simplified = dynamic_cast<Power*>(clone());

    // Recursive traversing of tree
    Function* simplifiedLeft = simplified->getBase()->simplify();
    Function* simplifiedRight = simplified->getExponent()->simplify();
    
    if(simplifiedLeft != simplified->getBase())
    {
        modified = true;
        simplified->setLhs(simplifiedLeft);
    }
    
    if(simplifiedRight != simplified->getRhs())
    {
        modified = true;
        simplified->setRhs(simplifiedRight);
    }
    
    if(!modified)
    {
        delete simplified;
        return const_cast<Power*>(this);
    }
    return simplified;
}

const Function* Power::getBase() const
{
	return getLhs();
}

Function* Power::getBase()
{
	return getLhs();
}

const Function* Power::getExponent() const
{
	return getRhs();
}

Function* Power::getExponent()
{
	return getRhs();
}

Function* Power::evaluate(std::map<Variable, Function*> parameters) const
{
	return new Power(getBase()->evaluate(parameters), getExponent()->evaluate(parameters));
}	

Function* Power::clone() const
{
	return new Power(getBase()->clone(), getExponent()->clone());
}

std::string Power::print() const
{
	return "(" + getBase()->print() + "^" + getExponent()->print() + ")";
}

// x^n = nx^(n-1)
Function* Power::derive(const std::string var) const
{
    if(getBase()->isFunctionType(VARIABLE))
    {
    	Variable* base = dynamic_cast<Variable*>(getBase()->clone());
    	if(base->getSymbol() == var){
			delete base;
	        return new Multiplication(getExponent()->clone(), new Power(getBase()->clone(), new Sum(getExponent()->clone(), new Constant(-1))) );
		}
    		delete base;
    }
    
    return new Power(getBase()->clone(), getExponent()->clone());
} 


bool Power::equals(const Power* p1, const Power* p2)
	{
		const Variable *var1, *var2;
		const Constant *c1, *c2;

		if (p1->getBase()->isFunctionType(VARIABLE))
			var1 = dynamic_cast<const Variable*>(p1->getBase());
		else
			return false;

		if (p2->getBase()->isFunctionType(VARIABLE))
			var2 = dynamic_cast<const Variable*>(p2->getBase());
		else
			return false;

		if (p1->getExponent()->isFunctionType(CONSTANT))
			c1 = dynamic_cast<const Constant*>(p1->getExponent());
		else
			return false;

		if (p2->getExponent()->isFunctionType(CONSTANT))
			c2 = dynamic_cast<const Constant*>(p2->getExponent());
		else
			return false;

		return (*var1 == *var2 && c1->getNumber() == c2->getNumber());

	}
