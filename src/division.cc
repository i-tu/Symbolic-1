#include <string>
#include "functions.hh"


Function* Division::simplify() const
{
	Function* sLhs = getLhs()->simplify();
	Function* sRhs = getRhs()->simplify();
	if(sLhs->isFunctionType(CONSTANT) && sRhs->isFunctionType(CONSTANT))
    {
        const Constant* left = dynamic_cast<const Constant*>(sLhs);
        const Constant* right = dynamic_cast<const Constant*>(sRhs);
        Rational n = left->getNumber();
		Rational d = right->getNumber();
		return new Constant(n/d);
    }
	else
	{
		Polynomial n(getLhs()->clone());
		Polynomial d(getRhs()->clone());
		Polynomial q(n);
		Polynomial r(n);
		Polynomial::divide(n,d,q,r);
		if (r == 0)
		{
			// remainder is 0, return the quotient
			return q.getFunction();
		}
		else if (n.getTerms().size() == 1 && d.getTerms().size() == 1)
		{
			// remainder wasn't 0, but the polynomials consist of single terms so they might reduce
			Term nt(n.getTerms()[0]);
			Term dt(d.getTerms()[0]);
			Term::divideTerms(nt,dt);
			if (!(nt == n.getTerms()[0]))
			{
				Polynomial np(nt);
				Polynomial dp(dt);
				return new Division(np.getFunction(),dp.getFunction());
			}
		}

	bool modified = false;
    
    // A solution was not found. We must go deeper...
    Division* simplified = dynamic_cast<Division*>(clone());
    
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
        return const_cast<Division*>(this);
    }
    
    return simplified;
		
	}
}

Function* Division::evaluate(std::map<Variable, Function*> parameters) const
{
	return clone();
}

Function* Division::expand() const
{
	return clone();
}

Function* Division::clone() const
{
	return new Division(getLhs()->clone(), getRhs()->clone());
}

std::string Division::print() const
{
	return "(" + getLhs()->print() + " / " + getRhs()->print() + ")";
}
// (f/g)' = (f'*g - f*g')/g^2
Function* Division::derive(const std::string var) const
{
	Function* f1 = new Multiplication(getLhs()->derive(var),getRhs()->clone());
	Function* f2 = new Multiplication(getLhs()->clone(),getRhs()->derive(var));
	Function* s = new Sum(f1,new Neg(f2));
	Function* d = new Power(getRhs()->clone(),new Constant(2));
	return new Division(s,d);
}

double Division::calculate(std::map<Variable,double>) const
{
	return 0;
}
 
