#include <typeinfo>
#include <map>

#include "functions.hh"
#include <string>

int Multiplication::getFunctionType() const { return MULTIPLICATION; }
bool Multiplication::isFunctionType(int x) const { return x == MULTIPLICATION; }

// Returns simplified function or NULL if simplification was not found...
Function* Multiplication::simplify() const
{
    // 2 * 3 = 6
    if(getLhs()->isFunctionType(CONSTANT) && getRhs()->isFunctionType(CONSTANT))
    {
        const Constant* left = dynamic_cast<const Constant*>(getLhs());
        const Constant* right = dynamic_cast<const Constant*>(getRhs());
        
        return new Constant(left->getNumber() * right->getNumber());
    }
    
    // x * 0 = 0
    // x * 1 = x
    else if(getLhs()->isFunctionType(CONSTANT) || getRhs()->isFunctionType(CONSTANT))
    {
        if(getLhs()->isFunctionType(CONSTANT))
        {
            const Constant* left = dynamic_cast<const Constant*>(getLhs());
            
            if(left->getNumber() == 0)
                return new Constant(0);
                
            else if(left->getNumber() == 1)
                return getRhs()->clone();
        }
        
        if(getRhs()->isFunctionType(CONSTANT))
        {
            const Constant* right = dynamic_cast<const Constant*>(getRhs());
            
            if(right->getNumber() == 0)
                return new Constant(0);
                
            else if(right->getNumber() == 1)
                return getLhs()->clone();
        }
    }
    
    // x * x = x^2
    else if(getLhs()->isFunctionType(VARIABLE) && getRhs()->isFunctionType(VARIABLE))
    {
        const Variable* right = dynamic_cast<const Variable*>(getRhs());
        const Variable* left  = dynamic_cast<const Variable*>(getLhs());
        
        if(left->getSymbol() == right->getSymbol())
            return new Power(new Variable(left->getSymbol(), "global"), new Constant(2));
    }
    // (-x) * (-y) = x * y
    else if(getLhs()->isFunctionType(NEG) && getRhs()->isFunctionType(NEG))
    {
        return new Multiplication(dynamic_cast<const Neg*>(getLhs())->getChild()->clone(),
                                  dynamic_cast<const Neg*>(getRhs())->getChild()->clone());
    }
    // (-x)*(y) = -(x*y)
    else if(getLhs()->isFunctionType(NEG))
    {
        Multiplication* me = dynamic_cast<Multiplication*>(clone());
        const Neg* neg = dynamic_cast<const Neg*>(me->getLhs());
        me->setLhs(neg->getChild()->clone());
        return new Neg(me);
    }
    // (x)*(-y) = -(x*y)
    else if(getRhs()->isFunctionType(NEG))
    {
        Multiplication* me = dynamic_cast<Multiplication*>(clone());
        const Neg* neg = dynamic_cast<const Neg*>(me->getRhs());
        me->setRhs(neg->getChild()->clone());   
        return new Neg(me);
    }
    // x * x^n = x^(n+1)
    else if((getLhs()->isFunctionType(POWER) && getRhs()->isFunctionType(VARIABLE))
         || (getRhs()->isFunctionType(POWER) && getLhs()->isFunctionType(VARIABLE)))
    {
         if(getLhs()->isFunctionType(POWER))
         {
             const Power* pow = dynamic_cast<const Power*>(getLhs());
             
             if(pow->getBase()->isFunctionType(VARIABLE))
             {
                 const Variable* var = dynamic_cast<const Variable*>(getRhs());
                 const Variable* base = dynamic_cast<const Variable*>(pow->getBase());
                 
                 if(var->getSymbol() == base->getSymbol())
                    return new Power(pow->getBase()->clone(), new Sum( pow->getExponent()->clone(), new Constant(1)));
             }
         }
         if(getRhs()->isFunctionType(POWER))
         {
             const Power* pow = dynamic_cast<const Power*>(getRhs());
             
             if(pow->getBase()->isFunctionType(VARIABLE))
             {
                 const Variable* var = dynamic_cast<const Variable*>(getLhs());
                 const Variable* base = dynamic_cast<const Variable*>(pow->getBase());
                 
                 if(var->getSymbol() == base->getSymbol())
                    return new Power(pow->getBase()->clone(), new Sum( pow->getExponent()->clone(), new Constant(1)));
             }
         }
    }
    
    // x^m * x^n = x^(m+n)
    else if(getLhs()->isFunctionType(POWER) && getRhs()->isFunctionType(POWER))
    {
         const Power* powleft = dynamic_cast<const Power*>(getLhs());
         const Power* powright = dynamic_cast<const Power*>(getRhs());
         
         if(powleft->getBase()->isFunctionType(VARIABLE)
         && powleft->getBase()->isFunctionType(VARIABLE))
         {
             const Variable* varleft = dynamic_cast<const Variable*>(powleft->getBase());
             const Variable* varright = dynamic_cast<const Variable*>(powright->getBase());
             
             if(varleft->getSymbol() == varright->getSymbol())
                return new Power(powleft->getBase()->clone(), new Sum( powright->getExponent()->clone(), powleft->getExponent()->clone() ));
         }
    }

	// (a/b)*(c/d) = (a*c)/(b*d)
    else if(getLhs()->isFunctionType(DIVISION) && getRhs()->isFunctionType(DIVISION))
    {
		const Division* divleft = dynamic_cast<const Division*>(getLhs());
		const Division* divright = dynamic_cast<const Division*>(getRhs());

		Multiplication* newN = new Multiplication(divleft->getLhs()->clone(),divright->getLhs()->clone());
		Multiplication* newD = new Multiplication(divleft->getRhs()->clone(),divright->getRhs()->clone());

		return new Division(newN,newD);
    }
    
    
    else if((getLhs()->getFunctionType() == MULTIPLICATION && getRhs()->getFunctionType() == MULTIPLICATION ))
    {
        Multiplication* simple = dynamic_cast<Multiplication*>(clone());
        
        Multiplication* traverser;
        for(traverser = dynamic_cast<Multiplication*>( simple->getLhs() );
            traverser->getLhs()->isFunctionType(MULTIPLICATION);
            traverser = dynamic_cast<Multiplication*>( traverser->getLhs() ) )
            ;
        
        Function* rightToLeft = getRhs()->clone();
        simple->setRhs(traverser->getLhs()->clone());
        traverser->setLhs(rightToLeft);
        
        return simple;
    }
    
    // chain: ((a * 2) * a) = 2*a^2
    
    // If this function is part of a chain...
    if( getLhs()->isFunctionType(MULTIPLICATION) )
    {
        bool modified = false;
        
        Multiplication* head = dynamic_cast<Multiplication*>(clone());

        Multiplication* link = head;
        Multiplication* linkfather = link;
    
        // While we have not reached the bottom,

        while(link->getLhs()->isFunctionType(MULTIPLICATION))
        {
            // save father.
            linkfather = link;

            // Descend in tree...
            link = dynamic_cast<Multiplication*>(link->getLhs());

            // If bottom is reached, we need to also check the right side.
            if( !link->getLhs()->isFunctionType(MULTIPLICATION) )
            {
                Multiplication* chained = new Multiplication(link->getLhs()->clone(), head->getRhs()->clone());
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
            
            Multiplication* chained = new Multiplication(link->getRhs()->clone(), head->getRhs()->clone());
            
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
    
    // A solution was not found. We must go deeper...
    Multiplication* simplified = dynamic_cast<Multiplication*>(clone());
    
    Function* simplifiedLeft  = simplified->getLhs()->simplify();
    Function* simplifiedRight = simplified->getRhs()->simplify();
    
    if(simplifiedLeft != simplified->getLhs())
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
        return const_cast<Multiplication*>(this);
    }
    
    return simplified;
}

Function* Multiplication::evaluate(std::map<Variable, Function*> parameters) const
{
    return new Multiplication(getLhs()->evaluate(parameters), getRhs()->evaluate(parameters));
}

Function* Multiplication::clone() const
{
	return new Multiplication(getLhs()->clone(), getRhs()->clone());
}

std::string Multiplication::print() const
{
	return getLhs()->print() + " * " + getRhs()->print();
}

// (fg)' = f'g+fg'
Function* Multiplication::derive(const std::string var) const
{ 
    Multiplication* left = new Multiplication(getLhs()->derive(var), getRhs()->clone());
    Multiplication* right = new Multiplication(getLhs()->clone(), getRhs()->derive(var));
    return new Sum(left, right);
} 
