#include <iostream>
#include <fstream>
#include <cstdlib>

#include "tools.h"
#include "regxstring.h"
#include "regxstring_impl.h"

using namespace std;

bool debug;

static std::string pre_handle(const std::string & str)
{
    std::string ret = Tools::Trim(str);
    if(!ret.empty()){
        if(ret[0] != '^')
            ret = "^" + ret;
        if(ret[ret.size() - 1] != '$')
            ret.push_back('$');
    }
    return ret;
}

#ifndef WIN32
#   include "test.h"
#endif

static void printUsage(const char * exe)
{
    cout<<"Usage: "<<exe<<" [N [-t]] [-d]\n"
        <<"  N     generate N random strings, default 1\n"
        <<"  -t    batch test\n"
        <<"  -d    output debug info\n"
        <<endl;
}

static void use(int c)
{
    CRegxString regxstr;
    std::string regx;
    while(std::getline(cin,regx)){
        regxstr.ParseRegx(pre_handle(regx));
        if(debug)
            regxstr.Debug(cout);
        for(int i = 0;i < c;++i)
            cout<<regxstr.RandString()<<endl;
        cout<<endl;
    }
}

int main(int argc,const char ** argv)
{
    int c = 0;
    bool test = false;
    for(int i = 1;i < argc;++i){
        const char * ret = 0;
        if((Tools::ExtractArg(argv[i],"-h",ret) ||
            Tools::ExtractArg(argv[i],"?",ret) ||
            Tools::ExtractArg(argv[i],"--help",ret)) && !ret)
        {
            printUsage(Tools::ProgramName(argv[0]));
            return 0;
        }else if((Tools::ExtractArg(argv[i],"-t",ret) || Tools::ExtractArg(argv[i],"-test",ret)) && !ret){
            test = true;
        }else if((Tools::ExtractArg(argv[i],"-d",ret) || Tools::ExtractArg(argv[i],"-debug",ret)) && !ret){
            debug = true;
        }else
            c = atoi(argv[i]);
    }
    if(c < 0)
        c = 0;
#ifndef WIN32
    if(test)
        test_pcre(c);
    else
#endif
        use(c);
    //test_01();
#if _MEM_LEAK
    cerr<<"__NodeBase::ref = "<<__NodeBase::ref<<endl;
#endif
    return 0;
}
