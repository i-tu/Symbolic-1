#ifndef SYMBOLIC_1_NEG_HH
#define SYMBOLIC_1_NEG_HH

#include <string>
#include <map>

class Neg : public UnaryOperation
{
	public:
	Neg(Function* child) : UnaryOperation(child) { }
	Neg(const Neg& other) : UnaryOperation(other) { }
	Function* simplify() const;
    Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* clone() const;
	Function* derive(const std::string) const;
	std::string print() const;
	int getFunctionType() const;
	bool isFunctionType(int x) const;
};

#endif
