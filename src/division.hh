#ifndef SYMBOLIC_1_DIVISION_HH
#define SYMBOLIC_1_DIVISION_HH

#include "functions.hh"
class Division : public BinaryOperation
{
	public:
	Division() : BinaryOperation() { }
	Division(Function* lhs, Function* rhs) : BinaryOperation(lhs, rhs) { }
	Function* simplify() const;
	Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* expand() const;
	Function* clone() const;
	std::string print() const;
	int getFunctionType() const { return DIVISION; }
    bool isFunctionType(int x) const { return x == DIVISION; }
	Function* derive(const std::string var) const;
	double calculate(std::map<Variable,double>) const;
};

#endif
 
