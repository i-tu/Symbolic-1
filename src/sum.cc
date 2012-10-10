#include <typeinfo>
#include <list>
#include <map>

#include "functions.hh"

int Sum::getFunctionType() const { return SUM; }
bool Sum::isFunctionType(int x) const { return x == SUM; }

Function* Sum::simplify() const
{
    // 1+1=2
    if(getLhs()->isFunctionType(CONSTANT) && getRhs()->isFunctionType(CONSTANT))
    {
        const Constant* left = dynamic_cast<const Constant*>(getLhs());
        const Constant* right = dynamic_cast<const Constant*>(getRhs());
        
        return new Constant(left->getNumber() + right->getNumber());
    }
    // x+0=x
    else if(getLhs()->isFunctionType(CONSTANT) || getRhs()->isFunctionType(CONSTANT))
    {
        if(getLhs()->isFunctionType(CONSTANT))
        {
            const Constant* left = dynamic_cast<const Constant*>(getLhs());
            if(left->getNumber()==0)
                return getRhs()->clone();
        }
        if(getRhs()->isFunctionType(CONSTANT))
        {
            const Constant* right = dynamic_cast<const Constant*>(getRhs());
            if(right->getNumber()==0)
                return getLhs()->clone();
        }
    }
    // a+a=2*a
    else if(getLhs()->getFunctionType() == VARIABLE && getRhs()->getFunctionType() == VARIABLE)
    {
        const Variable* right = dynamic_cast<const Variable*>(getRhs());
        const Variable* left = dynamic_cast<const Variable*>(getLhs());
        
        if(left->getSymbol() == right->getSymbol())
            return new Multiplication(new Variable(left->getSymbol(), "global"), new Constant(2));
    }
	// a^2 + a^2 = 2*a^2
	else if(getLhs()->getFunctionType() == POWER && getRhs()->getFunctionType() == POWER)
	{
		const Power* right = dynamic_cast<const Power*>(getRhs());
		const Power* left = dynamic_cast<const Power*>(getLhs());

		if(Power::equals(left,right))
			return new Multiplication(new Power(left->getBase()->clone(), left->getExponent()->clone()), new Constant(2));
	}
	// a/b + c/d = (a*d + c*b)/(b*d)
	else if(getLhs()->getFunctionType() == DIVISION && getRhs()->getFunctionType() == DIVISION)
    {
        const Division* right = dynamic_cast<const Division*>(getRhs());
        const Division* left = dynamic_cast<const Division*>(getLhs());

		Multiplication *m1 = new Multiplication(left->getLhs()->clone(),right->getRhs()->clone());
		Multiplication *m2 = new Multiplication(right->getLhs()->clone(),left->getRhs()->clone());
		Sum* n = new Sum(m1,m2);
		Multiplication *d = new Multiplication(left->getRhs()->clone(),right->getRhs()->clone());
        
        return new Division(n,d);
    }
	// a^2 + a^2 = 2*a^2
    
    // a+(2*a) = 3*a
    else if((getLhs()->getFunctionType() == MULTIPLICATION && getRhs()->getFunctionType() == VARIABLE)
         || (getRhs()->getFunctionType() == MULTIPLICATION && getLhs()->getFunctionType() == VARIABLE))
    {
        bool leftismult = getLhs()->isFunctionType(MULTIPLICATION);
        
        const Multiplication* mult = dynamic_cast<const Multiplication*>(leftismult ? getLhs() : getRhs() );
        if((mult->getLhs()->isFunctionType(VARIABLE) && mult->getRhs()->isFunctionType(CONSTANT)) 
        || (mult->getRhs()->isFunctionType(VARIABLE) && mult->getLhs()->isFunctionType(CONSTANT)))
        {
            const Variable* var = dynamic_cast<const Variable*>(mult->getLhs()->isFunctionType(VARIABLE) ? mult->getLhs() : mult->getRhs());
            const Variable* other_var = dynamic_cast<const Variable*>(leftismult ? getRhs() : getLhs());
            if(var->getSymbol() == other_var->getSymbol())
            {
                const Constant* con = dynamic_cast<const Constant*>(mult->getLhs()->isFunctionType(CONSTANT) ? mult->getLhs() : mult->getRhs() );
                return new Multiplication(new Variable(var->getSymbol(), "global"), new Constant(con->getNumber() + 1)); 
            }
        }
    }
	// a+^2+(2*a^2) = 3*a^2
    else if((getLhs()->getFunctionType() == MULTIPLICATION && getRhs()->getFunctionType() == POWER)
         || (getRhs()->getFunctionType() == MULTIPLICATION && getLhs()->getFunctionType() == POWER))
    {
        bool leftismult = getLhs()->isFunctionType(MULTIPLICATION);
        
        const Multiplication* mult = dynamic_cast<const Multiplication*>(leftismult ? getLhs() : getRhs() );
        if((mult->getLhs()->isFunctionType(POWER) && mult->getRhs()->isFunctionType(CONSTANT)) 
        || (mult->getRhs()->isFunctionType(POWER) && mult->getLhs()->isFunctionType(CONSTANT)))
        {
            const Power* pw = dynamic_cast<const Power*>(mult->getLhs()->isFunctionType(POWER) ? mult->getLhs() : mult->getRhs());
            const Power* other_pw = dynamic_cast<const Power*>(leftismult ? getRhs() : getLhs());
			if(Power::equals(pw,other_pw))
            {
                const Constant* con = dynamic_cast<const Constant*>(mult->getLhs()->isFunctionType(CONSTANT) ? mult->getLhs() : mult->getRhs() );
                return new Multiplication(new Power(pw->getBase()->clone(), pw->getExponent()->clone()), new Constant(con->getNumber() + 1)); 
            }
        }
    }
    // If there is exactly one sum as a child, it is always on the left side.
    else if((getLhs()->getFunctionType() != SUM && getRhs()->getFunctionType() == SUM ))
    {
        return new Sum(getRhs()->clone(), getLhs()->clone());
    }
    // If both children are sums, we travel to the left until we find something other than a sum, then swap these two.
    // eg.   +           +
    //     +   +   ->   + a
    //    a b c d      + b
    //                c d
    else if((getLhs()->getFunctionType() == SUM && getRhs()->getFunctionType() == SUM ))
    {
        Sum* simple = dynamic_cast<Sum*>(clone());
        
        Sum* traverser;
        for(traverser = dynamic_cast<Sum*>( simple->getLhs() );
            traverser->getLhs()->isFunctionType(SUM);
            traverser = dynamic_cast<Sum*>( traverser->getLhs() ) )
            ;
        
        Function* rightToLeft = getRhs()->clone();
        simple->setRhs(traverser->getLhs()->clone());
        traverser->setLhs(rightToLeft);
        
        return simple;
    }
    // (a+2) + (a+3) == ((a+a)+(2+3)))
    else if((getLhs()->getFunctionType() == SUM && getRhs()->getFunctionType() == SUM ))
    {
        const Sum* left  = dynamic_cast<const Sum*>(getLhs());
        const Sum* right = dynamic_cast<const Sum*>(getRhs());
        
        Function* simpler = new Sum(left->getLhs()->clone(), right->getLhs()->clone());
        Function* simple = simpler;
        simpler = simpler->simplify();
        
        if(simpler != simple)
            return simpler;
            
        simpler = new Sum(left->getRhs()->clone(), right->getLhs()->clone());
        simple = simpler;
        simpler = simpler->simplify();
        if(simpler != simple)
            return simpler;
        
        simpler = new Sum(left->getLhs()->clone(), right->getRhs()->clone());
        simple = simpler;
        simpler = simpler->simplify();
        if(simpler != simple)
            return simpler;
            
        simpler = new Sum(left->getRhs()->clone(), right->getRhs()->clone());
        simple = simpler;
        simpler = simpler->simplify();
        if(simpler != simple)
            return simpler;
    }
    // (a*2) + (a*3) == (a*(2+3))
    else if(getLhs()->getFunctionType() == MULTIPLICATION
         && getRhs()->getFunctionType() == MULTIPLICATION )
    {
        const Multiplication* left  = dynamic_cast<const Multiplication*>(getLhs());
        const Multiplication* right = dynamic_cast<const Multiplication*>(getRhs());
        
        if(left->getLhs()->getFunctionType() == VARIABLE || left->getRhs()->getFunctionType() == VARIABLE
        && right->getLhs()->getFunctionType() == VARIABLE || right->getRhs()->getFunctionType() == VARIABLE)
        {   
            if(left->getLhs()->isFunctionType(VARIABLE) && right->getLhs()->isFunctionType(VARIABLE))
            {
                const Variable* left_var  = dynamic_cast<const Variable*>(left->getLhs());
                const Variable* right_var = dynamic_cast<const Variable*>(right->getLhs());
                if(left_var->getSymbol() == right_var->getSymbol())
                {
                    return new Multiplication(
                        new Variable(left_var->getSymbol(),"global"),
                        new Sum(left->getRhs()->clone(), right->getRhs()->clone()));
                }
            }
            if(left->getRhs()->isFunctionType(VARIABLE) && right->getLhs()->isFunctionType(VARIABLE))
            {
                const Variable* left_var  = dynamic_cast<const Variable*>(left->getRhs());
                const Variable* right_var = dynamic_cast<const Variable*>(right->getLhs());
                if(left_var->getSymbol() == right_var->getSymbol())
                {
                    return new Multiplication(
                        new Variable(left_var->getSymbol(),"global"),
                        new Sum(left->getLhs()->clone(), right->getRhs()->clone()));
                }
            }
            if(left->getLhs()->isFunctionType(VARIABLE) && right->getRhs()->isFunctionType(VARIABLE))
            {
                const Variable* left_var  = dynamic_cast<const Variable*>(left->getLhs());
                const Variable* right_var = dynamic_cast<const Variable*>(right->getRhs());
                if(left_var->getSymbol() == right_var->getSymbol())
                {
                    return new Multiplication(
                        new Variable(left_var->getSymbol(),"global"),
                        new Sum(left->getRhs()->clone(), right->getLhs()->clone()));
                }
            }
            if(left->getRhs()->isFunctionType(VARIABLE) && right->getRhs()->isFunctionType(VARIABLE))
            {
                const Variable* left_var  = dynamic_cast<const Variable*>(left->getRhs());
                const Variable* right_var = dynamic_cast<const Variable*>(right->getRhs());
                if(left_var->getSymbol() == right_var->getSymbol())
                {
                    return new Multiplication(
                        new Variable(left_var->getSymbol(),"global"),
                        new Sum(left->getLhs()->clone(), right->getLhs()->clone()));
                }
            }
        }
		else if(left->getLhs()->getFunctionType() == POWER || left->getRhs()->getFunctionType() == POWER
        && right->getLhs()->getFunctionType() == POWER || right->getRhs()->getFunctionType() == POWER)
        {   
            if(left->getLhs()->isFunctionType(POWER) && right->getLhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getLhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getLhs());
				if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getRhs()->clone(), right->getRhs()->clone()));
                }
            }
            if(left->getRhs()->isFunctionType(POWER) && right->getLhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getRhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getLhs());
				if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getLhs()->clone(), right->getRhs()->clone()));
                }
            }
            if(left->getLhs()->isFunctionType(POWER) && right->getRhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getLhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getRhs());
                if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getRhs()->clone(), right->getLhs()->clone()));
                }
            }
            if(left->getRhs()->isFunctionType(POWER) && right->getRhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getRhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getRhs());
                if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getLhs()->clone(), right->getLhs()->clone()));
                }
            }
        }
    }
	// (a^2*2) + (a^2*3) == (a^2*(2+3))
    else if(getLhs()->getFunctionType() == MULTIPLICATION
         && getRhs()->getFunctionType() == MULTIPLICATION )
    {
        const Multiplication* left  = dynamic_cast<const Multiplication*>(getLhs());
        const Multiplication* right = dynamic_cast<const Multiplication*>(getRhs());
        
        if(left->getLhs()->getFunctionType() == POWER || left->getRhs()->getFunctionType() == POWER
        && right->getLhs()->getFunctionType() == POWER || right->getRhs()->getFunctionType() == POWER)
        {   
            if(left->getLhs()->isFunctionType(POWER) && right->getLhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getLhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getLhs());
				if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getRhs()->clone(), right->getRhs()->clone()));
                }
            }
            if(left->getRhs()->isFunctionType(POWER) && right->getLhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getRhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getLhs());
				if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getLhs()->clone(), right->getRhs()->clone()));
                }
            }
            if(left->getLhs()->isFunctionType(POWER) && right->getRhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getLhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getRhs());
                if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getRhs()->clone(), right->getLhs()->clone()));
                }
            }
            if(left->getRhs()->isFunctionType(POWER) && right->getRhs()->isFunctionType(POWER))
            {
                const Power* left_pow  = dynamic_cast<const Power*>(left->getRhs());
                const Power* right_pow = dynamic_cast<const Power*>(right->getRhs());
                if(Power::equals(left_pow,right_pow))
                {
                    return new Multiplication(
                        new Power(left_pow->getBase()->clone(),left_pow->getExponent()->clone()),
                        new Sum(left->getLhs()->clone(), right->getLhs()->clone()));
                }
            }
        }
    }
    // ((a+1)+a) = 2*a+1
    else if(getLhs()->isFunctionType(SUM))
    {
        bool modified = false;
        
        Sum* head = dynamic_cast<Sum*>(clone());

        Sum* link = head;
        Sum* linkfather = link;
    
        // While we have not reached the bottom,

        while(link->getLhs()->isFunctionType(SUM))
        {
            // save father.
            linkfather = link;

            // Descend in tree...
            link = dynamic_cast<Sum*>(link->getLhs());

            // If bottom is reached, we need to also check the right side.
            if( !link->getLhs()->isFunctionType(SUM) )
            {
                Sum* chained = new Sum(link->getLhs()->clone(), head->getRhs()->clone());
                Function* chained_simple = chained->simplify();

                // If it simplifies...
                if(chained != chained_simple)
                {
                    // attach it and change tree accordingly
                    head->setRhs(chained_simple);
                    linkfather->setLhs(link->getRhs()->clone());
                    // Start again.
                    link = head;
                    modified = true;
                }
                delete chained;
                continue;
            }
            
            Sum* chained = new Sum(link->getRhs()->clone(), head->getRhs()->clone());
            
            Function* chained_simple = chained->simplify();

            // If it simplifies...
            if(chained != chained_simple)
            {
                // attach it and change tree accordingly
                head->setRhs(chained_simple);
                linkfather->setLhs(link->getLhs()->clone());
                    
                // Start again.
                link = head;
                modified = true;

            }
            delete chained;
            continue;           
        }
        if(modified)
            return head;
        else
            delete head;
    }
    
    bool modified = false;
    
    Sum* simplified = dynamic_cast<Sum*>(clone());
    Function* simplifiedLeft  = simplified->getLhs()->simplify();
    Function* simplifiedRight = simplified->getRhs()->simplify();

    if(simplifiedLeft != simplified->getLhs())
    {
        simplified->setLhs(simplifiedLeft);
        modified = true;
    }
    
    if(simplifiedRight != simplified->getRhs())
    {
        simplified->setRhs(simplifiedRight);
        modified = true;
    }
    
    if(!modified)
    {
        delete simplified;
        return const_cast<Sum*>(this);
    }
    
    return simplified;
}

Function* Sum::clone() const
{
	return new Sum(getLhs()->clone(), getRhs()->clone());
}

std::string Sum::print() const
{
	return "(" + getLhs()->print() + " + " + getRhs()->print() + ")";
}

// (f+g)' = f'+g'
Function* Sum::derive(const std::string var) const
{
    return new Sum(getLhs()->derive(var), getRhs()->derive(var));
}

Function* Sum::evaluate(std::map<Variable, Function*> parameters) const
{
    return new Sum(getLhs()->evaluate(parameters), getRhs()->evaluate(parameters));
} 
