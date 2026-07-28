#include "Server.hpp"
#include "Channels.hpp"

void serv::cmdJOIN(client& cl, Message msg)
{
    if(msg.params.size() < 1)
    {
        sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        return ;
    }
    if(msg.params[0][0] != '#')
    {
        sendReply(cl, "476 ERR_BADCHANMASK");
        return ;
    }
    std::map<std::string, Channel>::iterator it;
    it = _channelList.find("#42");
    if(it != _channelList.end())
    {
        it->second.add_user(cl);
    }
    else
    {
        std::string name = msg.params[0].substr(1);
        Channel channel(name);
        channel.add_user(cl);
        channel.add_operator(cl);
        _channelList[name] = Channel(name);
    }
}