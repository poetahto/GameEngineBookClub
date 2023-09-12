#ifndef LOGGER_HPP
#define LOGGER_HPP

// Global logging utilities.
// TODO: serialize logging info, like the masks, and write logs to a file? Also, provide dear ImGui integration for managing the masks, and viewing logs in-game
// todo: i dont like having to leak all these headers in something as global as logging...
#include <unordered_map>
#include <iostream>
#include <format>
#include "enum_flags.hpp"

namespace Logger
{
    // What the subject of this log message relates to. Messages can be filtered by this state.
    ENUM_FLAGS(Channel)
    enum class Channel
    {
        None = 0,
        All = INT_MAX,
        General = 1 << 0,
        Rendering = 1 << 1,
    };

    // The importance of this log message's visibility. Messages can be filtered by this state.
    ENUM_FLAGS(Severity)
    enum class Severity
    {
        None = 0,
        All = INT_MAX,
        Error = 1 << 0,
        Warning = 1 << 1,
        Info = 1 << 2,
    };

    // A printf style format string.
    typedef std::string_view FormatString;

    // A bitmask of all severities that are allowed to be logged.
    extern Severity g_severityMask;

    // A bitmask of all channels that are allowed to be logged.
    extern Channel g_channelMask;

    // Should a message with the given severity be shown?
    bool shouldShow(Severity severity);

    // Should a message with the given channel be shown?
    bool shouldShow(Channel channel);

    // Uses RAII to automatically push-and-pop a default channel within its scope.
    struct DefaultLogChannelJanitor
    {
        explicit DefaultLogChannelJanitor(Channel channel);
        ~DefaultLogChannelJanitor();
    };

    // Pushes a new default logging channel for all following logs.
    void pushDefaultChannel(Channel channel);

    // Removes the current default logging channel. If no channels are pushed, the true default is "General".
    void popDefaultChannel();

    // Gets the current default logging channel. Logs without a specified channel will use this one.
    Channel getDefaultChannel();

    // Core logging logic (everything else calls into this).
    template<typename... Args>
    void log(Channel channel, Severity severity, FormatString format, Args&&... args)
    {
        static std::unordered_map<Severity, const char*> severityNameLookup {
                {
                        {Severity::Error, "Error"},
                        {Severity::Warning, "Warning"},
                        {Severity::Info, "Info"},
                }
        };

        static std::unordered_map<Channel, const char*> channelNameLookup {
                {
                        {Channel::General, "General"},
                        {Channel::Rendering, "Rendering"},
                }
        };

        if (shouldShow(channel) && shouldShow(severity))
        {
            const char* severityName {severityNameLookup[severity]};
            const char* channelName {channelNameLookup[channel]};
            std::cout << std::format("[{}:{}] ", severityName, channelName) << std::vformat(format, std::make_format_args(args...)) << std::endl;
        }
    }

    template<typename... Args>
    void log_info(Channel channel, FormatString format, Args&&... args)
    {
        log(channel, Severity::Info, format, args...);
    }

    template<typename... Args>
    void log_warning(Channel channel, FormatString format, Args&&... args)
    {
        log(channel, Severity::Warning, format, args...);
    }

    template<typename... Args>
    void log_error(Channel channel, FormatString format, Args&&... args)
    {
        log(channel, Severity::Error, format, args...);
    }

    template<typename... Args>
    void log(Severity severity, FormatString format, Args&&... args)
    {
        log(getDefaultChannel(), severity, format, args...);
    }

    template<typename... Args>
    void log_info(FormatString format, Args&&... args)
    {
        log(getDefaultChannel(), Severity::Info, format, args...);
    }

    template<typename... Args>
    void log_warning(FormatString format, Args&&... args)
    {
        log(getDefaultChannel(), Severity::Warning, format, args...);
    }

    template<typename... Args>
    void log_error(FormatString format, Args&&... args)
    {
        log(getDefaultChannel(), Severity::Error, format, args...);
    }

    template<typename... Args>
    void log(FormatString format, Args&&... args)
    {
        log(getDefaultChannel(), Severity::Info, format, args...);
    }
} // namespace Logger

#endif // LOGGER_HPP
