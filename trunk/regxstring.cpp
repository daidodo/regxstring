#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>

#include "tools.h"
#include "regxstring.h"
#include "regxstring_impl.h"

static void appendNode(__NodeBase *& parent,__NodeBase * node)
{
    if(!node)
        return;
    if(!parent)
        parent = New<__Seq>(node);
    else
        parent->AppendNode(node);
}

static int inEnds(int ch,const __Ends & ends)
{
    int ret = 1;
    for(__Ends::const_reverse_iterator i = ends.rbegin();i != ends.rend();++i,++ret){
        if(ch == *i)
            return ret;
        if(Tools::NeedEnd(*i))
            break;
    }
    return 0;
}


//class CRegxString
CRegxString::CRegxString()
    : top_(0)
{}

CRegxString::CRegxString(const __DZ_STRING & regx)
    : top_(0)
{
    ParseRegx(regx);
}

void CRegxString::ParseRegx(const __DZ_STRING & regx)
{
    uninit();
    regx_ = regx;
    if(regx_.empty())
        return;
    __ParseData pdata;
    top_ = processSeq(pdata).first;
    if(!top_)
        return;
    __NodeBase * r = top_->Optimize();
    if(r){
        Delete(top_);
        top_ = (r == __NodeBase::REP_NULL ? 0 : r);
    }
    if(top_)
        srand((unsigned int)time(0));
}

const char * CRegxString::RandString()
{
    __Refs refs;
    __DZ_OSTRINGSTREAM oss;
    if(top_)
        top_->RandString(oss,refs);
    str_ = oss.str();
    return str_.c_str();
}

void CRegxString::Debug(std::ostream & out) const{
    out<<"regx_ : "<<regx_<<"\nstructure :\n";
    if(top_)
        top_->Debug(out,0);
    else
        out<<"NULL\n";
}

void CRegxString::uninit()
{
    if(top_){
        Delete(top_);
        top_ = 0;
    }
    str_.clear();
}

CRegxString::__Ret CRegxString::processSeq(__ParseData & pdata)
{
    __Ret ret;
    __NodeBase * cur = 0;
    bool begin = true;
    for(const size_t e = regx_.length();pdata.i_ < e;++pdata.i_){
        int ch = regx_[pdata.i_];
        if(begin){
            if(Tools::IsBegin(ch)){
                cur = New<__Edge>(ch);
                continue;
            }
            begin = false;
        }
        if(Tools::IsRepeat(ch) && cur){
            int r = cur->Repeat(ch);
            if(r){
                if(r == 1)
                    cur = New<__Repeat>(cur,ch);
                continue;
            }
        }
        if(Tools::IsRepeatBegin(ch)){
            cur = processRepeat(cur,pdata);
            continue;
        }
        appendNode(ret.first,cur);
        ret.second = inEnds(ch,pdata.ends_);
        if(ret.second)
            return ret;
        if(Tools::IsSelect(ch))
            return processSelect(ret.first,pdata);
        if(Tools::IsEnd(ch))
            cur = New<__Edge>(ch);
        else if(Tools::IsAny(ch)){
             __Charset * set = New<__Charset>("\n",false);
             set->Unique();
             cur = set;
        }else if(Tools::IsSetBegin(ch))
            cur = processSet(pdata);
        else if(Tools::IsGroupBegin(ch))
            cur = processGroup(pdata);
        else if(Tools::IsSlash(ch))
            cur = processSlash(true,pdata).first;
        else
            cur = New<__Text>(ch);
    }
    appendNode(ret.first,cur);
    return ret;
}

CRegxString::__Ret CRegxString::processSlash(bool bNode,__ParseData & pdata)
{
    ++pdata.i_;
    __Ret ret(0,pdata.i_ < regx_.length() ? Tools::TransSlash(regx_[pdata.i_]) : '\\');
    __Charset * set = 0;
    switch(ret.second){
        case 'd':set = New<__Charset>("0123456789",true);break;
        case 'D':set = New<__Charset>("0123456789",false);break;
        case 's':set = New<__Charset>(/*"\f\n\r\v"*/"\t ",true);break;
        case 'S':set = New<__Charset>(/*"\f\n\r\v"*/"\t ",false);break;
        case 'w':{   //A-Za-z0-9_
            set = New<__Charset>();
            set->AddRange('A','Z');
            set->AddRange('a','z');
            set->AddRange('0','9');
            set->AddChar('_');
            break;}
        case 'W':{   //^A-Za-z0-9_
            set =  New<__Charset>();
            set->AddRange('A','Z');
            set->AddRange('a','z');
            set->AddRange('0','9');
            set->AddChar('_');
            set->Exclude();
            break;}
        default:;
    }
    if(set){
        set->Unique();
        ret.first = set;
    }else if(bNode){
        if(Tools::IsDigit(ret.second)){
            int i = ret.second - '0';
            if(!i)
                ret.second = 0;
            else if(i <= pdata.ref_)
                ret.first = New<__Ref>(i);
        }
        if(!ret.first)
            ret.first = New<__Text>(ret.second);
    }
    return ret;
}

__NodeBase * CRegxString::processSet(__ParseData & pdata)
{
    size_t bak = pdata.i_++;
    __Charset * ret = New<__Charset>();
    bool begin = true;
    int prev = 0;
    for(const size_t e = regx_.length();pdata.i_ < e;++pdata.i_){
        int ch = regx_[pdata.i_];
        if(begin && Tools::IsBegin(ch)){
            ret->Exclude();
            begin = false;
            continue;
        }
        if(Tools::IsDash(ch) && prev){
            int to = 0;
            if(processRange(to,pdata)){
                ret->AddRange(prev,to);
                prev = 0;
                continue;
            }
        }
        if(prev)
            ret->AddChar(prev);
        if(Tools::IsSetEnd(ch)){
            ret->Unique();
            return ret;
        }
        if(Tools::IsSlash(ch)){
            __Ret s = processSlash(false,pdata);
            if(s.first){    //charset
                ret->AddRange(dynamic_cast<__Charset *>(s.first));
                Delete(s.first);
                prev = 0;
                continue;
            }
            ch = s.second;
        }
        prev = ch;
    }
    Delete(ret);
    pdata.i_ = bak;
    return New<__Text>('[');
}

__NodeBase * CRegxString::processGroup(__ParseData & pdata)
{
    int bak = pdata.i_++;
    int mark = ignoreSubexpMarks(pdata);
    pdata.ends_.push_back(')');
    if(!mark)
        mark = ++pdata.ref_;
    __Ret ret = processSeq(pdata);
    pdata.ends_.pop_back();
    if(ret.second)
        return New<__Group>(ret.first,mark);
    Delete(ret.first);
    pdata.i_ = bak;
    return New<__Text>('(');
}

CRegxString::__Ret CRegxString::processSelect(__NodeBase * node,__ParseData & pdata)
{
    __Ret ret(New<__Select>(node),0);
    pdata.ends_.push_back('|');
    for(const size_t e = regx_.length();pdata.i_ < e;){
        ++pdata.i_;
        __Ret r = processSeq(pdata);
        ret.first->AppendNode(r.first);
        if(r.second > 1){
            ret.second = r.second - 1;
            break;
        }
    }
    pdata.ends_.pop_back();
    return ret;
}

__NodeBase * CRegxString::processRepeat(__NodeBase * node,__ParseData & pdata)
{
    if(node && node->Repeat(0)){
        size_t bak = pdata.i_++;
        int min = 0,max = __Repeat::INFINITE;
        switch(processInt(min,pdata)){
            case ',':
                ++pdata.i_;
                if(processInt(max,pdata) == '}')
                    return New<__Repeat>(node,min,(min < max ? max : min));
                break;
            case '}':
                return New<__Repeat>(node,min,min);
            default:;
        }
        pdata.i_ = bak;
    }
    return New<__Text>('{');
}

int CRegxString::processInt(int & result,__ParseData & pdata)
{
    bool begin = true;
    for(const size_t e = regx_.length();pdata.i_ < e;++pdata.i_){
        int ch = regx_[pdata.i_];
        if(Tools::IsDigit(ch)){
            ch = Tools::TransDigit(ch);
            if(begin){
                result = ch;
                begin = false;
            }else{
                result *= 10;
                if(result < 0)
                    return 0;
                result += ch;
            }
        }else
            return ch;
    }
    return 0;
}

bool CRegxString::processRange(int & result,__ParseData & pdata)
{
    if(++pdata.i_ < regx_.size() && regx_[pdata.i_] != ']'){
        result = regx_[pdata.i_];
        return true;
    }
    --pdata.i_;
    return false;
}

int CRegxString::ignoreSubexpMarks(__ParseData & pdata)
{
    int ret = 0;
    if(pdata.i_ + 1 < regx_.size()){
        ret = Tools::IsSubexpMark(&regx_[pdata.i_]);
        if(ret)
            pdata.i_ += 2;
    }
    return ret;
}
