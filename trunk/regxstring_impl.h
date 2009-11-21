#ifndef DOZERG_REGXSTRING_IMPL_H_20091012
#define DOZERG_REGXSTRING_IMPL_H_20091012

#include <string>
#include <vector>
#include <iosfwd>
#include <sstream>
#include <utility>

#define _DZ_DEBUG 0

typedef std::vector<std::pair<size_t,size_t> > __Refs;

struct __NodeBase
{
    //functions
    virtual ~__NodeBase();
    virtual void RandString(std::ostringstream & oss,__Refs & refs) const = 0;
    virtual void Debug(std::ostream & out,int lvl) const = 0;
    virtual int Repeat(int ch);
    virtual void AppendNode(__NodeBase * node);
};

class __Edge : public __NodeBase
{
    bool begin_;
public:
    explicit __Edge(int ch);
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
};

class __Text : public __NodeBase
{
    std::string str_;
public:
    //functions
    explicit __Text(int ch);
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
    __Text & operator +=(const __Text & other){str_ += other.str_;return *this;}
};

class __Charset : public __NodeBase
{
    std::string str_;
    bool inc_;
public:
    //functions
    __Charset();
    __Charset(const std::string & str,bool include);
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
    void Exclude();
    void AddChar(int ch);
    void AddRange(int from,int to);
    void AddRange(__Charset * node);
    void Unique();
private:
    void unite(__Charset & node);
    void reverse();
    void unique();
};

struct __Repeat : public __NodeBase
{
    static const int INFINITE = -1;
private:
    static const int _REPEAT_MAX = (1 << 6) - 1;
    static const int _NON_GREEDY = 1 << 17;
    static const int _PROSSESSIVE = 1 << 18;
    __NodeBase * node_;
    int min_,max_;
public:
    //functions
    __Repeat(__NodeBase * node,int ch);
    __Repeat(__NodeBase * node,int min,int max);
    ~__Repeat();
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
    int Repeat(int ch);
private:
    bool isNonGreedy() const{return (min_ & _NON_GREEDY) != 0;}
    bool isPossessive() const{return (min_ & _PROSSESSIVE) != 0;}
    bool canRepeat() const{return !(min_ & (_NON_GREEDY | _PROSSESSIVE));}
};

class __Seq : public __NodeBase
{
    typedef std::vector<__NodeBase *> __Con;
    __Con seq_;
public:
    //functions
    explicit __Seq(__NodeBase * node);
    ~__Seq();
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
    void AppendNode(__NodeBase * node);
};

class __Group : public __NodeBase
{
    static const int INDEX = 0x800000;   //group index flag
    static const int MAX_GROUPS = 9;
    __NodeBase * node_;
    int mark_;
public:
    //functions
    __Group(__NodeBase * node,int mark);
    ~__Group();
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
};

class __Select : public __NodeBase
{
    typedef std::vector<__NodeBase *> __Con;
    __Con sel_;
public:
    //functions
    explicit __Select(__NodeBase * node);
    ~__Select();
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
    void AppendNode(__NodeBase * node);
};

class __Ref : public __NodeBase
{
    int index_;
public:
    explicit __Ref(int index);
    void RandString(std::ostringstream & oss,__Refs & refs) const;
    void Debug(std::ostream & out,int lvl) const;
};

#endif
