#ifndef DOZERG_REGXSTRING_H_20091012
#define DOZERG_REGXSTRING_H_20091012

class __CRegxString;

class CRegxString
{
    __CRegxString * impl_;
public:
    CRegxString():impl_(0){}
    explicit CRegxString(const char * regx);
    ~CRegxString();
    void ParseRegx(const char * regx);
    const char * Regx() const;
    const char * RandString();
    const char * LastString() const;
    void Debug(std::ostream & out) const;
};

//struct Config{
//    int infiniteVal;
//    //functions:
//    Config():repeatInfinite(0)
//};



#endif
