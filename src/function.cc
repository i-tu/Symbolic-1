#include <typeinfo>
#include <sstream>
#include <string>

#include "functions.hh"
#include "treeFunctions.hh"


//	Function

Function::Function(Variable variable)
{
	_variables.insert(variable);
}

std::set<Variable> Function::getVariables() const
{
	return _variables;
}

std::string Function::print() const
{
	return "(unknown function)";
}

//	NaryOperation

NaryOperation::~NaryOperation()
{
	for (std::vector<Function*>::iterator i = _operands.begin(); i != _operands.end(); i++) {
		if (*i != 0)
		    delete *i;
	}
}


//	NullaryOperation

const std::vector<Function*> NullaryOperation::getOperands() const
{
	return std::vector<Function*>(0);
}


// UnaryOperation

UnaryOperation::UnaryOperation(Function* operand) : NaryOperation(1)
{
	_operands[0] = operand;
}
/*
BinaryOperation::~BinaryOperation()
{
	if (_operands[0] != 0)
		delete _operands[0];
}*/

void UnaryOperation::addChild(Function* child)
{
	if (_operands[0] != 0)
		delete _operands[0];
	_operands[0] = child;
}
const Function* UnaryOperation::getChild() const
{
	return _operands[0];
}
Function* UnaryOperation::getChild()
{
	return _operands[0];
}

//	BinaryOperation

BinaryOperation::BinaryOperation() : NaryOperation(2)
{
	_operands[0] = 0;
	_operands[1] = 0;
}

BinaryOperation::BinaryOperation(Function* lhs, Function* rhs) : NaryOperation(2)
{
	_operands[0] = lhs;
	_operands[1] = rhs;

}

BinaryOperation::BinaryOperation(const BinaryOperation& other) : NaryOperation(2)
{
	setRhs(other.getRhs()->clone());
	setLhs(other.getLhs()->clone());
}

/*
BinaryOperation::~BinaryOperation()
{
	if (_operands[0] != 0)
		delete _operands[0];
	if (_operands[1] != 0)
		delete _operands[1];
}
*/

void BinaryOperation::addChild(Function* child)
{
	setRhs(child);
}

const Function* BinaryOperation::getRhs() const
{
	return _operands[1];
}

Function* BinaryOperation::getRhs()
{
	return _operands[1];
}

const Function* BinaryOperation::getLhs() const
{
	return _operands[0];
}

Function* BinaryOperation::getLhs()
{
	return _operands[0];
}

void BinaryOperation::setRhs(Function* rhs)
{
	if (_operands[1] != 0)
		delete _operands[1];
	_operands[1] = rhs;
}

void BinaryOperation::setLhs(Function* lhs)
{
	if (_operands[0] != 0)
		delete _operands[0];
	_operands[0] = lhs;
}


//
//	IdentityOperation
//

Function* IdentityOperation::simplify() const
{
	return _operands[0]->simplify();
}

Function* IdentityOperation::evaluate(std::map<Variable, Function*> parameters) const
{
	return _operands[0]->evaluate(parameters);
}

Function* IdentityOperation::clone() const
{
	return new IdentityOperation(_operands[0]->clone());
}

// Simplify whole tree. Most simple possible tree is guaranteed.

Function* Function::simplifyTree() const
{
    Function* simple  = NULL;
    Function* simpler = dynamic_cast<Function*>(clone());

    while(simple != simpler)
    {
    	if(simple)
    		delete simple;
        simple = simpler;
        simpler = simpler->simplify();
    }

    return simple;
}

// Polynomial is not needed - this is here for reference

/*
long Polynomial::operator[](int exponent)
{ 
	if (_terms.find(exponent) == _terms.end())
		return 0;
	return _terms[exponent];
}

Polynomial& Polynomial::operator=(const Polynomial& other)
{
	this->_terms = other._terms;
	return *this;
}

Polynomial& Polynomial::operator+=(const Polynomial& other)
{
	for (std::map<int, long>::iterator i = _terms.begin(); i != _terms.end(); i++)
		i->second += other[i->first];
}

Function* Polynomial::simplify() const { return new Polynomial(*this); }
*/

// In all functions we first test for the trivial case D(f(a1,a2...)) dx == 0, (a1,a2...!=x)


Function* Function::derive(const std::string var) const
{
    return clone();
}

Function* IdentityOperation::derive(const std::string var) const
{
    return new Variable("IdentityOperationDerivative", "global");
}

int IdentityOperation::getFunctionType() const { return IDENTITY; }
bool IdentityOperation::isFunctionType(int x) const { return x == IDENTITY; }
 
