#include <iostream>
#include <fstream>
#include <cstdlib>

#include "tools.h"
#include "regxstring.h"

using namespace std;

#ifndef WIN32
#   include "test.h"
#endif

void use(int argc,const char ** argv)
{
    int c = 1;
    if(argc > 1){
        c = atoi(argv[1]);
        if(c < 1)
            c = 1;
    }
    CRegxString regxstr;
    std::string regx;
    while(std::getline(cin,regx)){
        regxstr.ParseRegx(Tools::Trim(regx));
#if _DZ_DEBUG
        regxstr.Debug(cout);
#endif
        for(int i = 0;i < c;++i)
            cout<<regxstr.RandString()<<endl;
        cout<<endl;
    }
}

int main(int argc,const char ** argv)
{
    batch_test();
    //user(argc,argv);
    return 0;
}
