#include <cctype>

#include "tools.h"

namespace Tools{
    __DZ_STRING Trim(__DZ_STRING str)
    {
        size_t i = 0,e = str.length();
        for(;i < e && std::isspace(str[i]);++i);
        size_t j = e - 1;
        for(;j > i && std::isspace(str[j]);--j);
        return (i <= j ? str.substr(i,j + 1 - i) : "");
    }

    bool ExtractArg(const char * argstr,const char * pattern,const char *& result)
    {
        if(!argstr || !pattern)
            return false;
        for(;*pattern;++pattern,++argstr)
            if(*pattern != *argstr)
                return false;
        result = *argstr ? argstr : 0;
        return true;
    }

    const char * ProgramName(const char * argstr)
    {
        const char * ret = argstr;
        for(const char * cur = argstr;cur && *cur;++cur)
            if(*cur == '/')
                ret = cur + 1;
        return ret;
    }

}
