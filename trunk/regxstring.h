#ifndef DOZERG_REGXSTRING_H_20091012
#define DOZERG_REGXSTRING_H_20091012

#include <vector>
#include "regxstring_impl.h"

class CRegxString
{
    typedef std::vector<char> __Ends;
    typedef std::pair<__NodeBase *,int> __Ret;
    std::string regx_;
    //regx tree
    __NodeBase * top_;
    //parse data
    __Ends ends_;
    size_t i_;
    int ref_;
public:
    CRegxString();
    explicit CRegxString(const std::string & regx);
    ~CRegxString(){uninit();}
    void ParseRegx(const std::string & regx);
    std::string Regx() const{return regx_;}
    std::string RandString() const;
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
