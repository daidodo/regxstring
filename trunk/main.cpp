#include <iostream>
#include <fstream>
#include <cstdlib>

#include "tools.h"
#include "regxstring.h"

using namespace std;

bool debug;
const char * filename;

static __DZ_STRING pre_handle(const __DZ_STRING & str)
{
    __DZ_STRING ret = Tools::Trim(str);
    if(!ret.empty()){
        if(ret[0] != '^')
            ret = "^" + ret;
        if(ret[ret.size() - 1] != '$')
            ret.push_back('$');
    }
    return ret;
}

#ifdef TEST
#   include "test.h"
#endif

static void printUsage(const char * exe)
{
    cout<<"Usage: "<<exe<<" [N] [-t] [-d] [-f=FILE] [-h|?|--help]\n"
        <<"  N          generate N random strings, default 1\n"
#ifdef TEST
        <<"  -t         batch test\n"
#endif
        <<"  -d         output debug info\n"
        <<"  -f         use FILE as input\n"
        <<"  -h\n"
        <<"  ?\n"
        <<"  --help     print this message\n"
        <<endl;
}

static void use(int c)
{
    CRegxString regxstr;
    __DZ_STRING regx;
    std::istream * in = &cin;
    if(filename){
        std::ifstream * file = New<std::ifstream>(filename);
        if(!file->is_open()){
            Delete(file);
            cerr<<"cannot open file "<<filename<<endl;
            return;
        }
        in = file;
    }
    while(std::getline(*in,regx)){
        regxstr.ParseRegx(pre_handle(regx).c_str());
        if(debug)
            regxstr.Debug(cout);
        for(int i = 0;i < c;++i)
            cout<<regxstr.RandString()<<endl;
        cout<<endl;
    }
    if(filename)
        Delete(in);
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
        }else if((Tools::ExtractArg(argv[i],"-f=",ret) || Tools::ExtractArg(argv[i],"-debug",ret)) && ret){
            filename = ret;
        }else
            c = atoi(argv[i]);
    }
    if(c < 0)
        c = 0;
#ifdef TEST
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
