//#include <boost/regex.hpp>
#include <pcre.h>
#include <vector>

//void test_boost()
//{
//    using namespace boost;
//    CRegxString regxstr;
//    std::string regx;
//    while(std::getline(cin,regx)){
//        regx = pre_handle(regx);
//        cout<<"Test regx : "<<regx<<endl;
//        regex expr;
//        try{
//            expr.assign(regx,regex_constants::normal);
//        }catch(regex_error & e){
//            cout<<"NOT supported : "<<e.what()<<"\n\n";
//            continue;
//        }
//        regxstr.ParseRegx(regx);
//        for(int i = 0;i < 10;++i){
//            std::string str = regxstr.RandString();
//            if(!regex_match(str, expr))
//                cout<<str<<endl;
//        }
//        cout<<endl;
//    }
//}

static pcre * pcre_compile_DZ(const std::string & regx)
{
    const char *error;
    int erroffset;
    pcre * re = pcre_compile(
        regx.c_str(),
        0,
        &error,
        &erroffset,
        0);
    if (!re)
        cerr<<"error at offset "<<erroffset<<": "<<error<<"\n\n";
    return re;
}

static bool pcre_exec_DZ(const std::string & str,pcre * re)
{
    const int size = 120;
    std::vector<int> ovector(size);
    pcre_extra extra;
    extra.flags = PCRE_EXTRA_MATCH_LIMIT;
    extra.match_limit = 200000000;
    int rc = pcre_exec(
        re,                   /* the compiled pattern */
        &extra,               /* no extra data - we didn't study the pattern */
        str.c_str(),          /* the subject string */
        (int)str.size(),      /* the length of the subject */
        0,                    /* start at offset 0 in the subject */
        0,                    /* default options */
        &ovector[0],          /* output vector for substring information */
        size);                /* number of elements in the output vector */
    if (rc < 0){
        switch(rc){
            case PCRE_ERROR_NOMATCH:break;
                // Handle other special cases if you like
            default:cerr<<"Matching error "<<rc<<" :\n";break;
        }
        return false;
    }
    return true;
}


static void test_pcre(int c)
{
    CRegxString regxstr;
    std::string regx;
    while(std::getline(cin,regx)){
        regx = pre_handle(regx);
        cout<<"Test regx : "<<regx<<endl;
        pcre * re = pcre_compile_DZ(regx);
        if (!re)
            continue;
        regxstr.ParseRegx(regx.c_str());
        for(int i = 0;i < c;++i){
            regxstr.RandString();
            const std::string & str = regxstr.LastString();
            if(!pcre_exec_DZ(str,re))
                cerr<<str<<endl;
        }
        cout<<endl;
        pcre_free(re);
    }
}

