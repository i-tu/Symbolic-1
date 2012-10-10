#include "treeFunctions.hh"

Function* rotateTreeRight(BinaryOperation* root)
{
	BinaryOperation* pivot = dynamic_cast<BinaryOperation*>(root->getLhs());
	if (pivot == 0)
		return root;
	root->setLhs(pivot->getRhs());
	pivot->setRhs(root);
	return pivot;
}

Function* rotateTreeLeft(BinaryOperation* root)
{
	BinaryOperation* pivot = dynamic_cast<BinaryOperation*>(root->getRhs());
	if (pivot == 0)
		return root;
	root->setRhs(pivot->getLhs());
	pivot->setLhs(root);
	return pivot;
}
 
