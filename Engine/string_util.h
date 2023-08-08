﻿#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

// todo: put in its own file, clean up

template<typename ... Args>
std::string stringFormat( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    // if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string{ buf.get(), buf.get() + size - 1 }; // We don't want the '\0' inside
}

inline std::string parseBetweenTags(const std::string& input, const std::string& startTag, const std::string& endTag)
{
    size_t start = input.find(startTag) + startTag.length();
    size_t end = input.find(endTag, start);
    return input.substr(start, end - start);
}

inline std::string readFromFile(const char* filePath)
{
    // todo: own custom platform layer for getting data from a file system?
    std::ifstream fileStream{};
    std::stringstream stringStream{};

    // Read the raw text data from a file on the disk.
    fileStream.open(filePath);

    if (!fileStream.is_open())
        printf("Unable to open the file %s to load shader.", filePath);

    stringStream << fileStream.rdbuf();
    fileStream.close();

    return stringStream.str();
}

#endif // STRING_UTIL_H
