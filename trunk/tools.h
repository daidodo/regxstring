#ifndef DOZERG_TOOLS_H_20091110
#define DOZERG_TOOLS_H_20091110

#include <string>

#include "alloc.h"

namespace Tools{

    inline bool IsRepeat(int ch){
        return ch == '?' || ch == '+' || ch == '*';
    }

    inline bool IsBegin(int ch){
        return ch == '^';
    }

    inline bool IsEnd(int ch){
        return ch == '$';
    }

    inline bool IsSlash(int ch){
        return ch == '\\';
    }

    inline bool IsSetBegin(int ch){
        return ch == '[';
    }

    inline bool IsSetEnd(int ch){
        return ch == ']';
    }

    inline bool IsGroupBegin(int ch){
        return ch == '(';
    }

    inline bool IsGroupEnd(int ch){
        return ch == ')';
    }

    inline bool IsSelect(int ch){
        return ch == '|';
    }

    inline bool IsRepeatBegin(int ch){
        return ch == '{';
    }

    inline bool IsRepeatEnd(int ch){
        return ch == '}';
    }

    inline bool NeedEnd(int ch){
        return IsGroupEnd(ch) || IsRepeatEnd(ch);
    }

    inline bool IsDigit(int ch){
        return '0' <= ch && ch <= '9';
    }

    inline int TransDigit(int ch){
        return ch - '0';
    }

    inline bool IsDash(int ch){
        return ch == '-';
    }

    inline bool IsAny(int ch){
        return ch == '.';
    }

    inline int IsSubexpMark(int ch){
        return (ch == ':' || ch == '=' || ch == '!' || ch == '>' ? ch : 0);
    }

    inline int IsSubexpMark(const char * s){
        return (*s == '?' ? IsSubexpMark(*(s + 1)) : 0);
    }

    inline char TransSlash(int ch){
        switch(ch){
            case 'f':return '\f';
            case 'n':return '\n';
            case 'r':return '\r';
            case 't':return '\t';
            case 'v':return '\v';
        }
        return ch;
    }

    __DZ_STRING Trim(__DZ_STRING str);

    bool ExtractArg(const char * argstr,const char * pattern,const char *& result);

    const char * ProgramName(const char * argstr);

}

#endif
