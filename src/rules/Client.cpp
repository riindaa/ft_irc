#include "Client.hpp"
#include "Channel.hpp"
#include <unistd.h>

Client::Client(int fd) : _fd(fd), _isRegistered(false), _hasPassword(false)
{
}

Client::~Client()
{
    closeClient();
}

int Client::getFd() const
{
    return _fd;
}

const std::string Client::getPrefix() const
{
    return ":" + getNickname() + "!" + getUsername() + "@localhost";
}

const std::string& Client::getNickname() const
{
    return _nickname;
}

const std::string& Client::getUsername() const
{
    return _username;
}

const std::string& Client::getRealname() const
{
    return _realname;
}

const std::string& Client::getInBuff() const
{
    return _inBuff;
}

const std::string& Client::getOutBuff() const
{
    return _outBuff;
}

const std::set<Channel*>& Client::getChannels() const
{
    return _channels;
}

bool Client::isRegistered() const
{
    return _isRegistered;
}

bool Client::hasPass() const
{
    return _hasPassword;
}

void Client::setNickname(const std::string& nick)
{
    _nickname = nick;
}

void Client::setUsername(const std::string& user)
{
    _username = user;
}

void Client::setRealname(const std::string& real)
{
    _realname = real;
}

void Client::setHasPass(bool hasPass)
{
    _hasPassword = hasPass;
}

void Client::setIsRegistered(bool registered)
{
    _isRegistered = registered;
}

void Client::addChannel(Channel* channel)
{
    if (channel)
        _channels.insert(channel);
}

void Client::removeChannel(Channel* channel)
{
    if (channel)
        _channels.erase(channel);
}

void Client::appendInBuff(const std::string& str)
{
    _inBuff += str;
}

void Client::appendOutBuff(const std::string& str)
{
    _outBuff += str;
}

void Client::clearInBuff()
{
    _inBuff.clear();
}

void Client::closeClient()
{
    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

bool Client::extractNextCommand(std::string &command_line)
{
    size_t pos = _inBuff.find("\r\n");
    size_t delimiter_len = 2;

    if (pos == std::string::npos)
    {
        pos = _inBuff.find("\n");
        delimiter_len = 1;
    }

    if (pos == std::string::npos)
        return false;

    command_line = _inBuff.substr(0, pos);
    _inBuff.erase(0, pos + delimiter_len);
    return true;
}
