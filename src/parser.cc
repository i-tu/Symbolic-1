#include <deque>
#include <stdexcept>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <vector>

#include "functions.hh"
#include "debug.hh"

#define OPERATORS "()+-*/!^=SD[]"
#define NUMBERS "1234567890."
#define OPERATORNUMBER "()+-*/!^=SD[]1234567890."
#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define STRICTVALID "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.()+-*/!^=,[]"
#define VALID "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.()+-*/!^=,[] "

extern std::map<std::string, Function*> functionList;
extern std::map<std::string, std::vector<std::string> > functionVariableList;

bool isOperator(const std::string &s){
    return s.find_first_not_of(OPERATORS) == std::string::npos;
}

bool isOpNum(const std::string &s){
    return s.find_first_not_of(OPERATORNUMBER) == std::string::npos;
}

bool isValid(const std::string &s){        
    return s.find_first_not_of(NUMBERS) == std::string::npos
        && s.find_first_not_of(OPERATORS) == std::string::npos;
}

bool isNumber(const std::string &s){
    return s.find_first_not_of(NUMBERS) == std::string::npos;
}

bool isClean(const std::string &s){
    return s.find_first_not_of(VALID) == std::string::npos;
}

bool isAlphabetic(const std::string &s){
    return s.find_first_not_of(ALPHABET) == std::string::npos;
}

int s2i(const std::string& s)
{
  std::istringstream i(s);
  int x;
  if (!(i >> x))
    return 0;
  return x;
}


// Parameter: Reference to deque which contains input to parse in prefix form
// Returns: head of resultant abstract syntax tree.
Function* parsePrefix(std::deque<std::string> &commands)
{
    // The next command must exist.
    if(commands.empty())
        throw std::logic_error("Incomplete input!");

    Function* next;
    
    if(commands.front() == "+")
    {
        commands.pop_front();
		Function *lhs = parsePrefix(commands);
		Function *rhs = parsePrefix(commands);
	    next = new Sum(lhs,rhs);
    }
    else if(commands.front() == "-")
    {
        commands.pop_front();
	    next = new Neg(parsePrefix(commands));
    }
    else if(commands.front() == "*")
    {
        commands.pop_front();
		Function *lhs = parsePrefix(commands);
		Function *rhs = parsePrefix(commands);
	    next = new Multiplication(lhs,rhs);
    }
    else if(commands.front() == "^")
    {
        commands.pop_front();
		Function *lhs = parsePrefix(commands);
		Function *rhs = parsePrefix(commands);
	    next = new Power(lhs,rhs);
    }    
	else if(commands.front() == "/")
	{
		commands.pop_front();
		Function *lhs = parsePrefix(commands);
		Function *rhs = parsePrefix(commands);
	    next = new Division(lhs,rhs);
    }    
    else if(commands.front() == "S")
    {
        commands.pop_front();
        next = parsePrefix(commands);
        Function* temp = next;
        next = next->simplifyTree();
        delete temp;
    }
    else if(commands.front() == "D")
    {
        commands.pop_front();
        next = parsePrefix(commands);
        Function* temp = next;
        next = next->derive("x");
        delete temp;
    }
    else if(functionList.find(commands.front()) != functionList.end())
    {
        next = functionList[commands.front()]->clone();
        commands.pop_front();
    }
    // Variable-calling function
    else if(commands.front() == "[")
    {
        commands.pop_front();
        std::string name = commands.front();
        commands.pop_front();
        
        if(functionList.find(name) != functionList.end())
        {
            if(functionVariableList.find(name) == functionVariableList.end())
                throw std::logic_error("Calling function without variables!");

            next = functionList[name]->clone();
            
            std::vector<std::string> varList = functionVariableList[name];
            std::map<Variable, Function*> parameters;
            
            for(std::vector<std::string>::iterator it = varList.begin();it != varList.end();it++)
            {
                Variable* nextVar = new Variable(*it, name);
                parameters[ *nextVar ] = parsePrefix(commands);
                delete nextVar;
                
                if(commands.front() == ",")
                    commands.pop_front();
            }
            Function* deleteMe = next;
            next = next->evaluate(parameters);
            delete deleteMe;
            
            for(std::map<Variable, Function*>::iterator it = parameters.begin(); it!=parameters.end();it++)
                delete (*it).second;
        
        }
        else
            throw std::logic_error("Mismatched parameter call!");
    }
    else if(isNumber(commands.front())){
        next = new Constant(s2i(commands.front()));
        commands.pop_front();
    }
    else
    {
        next = new Variable(commands.front(), "global");
        commands.pop_front();
    }
    
    return next;
}

// Returns precedence value of operator
int precedence(const std::string op)
{
    if(op == "[")
        return 8;
    else if(op == ",")
        return 7;
    else if (op == "S"||op == "D")
        return 6;
    else if (op == "-")
        return 5;
    else if(op == "^")
        return 4;
    else if(op == "*" || op == "/")
        return 3;
    else if (op == "+")
        return 2;
    else if (op == "=")
        return 1;
    return 0;
}

// Divides given string into a deque containing tokenized input.
// words like "applePie" and numbers like 123.456 are copied as is.
// operators like "/" or "(" are individual entries. Whitespace is eaten.
std::deque<std::string> split(const std::string &s)
{
    std::deque<std::string> elems;
    
    for (size_t i = 0;i < s.length(); )
    {
        // Eat whitespace. Yum!
        while(s.substr(i,1) == " ")
            i++;
            
        // No use for bracket close...
        if(s.substr(i,1) == "]")
        {
            i++;
            continue;
         }

        // If we find a number, keep eating that number until the end and add it.
        if(s.substr(i,1).find_first_of(NUMBERS) != std::string::npos )
        {
            std::string num = "";
            while(s.substr(i,1).find_first_of(NUMBERS) != std::string::npos )
                num.append(s.substr(i++,1));
                
            elems.push_back(num);
            continue;            
        }
        // If instead we find a name (alphabetic), keep eating that name until the end and add it.
        else if(s.substr(i,1).find_first_of(ALPHABET) != std::string::npos)
        {
            std::string alph = "";
            while(s.substr(i,1).find_first_of(ALPHABET) != std::string::npos)
                alph.append(s.substr(i++,1));
                
            elems.push_back(alph);
            continue;
        }
        
        // What comes next must be a symbol.
        elems.push_back(s.substr(i++,1));
    }
    
    return elems;
}

// Parses Infix notation into Prefix notation using
// Djikstra's shunting yard algorithm
// Parameter: Deque of tokenized input in infix form.
// On success given parameter contains result.
void infixToPrefix(std::deque<std::string> &infix)
{
    std::string next;
    std::deque<std::string> stack;
    std::deque<std::string> prefix;
    
    // While there are tokens to read
    while(!infix.empty())
    {
        // Read next token
        next = infix.back();
        infix.pop_back();
        
        // If next token is right-hand parentheses, push it to the stack
        if(next == ")")
            stack.push_back(next);
        
        // If next token is left-hand parentheses,
        else if(next == "(")
        {
            if(stack.empty())
                throw std::logic_error("Mismatched parantheses.");
        
            // push from stack to prefix stack until right-hand parentheses is found
            while(stack.back() != ")")
            {
                prefix.push_back(stack.back());
                stack.pop_back();
                if(stack.empty())
                    throw std::logic_error("Mismatched parantheses.");
            }
            
            // Remove ")" at end
            if(!stack.empty())
                stack.pop_back();
            else throw std::logic_error("Mismatched parantheses.");
        }
        
        // If operator is met,
        else if(isOperator(next))
        {
            if(!stack.empty())
            {
                while(precedence(stack.back()) > precedence(next))
                {
                    prefix.push_back(stack.back());
                    stack.pop_back();
                    if(stack.empty())
                        break;
                }
            }
            stack.push_back(next);
        }
        
        // If something else, like a name is found
        else
            prefix.push_back(next);
    }
    
    // Empty rest of stack
    while(!stack.empty())
    {
        if(stack.back() == "(" || stack.back() == ")")
            throw std::logic_error("Mismatched parantheses.");

        prefix.push_back(stack.back());
        stack.pop_back();
    }

    // Flip commands from "prefix" into the final destination, the parameter deque.
    while(!prefix.empty())
    {
        infix.push_front(prefix.front());
        prefix.pop_front();
    }
    
}
