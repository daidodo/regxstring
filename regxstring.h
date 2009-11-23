#ifndef DOZERG_REGXSTRING_H_20091012
#define DOZERG_REGXSTRING_H_20091012

#include <vector>
#include <string>

#include "alloc.h"

struct __NodeBase;

typedef __DZ_VECTOR(char) __Ends;

class CRegxString
{
    typedef std::pair<__NodeBase *,int> __Ret;
    struct __ParseData{
        __Ends ends_;
        size_t i_;
        int ref_;
        __ParseData():i_(0),ref_(0){}
    };
    __DZ_STRING regx_;
    __DZ_STRING str_;
    //regx tree
    __NodeBase * top_;
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
    __Ret processSeq(__ParseData & pdata);
    __Ret processSlash(bool bNode,__ParseData & pdata);
    __NodeBase * processSet(__ParseData & pdata);
    __NodeBase * processGroup(__ParseData & pdata);
    __Ret processSelect(__NodeBase * node,__ParseData & pdata);
    __NodeBase * processRepeat(__NodeBase * node,__ParseData & pdata);
    int processInt(int & result,__ParseData & pdata);
    bool processRange(int & result,__ParseData & pdata);
    int ignoreSubexpMarks(__ParseData & pdata);
};

#endif
