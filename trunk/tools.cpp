#include <cctype>

#include "tools.h"

namespace Tools{
    std::string Trim(std::string str)
    {
        size_t i = 0,e = str.length();
        for(;i < e && std::isspace(str[i]);++i);
        size_t j = e - 1;
        for(;j > i && std::isspace(str[j]);--j);
        return (i <= j ? str.substr(i,j + 1 - i) : "");
    }
}
