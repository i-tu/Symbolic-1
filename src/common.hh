#ifndef SYMBOLIC_1_COMMON_HH
#define SYMBOLIC_1_COMMON_HH
    #include <string>
    #include <map>
    #include <vector>
    
    #include "functions.hh"
    std::map<std::string, Function*> functionList;
    std::map<std::string, std::vector<std::string> > functionVariableList;
#endif 
