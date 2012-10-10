#ifndef SYMBOLIC_1_VARIABLE_HH
#define SYMBOLIC_1_VARIABLE_HH

#include <map>

class Variable : public NullaryOperation
{
	public:
	Variable(std::string symbol, std::string ns) : _symbol(symbol), _namespace(ns) {}
	Variable(const Variable& other) : _symbol(other._symbol), _namespace(other._namespace) { }
	
	std::string getSymbol() const;
	std::string getNamespace() const;

	Function* simplify() const;
	Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* clone() const;
	std::string print() const;
	Function* derive(const std::string) const;
	int getFunctionType() const;
	bool isFunctionType(int x) const;
	
	bool operator==(const Function& other) const;
	bool operator<(const Variable& other) const;
	
	private:
	std::string _symbol;
	std::string _namespace;

};

#endif
 
