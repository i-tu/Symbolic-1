#include <sstream>
#include "functions.hh"
#include <stdexcept>
#include <set>
#include <algorithm>



Polynomial::Polynomial(Function* f) : _terms()
{
	if (dynamic_cast<Sum*>(f))
	{

		Sum *s = dynamic_cast<Sum*>(f);
		Function *lhs = f;
		while (dynamic_cast<Sum*>(lhs))
		{
			s = dynamic_cast<Sum*>(lhs);
			Term term(s->getRhs());
			_terms.push_back(term);
			lhs = s->getLhs();
			//std::cout << term.debugPrint() << std::endl;
		}
		Term term(s->getLhs());
		_terms.push_back(term);
		//std::cout << term.debugPrint() << std::endl;
	}
	else
	{
		// f either consists of a single term or is not a polynomial
		Term term(f);
		_terms.push_back(term);
		//std::cout << term.debugPrint() << std::endl;
	}
	sortTerms();
	simplify();
}


Polynomial::Polynomial(const Polynomial &other)
{
	for (size_t i = 0; i < other._terms.size(); i++)
	{
		_terms.push_back(other._terms[i]);
	}
	simplify();
	sortTerms();
}

Polynomial::Polynomial(const Term& t)
{
	_terms.push_back(Term(t));
}

Polynomial::Polynomial(std::vector<Term> terms) : _terms(terms) 
{
	simplify();
	sortTerms();
}

void Polynomial::divide(const Polynomial &n, const Polynomial &d, Polynomial &quotient, Polynomial &remainder)
{

	Polynomial u(Term(Rational(0)));
	Polynomial r(Term(Rational(0)));
	Polynomial h(n);

	while (!(h == 0))
	{
		Term ltd(d._terms[0]);
		Term lth(h._terms[0]);
		if (Term::divideTerms(lth,ltd))
		{
			Polynomial lthPoly(lth);
			u = u + lthPoly;
			h = h - (lthPoly*d);
		}
		else
		{
			Term lth(h._terms[0]);
			Polynomial lthPoly(lth);
			r = r + lthPoly;
			h = h - lthPoly;
		}
	}

	quotient = u;
	remainder = r;

}

DegreeType Polynomial::getHighestDegreeOfVar(const Variable& var) const
{
	DegreeType max = 0;
	for (size_t i = 0; i < _terms.size(); i++)
	{
		DegreeType current = _terms[i].getDegreeOfVar(var);
		if (current > max)
		{
			max = current;
		}
	}
	return max;
}

std::set<Variable> Polynomial::getVars() const
{
	std::set<Variable> ret;
	for (size_t i = 0; i < _terms.size(); i++)
	{
		std::set<Variable> termVars = _terms[i].getVars();
		for (std::set<Variable>::iterator it = termVars.begin(); it != termVars.end(); it++)
		{
			ret.insert(*it);
		}
	}
	return ret;
}


void Polynomial::simplify()
{
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (size_t i = 0; i < _terms.size(); i++)
		{
			for (size_t j = i + 1; j < _terms.size(); j++)
			{
				if (Term::combineable(_terms[i],_terms[j]))
				{
					_terms[i] = _terms[i] + _terms[j];
					_terms[j] = Term(Rational(0));
					changed = true;
				}
			}
			if (changed)
			{
				break;
			}
		}
		removeZeros();
	}
}

Polynomial operator+(const Polynomial& p1, const Polynomial& p2)
{
	Polynomial result(p1);
	std::set<size_t> used;
	for (size_t i = 0; i < result._terms.size(); i++)
	{
		for (size_t j = 0; j < p2._terms.size(); j++)
		{
			if (Term::combineable(p1._terms[i], p2._terms[j]))
			{
				used.insert(j);
				result._terms[i] = p1._terms[i] + p2._terms[j];
			}
		}
	}
	for (size_t j = 0; j < p2._terms.size(); j++)
	{
		if (used.find(j) == used.end())
		{
			result._terms.push_back(p2._terms[j]);
		}
	}
	result.removeZeros();
	result.sortTerms();
	return result;
}

Polynomial operator-(const Polynomial& p1, const Polynomial& p2)
{
	Polynomial result(p1);
	std::set<size_t> used;
	for (size_t i = 0; i < result._terms.size(); i++)
	{
		for (size_t j = 0; j < p2._terms.size(); j++)
		{
			if (Term::combineable(p1._terms[i], p2._terms[j]))
			{
				used.insert(j);
				result._terms[i] = p1._terms[i] - p2._terms[j];
			}
		}
	}
	for (size_t j = 0; j < p2._terms.size(); j++)
	{
		if (used.find(j) == used.end())
		{
			result._terms.push_back(-p2._terms[j]);
		}
	}
	result.removeZeros();
	result.sortTerms();
	return result;
}

Polynomial operator*(const Polynomial& p1, const Polynomial& p2)
{
	Polynomial result;
	for (size_t i = 0; i < p1._terms.size(); i++)
	{
		for(size_t j = 0; j < p2._terms.size(); j++)
		{
			result._terms.push_back(p1._terms[i]*p2._terms[j]);
		}
	}
	result.simplify();
	return result;
}

bool operator==(const Polynomial& p1, int i)
{
	if (p1._terms.size() == 0)
	{
		return i == 0;
	}
	else if (p1._terms.size() == 1)
	{
		if (p1._terms[0].getHighestDegree() == 0)
		{
			return p1._terms[0].getCoeff() == Rational(i);
		}
	}
	return false;
}

Function* Polynomial::getFunction() const
{
	Function* f;
	if (_terms.empty())
	{
		f = new Constant(0);
	}
	else
	{
		Function *previous = NULL;
		for (size_t i = 0; i < _terms.size(); i++)
		{
			const Term &term = _terms[i];
			if (previous == NULL)
			{
				previous = term.getFunction();
			}
			else
			{
				previous = new Sum(previous,term.getFunction());
			}
		}
		f = previous;
	}
	return f;
}

void Polynomial::sortTerms()
{
	std::sort(_terms.begin(),_terms.end());
}

void Polynomial::removeZeros()
{
	for (std::vector<Term>::iterator it = _terms.begin(); it < _terms.end(); it++)
	{
		if (*it == 0)
			it = _terms.erase(it);
		if (it == _terms.end())
			break;
	}
}

std::set<Variable> Term::getVars() const
{
	std::set<Variable> ret;
	for (std::map<Variable,DegreeType>::const_iterator it = _degrees.begin(); it != _degrees.end(); it++)
	{
		ret.insert(it->first);
	}
	return ret;
}

Term::Term(Function* f) : _degrees()
{
	// find the type of f and populate the degrees map
	if (dynamic_cast<Multiplication*>(f))
	{
		bool coeffFound = false;
		Multiplication* mult = dynamic_cast<Multiplication*>(f);
		Function* lhs = f;
		while (dynamic_cast<Multiplication*>(lhs))
		{
			mult = dynamic_cast<Multiplication*>(lhs);
			if (dynamic_cast<Constant*>(mult->getRhs()))
			{
				Constant* con = dynamic_cast<Constant*>(mult->getRhs());
				if (coeffFound)
					_coeff = _coeff * con->getNumber();
				else
					_coeff = con->getNumber();
				coeffFound = true;
			}
			else if (dynamic_cast<Variable*>(mult->getRhs()))
			{
				Variable var = *dynamic_cast<Variable*>(mult->getRhs());
				_degrees[var] += 1;
			}
			else if(dynamic_cast<Power*>(mult->getRhs()))
			{
				Power* pow = dynamic_cast<Power*>(mult->getRhs());
				Variable var = *dynamic_cast<Variable*>(pow->getBase());
				Constant* c = dynamic_cast<Constant*>(pow->getExponent());
				_degrees[var] += getDegreeFromRational(c->getNumber());
			}
			else if (dynamic_cast<Neg*>(mult->getLhs()))
			{
				Neg *n = dynamic_cast<Neg*>(mult->getLhs());
				if (dynamic_cast<Constant*>(n->getChild()))
				{
					Constant* con = dynamic_cast<Constant*>(n->getChild());
					if (coeffFound)
						_coeff = _coeff * (-1) * con->getNumber();
					else
						_coeff = -1 * con->getNumber();
					coeffFound = true;
				}
				else if (dynamic_cast<Variable*>(n->getChild()))
				{
					Variable var = *dynamic_cast<Variable*>(n->getChild());
					_degrees[var] += 1;
					if (!coeffFound)
						_coeff = -1;
					else
						_coeff = -1 * _coeff;
					coeffFound = true;
				}
			}

			lhs = mult->getLhs();
		}

		// handle rightmost multiplicand
		if (dynamic_cast<Constant*>(mult->getLhs()))
		{
			Constant* con = dynamic_cast<Constant*>(mult->getLhs());
			if (coeffFound)
				_coeff = _coeff * con->getNumber();
			else
				_coeff = con->getNumber();
			coeffFound = true;
		}
		else if (dynamic_cast<Neg*>(mult->getLhs()))
		{
			Neg *n = dynamic_cast<Neg*>(mult->getLhs());
			if (dynamic_cast<Constant*>(n->getChild()))
			{
				Constant* con = dynamic_cast<Constant*>(n->getChild());
				if (coeffFound)
					_coeff = _coeff * (-1) * con->getNumber();
				else
					_coeff = -1 * con->getNumber();
				coeffFound = true;
			}
			else if (dynamic_cast<Variable*>(n->getChild()))
			{
				Variable var = *dynamic_cast<Variable*>(n->getChild());
				_degrees[var] += 1;
				if (!coeffFound)
					_coeff = -1;
				else
					_coeff = -1 * _coeff;
				coeffFound = true;
			}
		}
		else if (dynamic_cast<Variable*>(mult->getLhs()))
		{
			Variable var = *dynamic_cast<Variable*>(mult->getLhs());
			_degrees[var] += 1;
		}
		else if(dynamic_cast<Power*>(mult->getLhs()))
		{
			Power* pow = dynamic_cast<Power*>(mult->getLhs());
			Variable var = *dynamic_cast<Variable*>(pow->getBase());
			_degrees[var] += getDegreeFromRational(dynamic_cast<Constant*>(pow->getExponent())->getNumber());
		}
		if (!coeffFound)
		{
			_coeff = 1;
		}
	}
	else if (dynamic_cast<Power*>(f))
	{
		Power* pow = dynamic_cast<Power*>(f);
		Variable var = *dynamic_cast<Variable*>(pow->getBase());
		_degrees[var] = getDegreeFromRational(dynamic_cast<Constant*>(pow->getExponent())->getNumber());
		_coeff = 1;
	}
	else if (dynamic_cast<Variable*>(f))
	{
		Variable var = *dynamic_cast<Variable*>(f);
		_degrees[var] = 1;
		_coeff = 1;
	}
	else if (dynamic_cast<Constant*>(f))
	{
		Constant* con = dynamic_cast<Constant*>(f);
		_coeff = con->getNumber();
	}
	else if (dynamic_cast<Neg*>(f))
	{
		Neg *n = dynamic_cast<Neg*>(f);
		if (dynamic_cast<Constant*>(n->getChild()))
		{
			Constant* con = dynamic_cast<Constant*>(n->getChild());
			_coeff = -1*con->getNumber();
		}
		else if (dynamic_cast<Variable*>(n->getChild()))
		{
			Variable var = *dynamic_cast<Variable*>(n->getChild());
			_degrees[var] = 1;
			_coeff = -1;
		}
	}
	else
	{
		throw new std::logic_error("\"" + f->print() + "\" is not a polynomial term.");
	}

}

Term::Term(const Term& other) : _coeff(other._coeff)
{
	for (std::map<Variable,DegreeType>::const_iterator it = other._degrees.begin(); it != other._degrees.end(); it++)
	{
		_degrees[it->first] = it->second;
	}
}
Term::Term(const Rational &r) : _degrees(), _coeff(r)
{

}
std::string Term::debugPrint() const
{
	std::stringstream ss("");
	ss << "function: " << getFunction()->print() << std::endl;
	ss << "coefficient: " << _coeff << std::endl;
	ss << "var\tdegree" << std::endl;
	for (std::map<Variable,DegreeType>::const_iterator it = _degrees.begin(); it != _degrees.end(); it++)
	{
		ss << it->first.print() << "\t" << it->second << std::endl;
	}
	ss << "max degree: " << getHighestDegree() << std::endl;
	return ss.str();
}

DegreeType Term::getHighestDegree() const
{
	DegreeType max = 0;
	for (std::map<Variable,DegreeType>::const_iterator it = _degrees.begin(); it != _degrees.end(); it++)
	{
		if (it->second  > max)
			max = it->second;
	}
	return max;
}

DegreeType Term::getTotalDegree() const
{
	DegreeType result = 0;
	for (std::map<Variable,DegreeType>::const_iterator it = _degrees.begin(); it != _degrees.end(); it++)
	{
		result += it->second;
	}
	return result;
}

DegreeType Term::getDegreeOfVar(const Variable &var) const
{
	if (_degrees.empty())
		return 0;
	else if (_degrees.find(var) == _degrees.end())
		return 0;
	else
		return _degrees.find(var)->second;
}


bool Term::divideTerms(Term &n, Term &d)
{
	std::vector<Variable> deleteFromN, deleteFromD;
	for (std::map<Variable,DegreeType>::iterator it = n._degrees.begin(); it != n._degrees.end(); it++)
	{
		if (d._degrees.find(it->first) != d._degrees.end())
		{
			DegreeType newDeg = n._degrees[it->first] - d._degrees[it->first];
			if (newDeg > 0)
			{
				n._degrees[it->first] = newDeg;
				deleteFromD.push_back(it->first);
			}
			else if (newDeg == 0)
			{
				deleteFromN.push_back(it->first);
				deleteFromD.push_back(it->first);
			}
			else if (n._degrees[it->first] > 0)
			{
				deleteFromN.push_back(it->first);
				d._degrees[it->first] = -newDeg;
			}
		}
	}
	for (size_t i = 0; i < deleteFromN.size(); i++)
	{
		n._degrees.erase(deleteFromN[i]);
	}
	for (size_t i = 0; i < deleteFromD.size(); i++)
	{
		d._degrees.erase(deleteFromD[i]);
	}
	n._coeff = n._coeff / d._coeff;

	if (d._degrees.empty())
		return true;
	else
		return false;
}

Function* Term::getFunctionFromVar(const Variable &var) const
{
	if (_degrees.find(var)->second == 1)
	{
		return new Variable(var);
	}
	else
	{
		return new Power(new Variable(var),new Constant(_degrees.find(var)->second));
	}
}

Function* Term::getFunction() const
{
	Function* f;
	if (_degrees.empty())
	{
		f = new Constant(_coeff);
	}
	else
	{
		Function *previous = NULL;
		for (std::map<Variable,DegreeType>::const_iterator it = _degrees.begin(); it != _degrees.end(); it++)
		{
			const Variable &var = it->first;
			DegreeType deg = it->second;
			if (previous == NULL)
			{
				previous = getFunctionFromVar(var);
			}
			else
			{
				previous = new Multiplication(previous,getFunctionFromVar(var));
			}
		}
		if (!(Rational(1) == _coeff))
			f = new Multiplication(previous,new Constant(_coeff));
		else
			f = previous;
	}
	return f;
}
DegreeType Term::getDegreeFromRational(const Rational& r)
{
	int n = r.getNumerator();
	int d = r.getDenominator();
	if (d == 1)
	{
		return n;
	}
	else
	{
		std::ostringstream oss;
		oss << r << " is not a valid polynomial degree.";
		throw new std::logic_error(oss.str().c_str());
	}
} 

Term operator*(const Term& t1, const Term& t2)
{
	std::set<Variable> handledVars;
	Term result(t1);
	for (std::map<Variable,DegreeType>::const_iterator it = t1._degrees.begin(); it != t1._degrees.end(); it++)
	{
		result._degrees[it->first] += t2.getDegreeOfVar(it->first);
		handledVars.insert(it->first);
	}
	for (std::map<Variable,DegreeType>::const_iterator it = t2._degrees.begin(); it != t2._degrees.end(); it++)
	{
		if (handledVars.find(it->first) == handledVars.end())
			result._degrees[it->first] += t2.getDegreeOfVar(it->first);
	}
	result._coeff = t1._coeff*t2._coeff;
	return result;
}

bool operator<(const Term& t1, const Term& t2)
{
	if (t1._degrees.empty())
		return false;
	else if (t2._degrees.empty())
		return true;
	else if (t1.getTotalDegree() > t2.getTotalDegree())
		return true;
	else if (t1.getTotalDegree() == t2.getTotalDegree())
		return *t1._degrees.begin() < *t2._degrees.begin();
	else
		return false;
}


bool Term::combineable(const Term& t1, const Term& t2)
{
	for (std::map<Variable,DegreeType>::const_iterator it = t1._degrees.begin(); it != t1._degrees.end(); it++)
	{
		if (t1.getDegreeOfVar(it->first) != t2.getDegreeOfVar(it->first))
			return false;
	}
	for (std::map<Variable,DegreeType>::const_iterator it = t2._degrees.begin(); it != t2._degrees.end(); it++)
	{
		if (t1.getDegreeOfVar(it->first) != t2.getDegreeOfVar(it->first))
			return false;
	}
	return true;
}

Term operator+(const Term& t1, const Term& t2)
{
	if (!Term::combineable(t1,t2))
		throw new std::logic_error("");
	Term result(t1);
	result._coeff += t2._coeff;
	if (result._coeff == 0)
		result._degrees.clear();
	return result;
}

Term operator-(const Term& t1, const Term& t2)
{
	if (!Term::combineable(t1,t2))
		throw new std::logic_error("");
	Term result(t1);
	result._coeff -= t2._coeff;
	if (result._coeff == 0)
		result._degrees.clear();
	return result;
}

Term operator-(const Term& t1)
{
	Term result(t1);
	result._coeff = -1*t1._coeff;
	return result;
}

bool operator==(const Term& t1, int i)
{
	if (t1._degrees.empty())
	{
		Rational tmp(t1._coeff);
		return tmp == Rational(i);
	}
	else
		return false;
}

bool operator==(const Term& t1, const Term& t2)
{
	if (t1._degrees.size() != t2._degrees.size())
		return false;
	else
		for (std::map<Variable,DegreeType>::const_iterator it = t1._degrees.begin(); it != t1._degrees.end(); it++)
			if (t1.getDegreeOfVar(it->first) != t2.getDegreeOfVar(it->first))
				return false;
	Rational tmp(t1._coeff);
	return tmp == t2._coeff;
}

