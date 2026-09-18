#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Channel; // Forward declaration pour éviter les includes circulaires

class Client {
private:
    int         _fd;
    std::string _ipAddress;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    bool        _isRegistered;
    bool        _hasAuthenticated;
    
    std::string _inBuffer;
    std::string _outBuffer;

    std::vector<Channel*> _channels;

public:
    Client(int fd, const std::string& ip);
    ~Client();

    int         getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    bool        isRegistered() const;
    bool        hasAuthenticated() const;
    std::string& getInBuffer();
    std::string& getOutBuffer();

    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setRegistered(bool status);
    void setAuthenticated(bool status);

    void addChannel(Channel* channel);
    void removeChannel(Channel* channel);
};

#endif
