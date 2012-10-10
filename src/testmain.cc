#include <deque>
#include <list>
#include <iostream>
#include <stdexcept>
#include <exception>
#include <string>
#include <iterator>
#include <map>

#include "laskin.hh"
#include "parser.hh"
#include "function.hh"

int main (void)
{
    // Initialization
    std::string input = "";
    bool infix = true;
    std::map<std::string, Function**> functions;
    std::string nextName;
    
    // Control Loop
    while(1)
    { try {
        std::cout << ">> ";
        std::getline(std::cin, input, '\n');
        
        if(!isClean(input))
        {
            throw(std::logic_error("Invalid characters in input."));
        }
        else if(input == "exit")
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
        else if (input == "who")
        {
            if(!functions.empty())
            {
                std::cout << "Defined functions:" << std::endl;
                for(std::map<std::string, Function**>::iterator it = functions.begin();it!=functions.end();it++)
                    std::cout << it->first << std::endl;
            }
            else
                std::cout << "No functions." << std::endl;
        }
        else if (input == "clear")
        {
            delete &functions;
        }
        // Other special commands go here!
        else if (input == "help")
        {
        }
        else
        {
            // Input is clean and presumably of the form functionName(variables) = function, for example f=x+y
            std::deque<std::string> splitinput = split(input);

            #ifdef DEBUG
                std::cout << "Tokenized input: ";
                for(std::deque<std::string>::iterator it = splitinput.begin();it!=splitinput.end();it++)
                    std::cout <<"["<<*it<<"] ";
                std::cout << std::endl;
            #endif

            // Lets find if there's an "=" sign...
            std::deque<std::string>::iterator equals;
            for(equals = splitinput.begin();equals != splitinput.end();equals++)
                if(*equals == "=") break;

            if(equals != splitinput.end())
            {
                std::deque<std::string> leftSide(splitinput.begin(), equals);

                if(leftSide.size() > 1)
                    throw std::logic_error("Invalid left-hand input! (more than one word)");
                else if (!isAlphabetic(leftSide.front()))
                    throw std::logic_error("Invalid left-hand input! (function name must be alphabetic)");
                    
                nextName = leftSide.front();
                splitinput.pop_front();splitinput.pop_front();
            }
            else
                nextName = "ans";

            if(infix)
                infixToPrefix(splitinput);

            if(functions.find(nextName) != functions.end())
                functions.erase(nextName);
                
            // This is where the magic happens
            Function* next = parsePrefix(splitinput);
            functions[nextName] = &next;

            if(!splitinput.empty())
                throw std::logic_error("Incomplete input!");

            std::cout << (*functions[nextName])->print() << std::endl;
        }
        input = "";

    }
    catch (std::exception &e) { std::cout << "Error: " << e.what() << std::endl; }
    } // End Control Loop

    // Clean up
    delete &functions;
    
    return 0;
}
 
