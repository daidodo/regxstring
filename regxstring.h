#ifndef DOZERG_REGXSTRING_H_20091012
#define DOZERG_REGXSTRING_H_20091012

#include <vector>
#include <string>

#include "alloc.h"

struct __NodeBase;

class CRegxString
{
    typedef __DZ_VECTOR(char) __Ends;
    typedef std::pair<__NodeBase *,int> __Ret;
    __DZ_STRING regx_;
    __DZ_STRING str_;
    //regx tree
    __NodeBase * top_;
    //parse data
    __Ends ends_;
    size_t i_;
    int ref_;
public:
    CRegxString();
    explicit CRegxString(const __DZ_STRING & regx);
    ~CRegxString(){uninit();}
    void ParseRegx(const __DZ_STRING & regx);
    __DZ_STRING Regx() const{return regx_;}
    const char * RandString();
    const __DZ_STRING & LastString() const{return str_;}
    void Debug(std::ostream & out) const;
private:
    void uninit();
    int inEnds(int ch) const;
    __Ret processSeq();
    __Ret processSlash(bool bNode);
    __NodeBase * processSet();
    __NodeBase * processGroup();
    __Ret processSelect(__NodeBase * node);
    __NodeBase * processRepeat(__NodeBase * node);
    int processInt(int & result);
    bool processRange(int & result);
    int ignoreSubexpMarks();
};

#endif
