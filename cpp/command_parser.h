#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "common.h"

struct ParsedCommand {
    string raw;                 
    vector<string> tokens;     
    ContainerType type;         
    string command;    
};

ParsedCommand parseCommand(const string& query);

ContainerType containerTypeFromCommand(const string& command);

vector<string> splitTokens(const string& s);

#endif