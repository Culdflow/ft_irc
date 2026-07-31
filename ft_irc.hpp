#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <vector>

#include "Client.hpp"

struct Message {
    std::string prefix;             
    std::string command;           
    std::vector<std::string> params; 	
};

// parsing
void parseArguments(int ac, char **av);
std::vector<Message> socketBufferParsing(client& Client, bool &closed);
Message parseCommand(std::string& raw);

#endif