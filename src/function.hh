#ifndef SYMBOLIC_1_FUNCTION_HH
#define SYMBOLIC_1_FUNCTION_HH

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

#define CONSTANT 0
#define VARIABLE 1
#define SUM 2
#define MULTIPLICATION 3
#define POWER 4
#define DIVISION 5
#define NEG 6
#define IDENTITY -1

class Variable;

class Function
{
	public:
	
	// What is an empty function?
	Function() : _variables() { }

	// one-variable function
	Function(Variable variable);

	// multivariable function
	Function(std::set<Variable> variables) : _variables(variables) { }

	// Copy constructor
	Function(const Function& other) : _variables(other._variables) { }

	virtual ~Function() { };

	// Returns a pointer to a new, simplified function derived from this function.
	// We always generate a whole new "Function" tree, so the original one stays intact
	virtual Function* simplify() const = 0;

	// Evaluate this function at points given by the parameter map.
	// The parameter map has variables as keys and evaluation points as values.
	// We always generate a whole new "Function" tree, so the original one stays intact
	virtual Function* evaluate(std::map<Variable, Function*> parameters) const = 0;

	virtual std::set<Variable> getVariables() const;
	virtual Function* clone() const = 0;

	virtual std::string print() const;
	
	virtual Function* derive(const std::string) const = 0;
    
    virtual int getFunctionType() const = 0;
	virtual bool isFunctionType(int x) const = 0;
	
	Function* simplifyTree() const;

/*
	virtual Function* add(Function* other) const;
	virtual Function* subtract(Function* other) const;
	virtual Function* multiply(Function* other) const;
*/
	private:

	// a list of symbols that are interpreted as variables of this function.
	// e.g. f(x,y) = x²y³ has variables x and y.
	std::set<Variable> _variables;

};

std::ostream& operator<<(std::ostream& os, const Function& f);

class NaryOperation : public Function
{
	public:
	NaryOperation(size_t arity) : _operands(arity) { }
	NaryOperation(std::vector<Function*> operands) : _operands(operands) { }
	virtual ~NaryOperation();

	virtual void addChild(Function* child) = 0;
	virtual const std::vector<Function*> getOperands() const { return _operands; }

	protected:
	std::vector<Function*> _operands;
};

class NullaryOperation : public NaryOperation
{
	public:
	NullaryOperation() : NaryOperation(0) { }
	void addChild(Function* child) { }
	const std::vector<Function*> getOperands() const;
};

class UnaryOperation : public NaryOperation
{
	public:
	UnaryOperation(Function* operand);
	void addChild(Function* child);
	virtual const Function* getChild() const;
	virtual Function* getChild();
};

class BinaryOperation : public NaryOperation
{
	public:
	BinaryOperation();
	BinaryOperation(Function* lhs, Function* rhs = 0);
	BinaryOperation(const BinaryOperation& other);
//	virtual ~BinaryOperation();

	void addChild(Function* child);

	virtual const Function* getRhs() const;
	virtual Function* getRhs();
	virtual const Function* getLhs() const;
	virtual Function* getLhs();
	virtual void setRhs(Function* rhs);
	virtual void setLhs(Function* lhs);
	
};


class IdentityOperation : public UnaryOperation
{
	public:
	IdentityOperation(Function* operand) : UnaryOperation(operand) { }

	Function* simplify() const;
	Function* evaluate(std::map<Variable, Function*> parameters) const;
	Function* clone() const;
	Function* derive(const std::string) const;
	int getFunctionType() const;
	bool isFunctionType(int x) const;
};


/*
class Polynomial : public Function
{
	public:
	Polynomial(long constant)
	{
		_terms[0] = constant;
	}

	Polynomial(std::map<int, long> terms, std::string variable) : Function(variable), terms(_terms) { }

	Polynomial(const Polynomial& other) : _terms(other._terms) { }

	long operator[](int exponent);
	Polynomial& operator=(const Polynomial& other);
	Polynomial& operator+=(const Polynomial& other);
	Function* simplify() const { return new Polynomial(*this); }

	private:
	// a map of the terms having the exponent of the variable as key and the coefficient as value.
	std::map<int, long> _terms;

};
*/

#endif
 
