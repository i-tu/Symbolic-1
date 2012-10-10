#ifndef SYMBOLIC_1_MULTIPLICATION_HH
#define SYMBOLIC_1_MULTIPLICATION_HH

#include <string>
#include <map>

class Multiplication : public BinaryOperation
{
	public:
	Multiplication() : BinaryOperation() { }
	Multiplication(Function* lhs, Function* rhs) : BinaryOperation(lhs, rhs) { }
	Multiplication(const Multiplication& other) : BinaryOperation(other) { }
	Function* simplify() const;
    Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* expand() const { return clone(); }
	Function* clone() const;
	Function* derive(const std::string) const;
	std::string print() const;
	int getFunctionType() const;
	bool isFunctionType(int x) const;
	
};

#endif
 
