#include "command_parser.h"

vector<string> splitTokens(const string& s) {
    vector<string> result;
    istringstream iss(s);
    string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

ContainerType containerTypeFromCommand(const string& command) {
    if (command.empty()) return CT_UNKNOWN;
    switch (command[0]) {
        case 'M': return CT_ARRAY;
        case 'F': return CT_SINGLY;
        case 'L': return CT_DOUBLY;
        case 'S': return CT_STACK;
        case 'Q': return CT_QUEUE;
        case 'T': return CT_TREE;
        default: return CT_UNKNOWN;
    }
}

ParsedCommand parseCommand(const string& query) {
    ParsedCommand pc;
    pc.raw = query;
    pc.tokens = splitTokens(query);
    pc.command = pc.tokens.empty() ? "" : pc.tokens[0];
    pc.type = containerTypeFromCommand(pc.command);
    return pc;
}