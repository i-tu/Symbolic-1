#ifndef SYMBOLIC_1_POLYNOMIAL_HH
#define SYMBOLIC_1_POLYNOMIAL_HH

#include <utility>
#include <vector>
#include <set>
#include "functions.hh"

typedef long DegreeType;

class Term
{

public:

	//enum functionType {MULTIPLICATION, POWER, VARIABLE, CONSTANT};

	explicit Term(Function* f);

	Term(const Term& );

	explicit Term(const Rational &r);

	DegreeType getHighestDegree() const;

	DegreeType getTotalDegree() const;

	DegreeType getDegreeOfVar(const Variable &var) const;

	std::string print() const;

	std::string debugPrint() const;

	static bool divideTerms(Term &n, Term &d);

	Function* getFunctionFromVar(const Variable &var) const;

	Function* getFunction() const;

	std::set<Variable> getVars() const;

	static bool combineable(const Term& t1, const Term& t2);

	friend Term operator*(const Term& t1, const Term& t2);

	friend Term operator+(const Term& t1, const Term& t2);

	friend Term operator-(const Term& t1, const Term& t2);

	friend Term operator-(const Term& t1);

	friend bool operator<(const Term& t1, const Term& t2);

	friend bool operator==(const Term& t1, int);

	friend bool operator==(const Term& t1, const Term& t2);

	Rational getCoeff() const
	{
		return _coeff;
	}

	static DegreeType getDegreeFromRational(const Rational& r);


private:

	std::map<Variable,DegreeType> _degrees;

	Rational _coeff;

};

class Polynomial
{
public:

	Polynomial() { }

	explicit Polynomial(Function* f);

	explicit Polynomial(const Term&);

	explicit Polynomial(std::vector<Term> terms);

	Polynomial(const Polynomial &other);

	//std::vector<Term> getTermsContainingVarOfDegree(const Variable &var, const DegreeType &degree) const;

	//std::vector<Term> getTermsContainingHighestDegreeOfVar(const Variable &var) const;

	DegreeType getHighestDegreeOfVar(const Variable& var) const;

	static void divide(const Polynomial &n, const Polynomial &d, Polynomial &quotient, Polynomial &remainder);

	Function* getFunction() const;

	std::set<Variable> getVars() const;

	void sortTerms();

	void removeZeros();

	void simplify();

	friend Polynomial operator+(const Polynomial& p1, const Polynomial& p2);

	friend Polynomial operator-(const Polynomial& p1, const Polynomial& p2);

	friend Polynomial operator*(const Polynomial& p1, const Polynomial& p2);

	friend bool operator==(const Polynomial& p1, int);

	std::vector<Term> getTerms() const
	{
		return _terms;
	}

private:

	bool compareTerms(Term* t1, Term* t2);

	std::vector<Term> _terms;
};




#endif
 
