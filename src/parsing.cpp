
#include "../include/ft_irc.hpp"

static bool parsePort(const std::string& argPort) {
    if (argPort.empty())
        return false;
    char *end;
    long port = strtol(argPort.c_str(), &end, 10);
    if (*end != '\0' || (port <= 0 || port > 65535))
        return false;
    return true;
}

static bool parsePassword(const std::string& argPass) {
    if (argPass.empty())
        return false;
    for (size_t i = 0; i < argPass.size(); i++) {
        if (isspace(argPass[i]))
            return false;
    }
    return true;
}

void parseArguments(int ac, char **av) {
    if (ac != 3)
        throw std::invalid_argument("Usage: ./ircserv <port> <password>");
    std::string argPort = av[1];
    std::string argPass = av[2];
    
    if (!parsePort(argPort))
        throw std::invalid_argument("Error: invalid port");
    if (!parsePassword(argPass))
        throw std::invalid_argument("Error: invalid password");
}

      
std::vector<Message> socketBufferParsing(client& Client, bool& closed) {
    char buf[512];
    int n = recv(Client.getSocketFd(), buf, sizeof(buf), 0);
    std::vector<Message> vecMsgs;

    if (n <= 0)
    {
        closed = true;
        return vecMsgs;
    }
    Client.getInputBuf().append(buf, n);
    size_t pos;
    while ((pos = Client.getInputBuf().find('\n')) != std::string::npos) {
        std::string cmd = Client.getInputBuf().substr(0, pos);
        if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
            cmd.erase(cmd.size() - 1);
        Client.getInputBuf().erase(0, pos + 1);
        if (!cmd.empty()) {
            Message msg = parseCommand(cmd);
            vecMsgs.push_back(msg);
        }
    }
    return vecMsgs;
}

Message parseCommand(std::string& raw) {
    Message msg;
    std::string line = raw;
    size_t pos = 0;

    msg.command = "";
    msg.prefix = "";

    if (line[0] == ':') {
        size_t end = line.find(' ');
        if (end == 1 || end == std::string::npos)
            return msg;
        msg.prefix = line.substr(1, end - 1);
        pos = end + 1;        
    }
    
    while (pos < line.size() && line[pos] == ' ')
        pos++;
    
    size_t cmdEnd = line.find(' ', pos);
    msg.command = line.substr(pos, cmdEnd - pos);
    for (size_t i = 0; i < msg.command.size(); i++)
        msg.command[i] = toupper(msg.command[i]);
    if (cmdEnd == std::string::npos)
        pos = line.size();
    else
        pos = cmdEnd;
    
    while (pos < line.size()) {
        while (pos < line.size() && line[pos] == ' ')
            pos++;
        if (pos == line.size())
            break;
        if (line[pos] == ':') {
            msg.params.push_back(line.substr(pos + 1));
            break;
        }
        size_t spaceEnd = line.find(' ', pos);
        if (spaceEnd == std::string::npos) {
            msg.params.push_back(line.substr(pos));
            break;            
        }
        else {
            msg.params.push_back(line.substr(pos, spaceEnd - pos));
            pos = spaceEnd;
        }
    }
    return msg;
}