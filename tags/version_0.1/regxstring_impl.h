#ifndef DOZERG_REGXSTRING_IMPL_H_20091012
#define DOZERG_REGXSTRING_IMPL_H_20091012

#include <string>
#include <vector>
#include <iosfwd>
#include <utility>
#include <memory>

#define _DZ_DEBUG 0

#define _MEM_LEAK 0

//allocator choice
#ifndef __GNUC__ 
#   define __DZ_ALLOC       std::allocator
#else
#   ifndef NDEBUG
#       define __DZ_ALLOC   std::allocator
#   else
#       include <ext/pool_allocator.h>
#       define __DZ_ALLOC   __gnu_cxx::__pool_alloc
#   endif
#endif

//stl containers redefine
    //Sequence
#define __DZ_BASIC_STRING(C)            std::basic_string< C,std::char_traits< C >,__DZ_ALLOC< C > >
#define __DZ_BASIC_STRING1(C,T)         std::basic_string< C,T,__DZ_ALLOC< C > >
#define __DZ_STRING                     __DZ_BASIC_STRING(char)
#define __DZ_WSTRING                    __DZ_BASIC_STRING(wchar_t)
#define __DZ_DEQUE(T)                   std::deque< T,__DZ_ALLOC< T > >
#define __DZ_LIST(T)                    std::list< T,__DZ_ALLOC< T > >
#define __DZ_VECTOR(T)                  std::vector< T,__DZ_ALLOC< T > >
    //Associative
#define __DZ_MAP(K,V)                   std::map< K,V,std::less< K >,__DZ_ALLOC<std::pair< K,V > > >
#define __DZ_MAP1(K,V,C)                std::map< K,V,C,__DZ_ALLOC<std::pair< K,V > > >
#define __DZ_MULTIMAP(K,V)              std::multimap< K,V,std::less< K >,__DZ_ALLOC<std::pair< K,V > > >
#define __DZ_MULTIMAP1(K,V,C)           std::multimap< K,V,C,__DZ_ALLOC<std::pair< K,V > > >
#define __DZ_SET(K)                     std::set< K,std::less< K >,__DZ_ALLOC< K > >
#define __DZ_SET1(K,C)                  std::set< K,C,__DZ_ALLOC< K > >
#define __DZ_MULTISET(K)                std::multiset< K,std::less< K >,__DZ_ALLOC< K > >
#define __DZ_MULTISET1(K,C)             std::multiset< K,C,__DZ_ALLOC< K > >
    //String Stream
#define __DZ_BASIC_ISTRINGSTREAM(C)     std::basic_istringstream< C,std::char_traits< C >,__DZ_ALLOC< C > >
#define __DZ_BASIC_ISTRINGSTREAM1(C,T)  std::basic_istringstream< C,T,__DZ_ALLOC< C > >
#define __DZ_BASIC_OSTRINGSTREAM(C)     std::basic_ostringstream< C,std::char_traits< C >,__DZ_ALLOC< C > >
#define __DZ_BASIC_OSTRINGSTREAM1(C,T)  std::basic_ostringstream< C,T,__DZ_ALLOC< C > >
#define __DZ_BASIC_STRINGSTREAM(C)      std::basic_stringstream< C,std::char_traits< C >,__DZ_ALLOC< C > >
#define __DZ_BASIC_STRINGSTREAM1(C,T)   std::basic_stringstream< C,T,__DZ_ALLOC< C > >
#define __DZ_ISTRINGSTREAM              __DZ_BASIC_ISTRINGSTREAM(char)
#define __DZ_OSTRINGSTREAM              __DZ_BASIC_OSTRINGSTREAM(char)
#define __DZ_STRINGSTREAM               __DZ_BASIC_STRINGSTREAM(char)
#define __DZ_WISTRINGSTREAM             __DZ_BASIC_ISTRINGSTREAM(wchar_t)
#define __DZ_WOSTRINGSTREAM             __DZ_BASIC_OSTRINGSTREAM(wchar_t)
#define __DZ_WSTRINGSTREAM              __DZ_BASIC_STRINGSTREAM(wchar_t)
    //Stream Buf
#define __DZ_BASIC_STRINGBUF(C)         std::basic_stringbuf< C,std::char_traits< C >,__DZ_ALLOC< C > >
#define __DZ_BASIC_STRINGBUF1(C,T)      std::basic_stringbuf< C,T,__DZ_ALLOC< C > >
#define __DZ_STRINGBUF                  __DZ_BASIC_STRINGBUF(char)
#define __DZ_WSTRINGBUF                 __DZ_BASIC_STRINGBUF(wchar_t)
    //Extension
#define __DZ_ROPE(T)                    __gnu_cxx::rope< T,__DZ_ALLOC< T > >
#define __DZ_SLIST(T)                   __gnu_cxx::slist< T,__DZ_ALLOC< T > >

#define REGXSTRING_NS       __DZ_Regx_String

#define NAMESAPCE_BEGIN     namespace __DZ_Regx_String{
#define NAMESAPCE_END       }

struct Config;

NAMESAPCE_BEGIN

typedef std::pair<size_t,size_t>    __RefValue;

typedef __DZ_VECTOR(__RefValue)     __Refs;

typedef __DZ_VECTOR(char)           __Ends;

struct __ParseData{
    __Ends ends_;
    const Config & config_;
    size_t i_;
    int ref_;
    //functions:
    explicit __ParseData(const Config & config)
        : config_(config)
        , i_(0)
        , ref_(0)
    {}
    int inEnds(int ch) const;
};

struct __GenerateData
{
    __Refs refs_;
    __DZ_OSTRINGSTREAM & oss_;
    explicit __GenerateData(__DZ_OSTRINGSTREAM & oss)
        : oss_(oss)
    {}
};

struct __NodeBase
{
    static __NodeBase * const REP_NULL; //replace with NULL(0)
#if _MEM_LEAK
    static int ref;
    __NodeBase(){++ref;}
#endif
    virtual ~__NodeBase();
    virtual __NodeBase * Optimize(__ParseData & pdata) = 0;
    virtual void RandString(__GenerateData & gdata) const = 0;
    virtual void Debug(std::ostream & out,int lvl) const = 0;
    virtual int Repeat(int ch);
    virtual void AppendNode(__NodeBase * node);
};

class __Edge : public __NodeBase
{
    bool begin_;
public:
    explicit __Edge(int ch);
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
};

class __Text : public __NodeBase
{
    __DZ_STRING str_;
public:
    //functions
    explicit __Text(int ch);
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
    __Text & operator +=(const __Text & other){str_ += other.str_;return *this;}
};

class __Charset : public __NodeBase
{
    __DZ_STRING str_;
    size_t inc_;
public:
    //functions
    __Charset();
    __Charset(const __DZ_STRING & str,bool include);
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
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
    static const int INFINITE = 1 << 16;
private:
    static const int _REPEAT_MAX = __Repeat::INFINITE - 1;
    static const int _NON_GREEDY = 1 << 17;
    static const int _PROSSESSIVE = 1 << 18;
    static const int _CLEAR_FLAGS = _NON_GREEDY - 1;
    __NodeBase * node_;
    int min_,max_;
public:
    //functions
    __Repeat(__NodeBase * node,int ch);
    __Repeat(__NodeBase * node,int min,int max);
    ~__Repeat();
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
    int Repeat(int ch);
private:
    bool isInfinite() const{return (max_ & INFINITE) != 0;}
    bool isNonGreedy() const{return (min_ & _NON_GREEDY) != 0;}
    bool isPossessive() const{return (min_ & _PROSSESSIVE) != 0;}
    bool canRepeat() const{return !(min_ & (_NON_GREEDY | _PROSSESSIVE));}
};

class __Seq : public __NodeBase
{
    typedef __DZ_VECTOR(__NodeBase *) __Con;
    __Con seq_;
public:
    //functions
    explicit __Seq(__NodeBase * node);
    ~__Seq();
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
    void AppendNode(__NodeBase * node);
};

class __Group : public __NodeBase
{
    static const int INDEX = 1 << 16;   //group index flag
    static const size_t MAX_GROUPS = 9;
    __NodeBase * node_;
    size_t mark_;
public:
    //functions
    __Group(__NodeBase * node,int mark);
    ~__Group();
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
};

class __Select : public __NodeBase
{
    typedef __DZ_VECTOR(__NodeBase *) __Con;
    __Con sel_;
    size_t sz_;
public:
    //functions
    explicit __Select(__NodeBase * node);
    ~__Select();
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
    void AppendNode(__NodeBase * node);
};

class __Ref : public __NodeBase
{
    size_t index_;
public:
    explicit __Ref(int index);
    __NodeBase * Optimize(__ParseData & pdata);
    void RandString(__GenerateData & gdata) const;
    void Debug(std::ostream & out,int lvl) const;
};

class __CRegxString
{
    typedef std::pair<__NodeBase *,int> __Ret;
public:
    __CRegxString();
    ~__CRegxString(){uninit();}
    void ParseRegx(const __DZ_STRING & regx,const Config * config);
    __DZ_STRING Regx() const{return regx_;}
    const __DZ_STRING & RandString();
    const __DZ_STRING & LastString() const{return str_;}
    void Debug(std::ostream & out) const;
private:
    __CRegxString(const __CRegxString &);
    __CRegxString & operator =(const __CRegxString &);
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
    //fields:
    __DZ_STRING regx_;
    __DZ_STRING str_;
    __NodeBase * top_;  //regx tree
};

NAMESAPCE_END

#endif
