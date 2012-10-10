#ifndef SYMBOLIC_1_CONSTANT_HH
#define SYMBOLIC_1_CONSTANT_HH

#include <string>
#include <map>
#include "functions.hh"
#include "rational.hh"
#include "polynomial.hh"

class Constant : public NullaryOperation
{
	public:

	Constant(const Constant& other)
		: _number(other._number)
		{ }

	//Constant(double number) : _number(number) { }

	Constant(const Rational &r) : _number(r) { }

	Constant(int i) : _number(Rational(i)) { }

	Rational getNumber() const { return _number; }

	Function* simplify() const;
	Function* evaluate(std::map<Variable, Function*> parameters) const;
    Function* clone() const { return new Constant(getNumber()); }
	Function* expand() const { return clone(); }
	Function* derive(const std::string) const;
	double calculate(std::map<Variable, double> parameters) const;
	std::string print() const;
	
	int getFunctionType() const;
	bool isFunctionType(int x) const;

	bool operator==(const Function& other) const;

	private:
	Rational _number;
};

#endif 
