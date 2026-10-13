#ifndef BOT_HPP
#define BOT_HPP

#include <string>

class Channel;

class Bot
{

  private:
    std::string _nickname;

  public:
    Bot();
    ~Bot();

    bool isNickname(const std::string& nickname) const;
    std::string handleMessage(const std::string& message, const std::string& nickname,
                              Channel* channel) const;
    const std::string getPrefix() const;
};

#endif
