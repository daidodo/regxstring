//#include <boost/regex.hpp>
#include <pcre.h>

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
        cout<<"error at offset "<<erroffset<<": "<<error<<"\n\n";
    return re;
}

static bool pcre_exec_DZ(const std::string & str,pcre * re)
{
    const int size = 60;
    std::vector<int> ovector(size);
    int rc = pcre_exec(
        re,                   /* the compiled pattern */
        0,                    /* no extra data - we didn't study the pattern */
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
            default:cout<<"Matching error "<<rc<<" :\n";break;
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
        regxstr.ParseRegx(regx);
        for(int i = 0;i < c;++i){
            std::string str = regxstr.RandString();
            if(!pcre_exec_DZ(str,re))
                cout<<str<<endl;
        }
        cout<<endl;
        pcre_free(re);
    }
}

static void test_01()
{
    std::string regx = "(a\\1+(\\2+(\\3+)))";
    pcre * re = pcre_compile_DZ(regx);
    std::string str;
    cout<<"search string matching regx : "<<regx<<endl;
    for(int i = 0;i < 1000;++i,str.push_back('a')){
        if(pcre_exec_DZ(str,re))
            cout<<str<<endl;
    }
}
