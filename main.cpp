#include <iostream>
#include <fstream>
#include <cstdlib>

#include "tools.h"
#include "regxstring.h"

using namespace std;

int main(int argc,const char ** argv)
{
    int c = 1;
    if(argc > 1){
        c = atoi(argv[1]);
        if(c < 1)
            c = 1;
    }
    CRegxString regxstr;
    std::string regx;
//*
    std::ifstream inf("input.txt");
    if(!inf.is_open()){
        cerr<<"cannot open input file\n";
        return 1;
    }
    std::getline(inf,regx);
/*/
    while(std::getline(cin,regx))

//*/
    {
        regxstr.ParseRegx(Tools::Trim(regx));
        //regxstr.Debug(cout);
        for(int i = 0;i < c;++i)
            cout<<regxstr.RandString()<<endl;
    }
    return 0;
}
