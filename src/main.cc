#include <deque>
#include <iostream>
#include <list>
#include <stdexcept>
#include <exception>
#include <string>
#include <iterator>
#include <map>

#include "common.hh"
#include "functions.hh"
#include "parser.hh"
#include "debug.hh"

int main (void)
{
    // Initialization
    std::string input = "";
    bool infix = true;
    bool simple = true;
    
    std::string nextName;
    std::cout << std::endl
    << "Welcome to Symbolic-1!" << std::endl
    << "Type \"help\" for help." << std::endl
    << std::endl;
    
    // Control Loop
    while(1)
    { try {
    	nextName = "";
    	input = "";
    	bool variableFunction = false;
        std::cout << ">> ";
        std::getline(std::cin, input, '\n');
        
        if(!isClean(input))
        {
            throw(std::logic_error("Invalid characters in input."));
        }
        else if(input == "exit"||input == "bye")
        {
            std::cout << "Bye!" << std::endl;
            break;
        }
        else if(input == "infix")
        {
            infix = true;
            std::cout << "Now Accepting only infix (traditional) input." << std::endl;
        }
        else if (input == "prefix")
        {
            infix = false;
            std::cout << "Now accepting only prefix (polish) input." << std::endl;
        }
        else if(input == "simple")
        {
            simple = simple ? false : true;
            std::cout << "Automatic simplifying now set to " << simple << std::endl;
        }
        else if (input =="who")
        {
            if(!functionList.empty())
            {
                std::cout << "Defined functions:" << std::endl;
                for(std::map<std::string, Function*>::iterator it = functionList.begin();it!=functionList.end();it++)
                {
                    std::cout << it->first;
                    if(functionVariableList.find(it->first) != functionVariableList.end())
                    {
                        std::cout << "[";
                        std::vector<std::string> vars(functionVariableList[it->first].begin(),functionVariableList[it->first].end());
                        std::vector<std::string>::iterator it_vars;
                        for(it_vars = vars.begin();it_vars!=vars.end();it_vars++)
                        {
                            std::cout << *it_vars;
                            if(it_vars+1 != vars.end())
                                std::cout << ",";
                        }
                        std::cout << "]";
                    }                    
                    std::cout << " = " << it->second->print() << std::endl;
                }
            }
            else
                std::cout << "No functions." << std::endl;
        }
        else if (input == "clear")
        {
            for(std::map<std::string, Function*>::iterator it = functionList.begin(); it!=functionList.end();it++)
                delete (*it).second;
            functionList.clear();
            functionVariableList.clear();
        }
        else if (input == "help")
        {
            std::cout << std::endl
            << "Help - a short guide to Symbolic-1" << std::endl
            << "----------------------------------" << std::endl
            << std::endl
            << "Numeric calculation is straightforward" << std::endl
            << std::endl
            << "    >> 1+1               -> 2" << std::endl
            << "    >> 2^3+4*5           -> 28" << std::endl
            << std::endl
            << " The minus operator is unary. ie. 1 - 3 -> 1 + -3" << std::endl
            << "Formulae are automatically simplified by default" << std::endl
            << std::endl 
            << "    >> x*2+x*2           -> x * 4" << std::endl
            << "    >> x*x^2             -> x^3" << std::endl
            << std::endl
            << "Results are assigned to 'ans' by default. To assign" << std::endl
            << "to some other name, first type the name, then the" << std::endl
            << "variables in square brackets separated by comma" << std::endl
            << "followed by an '=' sign and function definition." << std::endl
            << "If no variable list is provided, the variables in" << std::endl
            << "are assumed to be called in alphabetic order." << std::endl
            << std::endl
            << "    >> f[x,y]=x+y^2      (same as f=x+y^2)" << std::endl
            << std::endl
            << "To call a function, simply type its name and" << std::endl
            << "variables you are calling it with" << std::endl
            << std::endl
            << "    >> f                 -> (x+y^2)" << std::endl
            << "    >> f[2,3]            -> (2+3^2) -> 11" << std::endl
            << std::endl
            << "Calling any defined variable will create a copy of" << std::endl
            << "it and assign it." << std::endl
            << std::endl
            << "D derives by x. Call your functions accordingly." << std::endl
            << std::endl
            << "    D(x^3)               -> 3*x^2" << std::endl
            << "    D(f[2,x])            -> D(2+x^2) -> 2*x" << std::endl
            << std::endl
            << "Note that you can pretty much assign anything into" << std::endl
            << "any function." << std::endl
            << std::endl
            << "    >> f[x,y] = x^2+y^3+x" << std::endl
            << "    >> h[m]=f[m^2,m^3+2] -> ((8+m+m^2)^m)^3+2" << std::endl
            << "    >> aleph = h[3]      -> ((8+3+3^2)^3)^3+2 -> 5.12e+11" << std::endl
            << "    >> aleph             -> 5.12e+11" << std::endl
            << std::endl
            << "We hope you enjoy using Symbolic-1!" << std::endl
            << std::endl;
        }
        else if (input == "commands")
        {
            std::cout
            << std::endl
            << "Commands" << std::endl
            << "--------" << std::endl
            << std::endl
            << "    who           Displays list of defined functions and variables" << std::endl
            << "    clear         Clears defined functions and variables" << std::endl
            << "    simple        Toggles autosimplify (default: on)" << std::endl
            << "    prefix        Turns prefix mode on" << std::endl
            << "    infix         Turns infix mode on (default)" << std::endl
            << "    help          Displays help" << std::endl
            << "    commands      Displays this list" << std::endl
            << "    exit/bye      Ends program" << std::endl
            << std::endl;
        }
        else if (functionList.find(input) != functionList.end())
            std::cout << functionList[input]->print() << std::endl;
        else
        {
            // Input is clean and presumably of right form, like f[x,y]=x+y or g[a,b]=h[a^2+b,b^2+a]-z[a^2]
            std::deque<std::string> splitInput = split(input);
            
            if(splitInput.size() == 1)
            {
                std::cout << splitInput.front() << " is not a recognized command" << std::endl;
                continue;
            }

            // Lets find if there's an "=" sign...
            std::deque<std::string>::iterator equals;
            for(equals = splitInput.begin();equals != splitInput.end();equals++)
                if(*equals == "=") break;

            if(equals != splitInput.end())
            {
                std::deque<std::string> leftSide(splitInput.begin(), equals);
                splitInput = std::deque<std::string>(equals + 1, splitInput.end());
                
                if(leftSide.size() == 0)
                    throw std::logic_error("lvalue required");
                else if (!isAlphabetic(leftSide.front()))
                    throw std::logic_error("Invalid left-hand input! (function name must be alphabetic)");
                
                nextName = leftSide.front();
                leftSide.pop_front();
                
                if(!leftSide.empty())
                {
                    std::vector<std::string> variables;
                    
                    if(leftSide.front() == "[")
                    {
                        leftSide.pop_front();
                        while(!leftSide.empty())
                        {
                            if(isNumber(leftSide.front()))
                                throw std::logic_error("function vars in definition may not be numeric!");
                            if(isOperator(leftSide.front()))
                                throw std::logic_error("function vars in definition may not be operators!");
                            if(functionList.find(leftSide.front()) != functionList.end())
                                throw std::logic_error("symbol '" + leftSide.front() + "' is already in use!");
                            if(isAlphabetic(leftSide.front()))
                                variables.push_back(leftSide.front());
                            leftSide.pop_front();
                        }
                        
                        std::list<std::string> leftVars  = std::list<std::string>(variables.begin(),variables.end());
                        std::list<std::string> rightVars = std::list<std::string>(splitInput.begin(),splitInput.end());
                        leftVars.sort();
                        rightVars.remove(",");
                        rightVars.remove_if(isOpNum);
                        rightVars.sort();
                        rightVars.unique();
                        
                        for(std::list<std::string>::iterator it = rightVars.begin();it!=rightVars.end();it++)
                        {
                            if(functionList.find(*it) != functionList.end())
                            {
                                rightVars.erase(it);
//                                rightVars.insert(rightVars.end(), functionVariableList[*it].begin(), functionVariableList[*it].end());
                                it = rightVars.begin();
                            }
                        }
                        
                        rightVars.sort();
                        rightVars.unique();

                        if(leftVars.size() != rightVars.size())
                            throw std::logic_error("Amount of variables on both sides of '=' needs to be the same.");
                        
                        while(!leftVars.empty())
                        {
                            if(leftVars.front() != rightVars.front())
                                throw std::logic_error("You need to mutually define and use all variables!");
                            leftVars.pop_front(); rightVars.pop_front();
                        }
                        functionVariableList[nextName] = variables;
                    }
                }
                else
                {
                    std::list<std::string> rightVars = std::list<std::string>(splitInput.begin(), splitInput.end());
                    rightVars.remove_if(isOpNum);
                    rightVars.sort();
                    rightVars.unique();
                    
                    for(std::list<std::string>::iterator it = rightVars.begin();it!=rightVars.end();it++)
                    {
                        if(functionList.find(*it) != functionList.end())
                        {
                            rightVars.erase(it);
                            rightVars.insert(rightVars.end(), functionVariableList[*it].begin(), functionVariableList[*it].end());
                            it = rightVars.begin();
                        }
                    }
                    rightVars.sort();
                    rightVars.unique();
                    
                    functionVariableList[nextName] = std::vector<std::string>(rightVars.begin(), rightVars.end());
                }
           }
           else
	           nextName = "ans";

            if(infix)
                infixToPrefix(splitInput);

            // If there is already a function of the same name, delete it.
            if(functionList.find(nextName) != functionList.end())
            {
                delete functionList[nextName];
                functionList.erase(nextName);
            }

            // This is where the magic happens
            Function* next = parsePrefix(splitInput);
            
            if(!splitInput.empty())
                throw std::logic_error("Incomplete input!");
                
            if(simple)
            {
                Function* deleteMe = next;
                next = next->simplifyTree();
                delete deleteMe;
            }
            functionList[nextName] = next;
            std::cout << functionList[nextName]->print() << std::endl;
        }
    }
    catch (std::exception &e) 
    {
        std::cout << "Error: " << e.what() << std::endl;
        functionVariableList.erase(nextName);
        functionList.erase(nextName);
    }
    } // End Control Loop

    // Clean up
    for(std::map<std::string, Function*>::iterator it = functionList.begin(); it!=functionList.end();it++)
        delete (*it).second;
    
    functionList.clear();
    functionVariableList.clear();
    
    return 0;
} 
