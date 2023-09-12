#include <stack>
#include "logger.hpp"

using namespace Logger;

Severity Logger::g_severityMask {Severity::All};
Channel Logger::g_channelMask {Channel::All};

bool Logger::shouldShow(Channel channel)
{
    return (g_channelMask & channel) != Channel::None;
}

bool Logger::shouldShow(Severity severity)
{
    return (g_severityMask & severity) != Severity::None;
}

// Default Channel Stack
static std::stack<Channel> s_channelStack{};

void Logger::pushDefaultChannel(Channel channel)
{
    s_channelStack.push(channel);
}

void Logger::popDefaultChannel()
{
    s_channelStack.pop();
}

Channel Logger::getDefaultChannel()
{
    return s_channelStack.empty() ? Channel::General : s_channelStack.top();
}

DefaultLogChannelJanitor::DefaultLogChannelJanitor(Channel channel)
{
    pushDefaultChannel(channel);
}

DefaultLogChannelJanitor::~DefaultLogChannelJanitor()
{
    popDefaultChannel();
}
