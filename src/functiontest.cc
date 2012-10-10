#include "function.hh"
#include <iostream>

int main()
{

	Sum* sum = new Sum(new Variable("x", "ns"), new Variable("x", "ns"));
	std::cout << sum->print() << std::endl;

	Function* simplified = sum->simplify();
	std::cout << simplified->print() << std::endl;
	delete sum;
	delete simplified;

	sum = new Sum(new Sum(new Constant("a"), new Sum(new Constant("a"), new Constant("a"))), new Constant("a"));
	std::cout << sum->print() << std::endl;
	simplified = sum->simplify();
	std::cout << simplified->print() << std::endl;
	delete sum;
	delete simplified;

	sum = new Sum(new Sum(new Variable("x", "global"), new Variable("x", "global")), new Variable("x", "global"));
	std::cout << sum->print() << std::endl;
	simplified = sum->simplify();
	std::cout << simplified->print() << std::endl;
	delete sum;
	delete simplified;

	sum = new Sum(new Multiplication(new Constant(1.0), new Constant("a")), new Multiplication(new Constant("b"), new Constant("a")));
	std::cout << sum->print() << std::endl;
	simplified = sum->simplify();
	std::cout << simplified->print() << std::endl;
	delete sum;
	delete simplified;

	sum = new Sum(
			new Sum(
				new Constant(1.0),
				new Sum(
					new Variable("x", "global"),
					new Constant("b")
					)
				),
			new Sum(
				new Sum(
					new Constant(2.0),
					new Variable("x", "global")
					),
				new Constant("b")
				)
			);
	std::cout << sum->print() << std::endl;
	simplified = sum->simplify();
	std::cout << simplified->print() << std::endl;
	delete sum;
	delete simplified;

	Multiplication* m = new Multiplication();
	m->setLhs(new Constant(2.0));
	m->setRhs(new Multiplication(new Constant("a"), new Constant("a")));
	std::cout << m->print() << std::endl;
	simplified = m->simplify();
	std::cout << simplified->print() << std::endl;
	delete m;
	delete simplified;
}
 
