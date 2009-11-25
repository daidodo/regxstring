#ifndef DOZERG_REGXSTRING_H_20091012
#define DOZERG_REGXSTRING_H_20091012

struct Config
{
    static const int REPEAT_INFINITE = 3;
    int repeatInfinite;
    //functions:
    Config():repeatInfinite(REPEAT_INFINITE){}
};

class CRegxString
{
    class __CRegxString * impl_;
public:
    CRegxString():impl_(0){}
    explicit CRegxString(const char * regx);
    ~CRegxString();
    void ParseRegx(const char * regx,const Config * config = 0);
    const char * Regx() const;
    const char * RandString();
    const char * LastString() const;
    void Debug(std::ostream & out) const;
};




#endif
