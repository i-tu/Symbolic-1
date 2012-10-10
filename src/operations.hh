#ifndef SYMBOLIC_1_OPERATIONS_HH
#define SYMBOLIC_1_OPERATIONS_HH

#include "functions.hh"

namespace Operations
{
	Function* genericAddition(const Function*, const Function*);
	Function* genericAddition(const Function&, const Function&);
	Function* genericAddition(const Constant&, const Function&);
	Function* genericAddition(const Constant&, const Constant&);
//	Function* genericAddition(const Constant&, const Sum&);
	Function* genericAddition(const Constant&, const Multiplication&);
	Function* genericAddition(const Variable&, const Function&);
	Function* genericAddition(const Variable&, const Variable&);
	Function* genericAddition(const Variable&, const Multiplication&);
	Function* genericAddition(const Multiplication&, const Multiplication&);
//	Function* genericAddition(const Power&, const Power&);
	Function* genericAddition(const Multiplication&, const Power&);

	template<typename T>
	Function* genericAddition(const T& lhs, const Sum& rhs)
	{
		Function* sum = genericAddition(lhs, *(rhs.getLhs()));
		if (dynamic_cast<Sum*>(sum)) {
			delete sum;
			Function* result = new Sum(rhs.getLhs()->simplify(), genericAddition(lhs, *(rhs.getRhs())));
			return result;
		}
		else {
			Function* result = new Sum(sum, rhs.getRhs()->simplify());
			return result;
		}
	}

	Function* genericMultiplication(const Function*, const Function*);
	Function* genericMultiplication(const Constant&, const Function&);
	Function* genericMultiplication(const Constant&, const Constant&);
	Function* genericMultiplication(const Constant&, const Power&);
	Function* genericMultiplication(const Variable&, const Function&);
	Function* genericMultiplication(const Variable&, const Variable&);
	Function* genericMultiplication(const Variable&, const Power&);

	template<typename T>
	Function* genericMultiplication(const T& lhs, const Multiplication& rhs)
	{
		Function* mul = genericMultiplication(lhs, *(rhs.getLhs()));
		if (dynamic_cast<Multiplication*>(mul)) {
			delete mul;
			Function* result = new Multiplication(rhs.getLhs()->simplify(), genericMultiplication(lhs, *(rhs.getRhs())));
			return result;
		}
		else {
			Function* result = new Multiplication(mul, rhs.getRhs()->simplify());
			return result;
		}
	}
}

#endif
 
