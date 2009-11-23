#ifndef DOZERG_ALLOC_H_20080514
#define DOZERG_ALLOC_H_20080514

#include <memory>

//allocator choice
#ifndef __GNUC__ 
#   define __DZ_ALLOC   std::allocator
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

// Replacements for new and delete
template<class T>
T * New(){
    T * ret = __DZ_ALLOC<T>().allocate(1);
    return new (ret) T;
}

template<class T,class A>
T * New(const A & a){
    T * ret = __DZ_ALLOC<T>().allocate(1);
    return new (ret) T(a);
}

template<class T,class A,class B>
T * New(const A & a,const B & b){
    T * ret = __DZ_ALLOC<T>().allocate(1);
    return new (ret) T(a,b);
}

template<class T,class A,class B,class C>
T * New(const A & a,const B & b,const C & c){
    T * ret = __DZ_ALLOC<T>().allocate(1);
    return new (ret) T(a,b,c);
}
template<class T>
void Delete(T * p){
    typedef char __dummy[sizeof(T)];
    if(p){
        p->~T();
        __DZ_ALLOC<T>().deallocate(p,1);
    }
}


#endif
