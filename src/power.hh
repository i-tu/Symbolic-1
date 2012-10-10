#ifndef SYMBOLIC_1_POWER_HH
#define SYMBOLIC_1_POWER_HH

#include <string>
#include <map>

class Power : public BinaryOperation
{
	public:
	Power() : BinaryOperation() { }
	Power(Function* base, Function* exponent) : BinaryOperation(base, exponent) { }
	Power(const Power& other) : BinaryOperation(other) { }

	const Function* getBase() const;
	Function* getBase();
	const Function* getExponent() const;
	Function* getExponent();

	Function* simplify() const;
	Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* clone() const;
	Function* derive(const std::string) const;
	std::string print() const;
	
	int getFunctionType() const;
	bool isFunctionType(int x) const;

	static bool equals(const Power* p1, const Power* p2);
};

#endif
 
