#pragma once
#include <fstream>

#include "resource.hpp"

class ResourceFactory
{
public:
    virtual ~ResourceFactory() = default;
    virtual void serialize(const char* fileName, std::fstream& packageFile) = 0;
    virtual bool canSerialize(const char* type) = 0;
};
