/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/10 11:01:34 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/20 17:16:16 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

//parsing <port> <password> 
    // <port> 
        // nb entier entre 1 et 65535
        // est ce qu'on accepte n'importe quel port ?

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

      
// reconstituer la commande a partir de ce qui est recu
// manque : le client (sous quelle forme ? Je mets une struct en attendant mais surement une classe), son socket et son buffer d'input

void socketBufferParsing(client& Client) {
    char buf[512];
    size_t n = recv(Client.getSocketFd(), buf, sizeof(buf), 0);

    if (n <= 0) {
        // pas de message envoyé, quelle gestion ? 
        // dans le parsing je dirais juste return car normalement utilise avec poll()/POLLIN, on envoie au parsing que quand y a bien un message recu
        return;
    }
    Client.getInputBuf().append(buf, n);
    size_t pos;
    while ((pos = Client.getInputBuf().find('\n')) != std::string::npos) {
        std::string cmd = Client.getInputBuf().substr(0, pos);
        if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
            cmd.erase(cmd.size() - 1);
        Client.getInputBuf().erase(0, pos + 1);
        if (!cmd.empty())
            Message msg = parseCommand(cmd);
        //en realite ca sera plutot surement dans une classe ? mais je mets ca en attendant pour les tests
    }
}

// [':' prefix - optionnel SPACE] command [SPACE params] [SPACE ':' trailing]
// return un msg vide en cas d'erreur
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
        // mettre un message d'erreur ? Signifie que la commande a un prefix puis rien apres, pas un code d'erreur classique dans IRC
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

void test_parsing(std::string& s) {
    std::cout << "je suis rentre : string = " << s << "\n";
    size_t pos;
    while ((pos = s.find('\n')) != std::string::npos) {
        std::string cmd = s.substr(0, pos);
        if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
            cmd.erase(cmd.size() - 1);
        s.erase(0, pos + 1);
        if (!cmd.empty()) {
            Message msg = parseCommand(cmd);
            
            if (msg.command.empty())
                std::cout << "command empty\n";
            else if (msg.params.empty())
                std::cout << "param empty\n";
            else {
                if (!msg.prefix.empty())
                    std::cout << "prefix: " << msg.prefix << "\n";
                std::cout << "command: " << msg.command << "\n";
                for (size_t i = 0; i < msg.params.size(); i++)
                    std::cout << "params[" << i << "]: " << msg.params[i] << "\n"; 
            }
        }
    }
}
