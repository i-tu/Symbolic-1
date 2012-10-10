#ifndef SYMBOLIC_1_SUM_HH
#define SYMBOLIC_1_SUM_HH

#include <string>
#include <map>

class Sum : public BinaryOperation
{
	public:
	Sum() : BinaryOperation() {  }
	Sum(Function* lhs, Function* rhs) : BinaryOperation(lhs, rhs) {  }
	Sum(const Sum& other) : BinaryOperation(other) { }
	Function* simplify() const;
    Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* clone() const;
	Function* derive(const std::string) const;
	std::string print() const;
	int getFunctionType() const;
	bool isFunctionType(int x) const;
};

#endif
 
