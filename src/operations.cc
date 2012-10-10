#include "operations.hh"
#include <typeinfo>
#include <iostream>

// In the most general case we just return a Sum object.
// If there is a more specific overload, that will be used instead.
Function* Operations::genericAddition(const Function* lhs, const Function* rhs)
{
	return genericAddition(*lhs, *rhs);
}

Function* Operations::genericAddition(const Function& lhs, const Function& rhs)
{
	try	{
		return genericAddition(dynamic_cast<const Constant&>(lhs), rhs);
	} catch (const std::bad_cast& bce) { }

	try {
		return genericAddition(dynamic_cast<const Variable&>(lhs), rhs);
	} catch (const std::bad_cast& bce) { }

//	std::cout << "Couldn't add them together." <<std::endl;
	return new Sum(lhs.clone(), rhs.clone());
}

Function* Operations::genericAddition(const Constant& lhs, const Function& rhs)
{
//	std::cout << "So the first parameter is a constant." << std::endl;
	try	{
		return genericAddition(lhs, dynamic_cast<const Constant&>(rhs));
	} catch (const std::bad_cast& bce) { }

	try {
		return genericAddition(lhs, dynamic_cast<const Sum&>(rhs));
	} catch (const std::bad_cast& bce) { }

	try {
		return genericAddition(lhs, dynamic_cast<const Multiplication&>(rhs));
	} catch (const std::bad_cast& bce) { }

//	std::cout << "Couldn't add them together." <<std::endl;
	return new Sum(lhs.clone(), rhs.clone());
}

Function* Operations::genericAddition(const Constant& lhs, const Constant& rhs)
{
//	std::cout << "So the second parameter is a constant too." << std::endl;
	if (lhs.isNumber() && rhs.isNumber())
		return new Constant(lhs.getNumber() + rhs.getNumber());
	else
	{
		if (lhs.getSymbol() == rhs.getSymbol())
			return new Multiplication(new Constant(2), lhs.clone());
		else
			return new Sum(lhs.clone(), rhs.clone());
	}
	
}

Function* Operations::genericAddition(const Constant& constant, const Multiplication& multiplication)
{
	std::cout << "adding constant " << constant.print() << " with multiplication " << multiplication.print() <<std::endl;
	std::cout << std::boolalpha << constant.print() << " == " << multiplication.getLhs()->print() << ": ";
	std::cout << ( constant == *(multiplication.getLhs()) ) << std::endl;
	std::cout << constant.print() << " == " << multiplication.getRhs()->print() << ": ";
	std::cout << ( constant == *(multiplication.getRhs()) ) << std::endl;
	if (constant == *(multiplication.getLhs()))
		return new Multiplication(constant.clone(), genericAddition(Constant(1.0), *(multiplication.getRhs())));
	else if (constant == *(multiplication.getRhs()))
		return new Multiplication(constant.clone(), genericAddition(Constant(1.0), *(multiplication.getLhs())));
	return new Sum(constant.clone(), multiplication.clone());
}

Function* Operations::genericAddition(const Variable& var, const Function& f)
{
	try {
		return genericAddition(var, dynamic_cast<const Variable&>(f));
	} catch (const std::bad_cast& bce) { }

	try {
		return genericAddition(var, dynamic_cast<const Sum&>(f));
	} catch (const std::bad_cast& bce) { }

	return new Sum(var.clone(), f.clone());
}

Function* Operations::genericAddition(const Variable& lhs, const Variable& rhs)
{
	if (lhs == rhs)
		return new Multiplication(new Constant(2.0), lhs.clone());
	return new Sum(lhs.clone(), rhs.clone());
}

Function* Operations::genericAddition(const Variable& var, const Multiplication& multiplication)
{
	if (var == *(multiplication.getLhs()))
		return new Multiplication(var.clone(), genericAddition(Constant(1.0), *(multiplication.getRhs())));
	else if (var == *(multiplication.getRhs()))
		return new Multiplication(var.clone(), genericAddition(Constant(1.0), *(multiplication.getLhs())));
	return new Sum(var.clone(), multiplication.clone());
}

Function* Operations::genericAddition(const Multiplication& lhs, const Multiplication& rhs)
{
	Function* res = genericAddition(*(lhs.getLhs()), rhs);
	if (dynamic_cast<Sum*>(res))
	{
		delete res;
		res = genericAddition(*(lhs.getRhs()), rhs);
		return new Sum(lhs.getLhs()->clone(), res);
	}
	return new Sum(lhs.getRhs()->clone(), res);
}
/*
Function* Operations::genericAddition(const Power& lhs, const Power& rhs)
{
	if ()
}

Function* Operations::genericAddition(const Multiplication& m, const Power& p)
{
*/	

Function* Operations::genericMultiplication(const Function* lhs, const Function* rhs)
{
	try {
		return genericMultiplication(dynamic_cast<const Constant&>(*lhs), *rhs);
	} catch (const std::bad_cast& bce) { }
	try	{
		return genericMultiplication(dynamic_cast<const Variable&>(*lhs), *rhs);
	} catch (const std::bad_cast& bce) { }

	return new Multiplication(lhs->clone(), rhs->clone());
}

Function* Operations::genericMultiplication(const Constant& c, const Function& f)
{
	try {
		return genericMultiplication(c, dynamic_cast<const Constant&>(f));
	} catch (const std::bad_cast& bce) { }
	try {
		return genericMultiplication(c, dynamic_cast<const Multiplication&>(f));
	} catch (const std::bad_cast& bce) { }
	/*
	try {
		return genericMultiplication(c, dynamic_cast<const Sum&>(f));
	} catch (const std::bad_cast& bce) { }
*/
	return new Multiplication(c.clone(), f.clone());
}

Function* Operations::genericMultiplication(const Constant& lhs, const Constant& rhs)
{
	if (lhs.isNumber() && rhs.isNumber())
		return new Constant(lhs.getNumber() * rhs.getNumber());
	else if (!(lhs.isNumber() && rhs.isNumber()) && lhs == rhs)
		return new Power(lhs.clone(), new Constant(2.0));

	return new Multiplication(lhs.clone(), rhs.clone());
}

Function* Operations::genericMultiplication(const Constant& c, const Power& p)
{
	if (c == *(p.getBase()))
		return new Power(c.clone(), genericAddition(Constant(1.0), *(p.getExponent())));
	return new Multiplication(c.clone(), p.clone());
}

Function* Operations::genericMultiplication(const Variable& v, const Function& f)
{
	try {
		return genericMultiplication(v, dynamic_cast<const Variable&>(f));
	} catch (const std::bad_cast& bce) { }
	try {
		return genericMultiplication(v, dynamic_cast<const Multiplication&>(f));
	} catch (const std::bad_cast& bce) { }
	return new Multiplication(v.clone(), f.clone());
}

Function* Operations::genericMultiplication(const Variable& lhs, const Variable& rhs)
{
	if (lhs == rhs)
		return new Power(lhs.clone(), new Constant(2.0));
	return new Multiplication(lhs.clone(), rhs.clone());
}

Function* Operations::genericMultiplication(const Variable& v, const Power& p)
{
	if (v == *(p.getBase()))
		return new Power(v.clone(), genericAddition(Constant(1.0), *(p.getExponent())));
	return new Multiplication(v.clone(), p.clone());
}
 
