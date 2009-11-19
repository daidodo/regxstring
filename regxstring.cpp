#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cassert>

#include "tools.h"
#include "regxstring.h"

static void appendNode(__NodeBase *& parent,__NodeBase * node)
{
    if(!node)
        return;
    if(!parent)
        parent = new __Seq(node);
    else
        parent->AppendNode(node);
}

//class CRegxString
CRegxString::CRegxString()
    : top_(0)
    , i_(0)
    , ref_(0)
{}

CRegxString::CRegxString(const std::string & regx)
    : top_(0)
    , i_(0)
    , ref_(0)
{
    ParseRegx(regx);
}

void CRegxString::ParseRegx(const std::string & regx)
{
    uninit();
    i_ = ref_ = 0;
    ends_.clear();
    regx_ = regx;
    if(!regx_.empty()){
        top_ = processSeq().first;
        srand((unsigned int)time(0));
    }
}

std::string CRegxString::RandString() const
{
    __Refs refs;
    std::ostringstream oss;
    if(top_)
        top_->RandString(oss,refs);
    return oss.str();
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
        delete top_;
        top_ = 0;
    }
}

int CRegxString::inEnds(int ch) const
{
    int ret = 1;
    for(__Ends::const_reverse_iterator i = ends_.rbegin();i != ends_.rend();++i,++ret){
        if(ch == *i)
            return ret;
        if(Tools::NeedEnd(*i))
            break;
    }
    return 0;
}

CRegxString::__Ret CRegxString::processSeq()
{
    __Ret ret;
    __NodeBase * cur = 0;
    bool begin = true;
    for(const size_t e = regx_.length();i_ < e;++i_){
        int ch = regx_[i_];
        if(begin && Tools::IsBegin(ch)){
            cur = new __Edge(ch);
            begin = false;
            continue;
        }
        if(Tools::IsRepeat(ch) && cur && cur->CanRepeat(ch)){
            cur = new __Repeat(cur,ch);
            continue;
        }
        if(Tools::IsRepeatBegin(ch)){
            cur = processRepeat(cur);
            continue;
        }
        appendNode(ret.first,cur);
        ret.second = inEnds(ch);
        if(ret.second)
            return ret;
        if(Tools::IsSelect(ch))
            return processSelect(ret.first);
        if(Tools::IsEnd(ch))
            cur = new __Edge(ch);
        else if(Tools::IsAny(ch)){
             __Charset * set = new __Charset("\n",false);
             set->Unique();
             cur = set;
        }else if(Tools::IsSetBegin(ch))
            cur = processSet();
        else if(Tools::IsGroupBegin(ch))
            cur = processGroup();
        else if(Tools::IsSlash(ch))
            cur = processSlash(true).first;
        else
            cur = new __Text(ch);
    }
    appendNode(ret.first,cur);
    return ret;
}

CRegxString::__Ret CRegxString::processSlash(bool bNode)
{
    ++i_;
    __Ret ret(0,i_ < regx_.length() ? Tools::TransSlash(regx_[i_]) : '\\');
    __Charset * set = 0;
    switch(ret.second){
        case 'd':set = new __Charset("0123456789",true);break;
        case 'D':set = new __Charset("0123456789",false);break;
        case 's':set = new __Charset(/*"\f\n\r\v"*/"\t ",true);break;
        case 'S':set = new __Charset(/*"\f\n\r\v"*/"\t ",false);break;
        case 'w':{   //A-Za-z0-9_
            set = new __Charset();
            set->AddRange('A','Z');
            set->AddRange('a','z');
            set->AddRange('0','9');
            set->AddChar('_');
            break;}
        case 'W':{   //^A-Za-z0-9_
            set = new __Charset();
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
        if(Tools::IsDigit(ret.second) && ref_ + '0' >= ret.second)
            ret.first = new __Ref(ret.second - '0');
        else
            ret.first = new __Text(ret.second);
    }
    return ret;
}

__NodeBase * CRegxString::processSet()
{
    size_t bak = i_++;
    __Charset * ret = new __Charset();
    bool begin = true;
    int prev = 0;
    for(const size_t e = regx_.length();i_ < e;++i_){
        int ch = regx_[i_];
        if(begin && Tools::IsBegin(ch)){
            ret->Exclude();
            begin = false;
            continue;
        }
        if(Tools::IsDash(ch) && prev){
            int to = 0;
            if(processRange(to)){
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
            __Ret s = processSlash(false);
            if(s.first){    //charset
                ret->AddRange(dynamic_cast<__Charset *>(s.first));
                delete s.first;
                prev = 0;
                continue;
            }
            ch = s.second;
        }
        prev = ch;
    }
    delete ret;
    i_ = bak;
    return new __Text('[');
}

__NodeBase * CRegxString::processGroup()
{
    int bak = i_++;
    int mark = ignoreSubexpMarks();
    ends_.push_back(')');
    if(!mark)
        mark = ++ref_;
    __Ret ret = processSeq();
    ends_.pop_back();
    if(ret.second)
        return new __Group(ret.first,mark);
    delete ret.first;
    i_ = bak;
    return new __Text('(');
}

CRegxString::__Ret CRegxString::processSelect(__NodeBase * node)
{
    __Ret ret(new __Select(node),0);
    ends_.push_back('|');
    for(const size_t e = regx_.length();i_ < e;){
        ++i_;
        __Ret r = processSeq();
        ret.first->AppendNode(r.first);
        if(r.second > 1){
            ret.second = r.second - 1;
            break;
        }
    }
    ends_.pop_back();
    return ret;
}

__NodeBase * CRegxString::processRepeat(__NodeBase * node)
{
    if(node && node->CanRepeat(0)){
        size_t bak = i_++;
        int min = 0,max = __Repeat::INFINITE;
        switch(processInt(min)){
            case ',':
                ++i_;
                if(processInt(max) == '}')
                    return new __Repeat(node,min,(min < max ? max : min));
                break;
            case '}':
                return new __Repeat(node,min,min);
            default:;
        }
        i_ = bak;
    }
    return new __Text('{');
}

int CRegxString::processInt(int & result)
{
    bool begin = true;
    for(const size_t e = regx_.length();i_ < e;++i_){
        int ch = regx_[i_];
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

bool CRegxString::processRange(int & result)
{
    if(++i_ < regx_.size() && regx_[i_] != ']'){
        result = regx_[i_];
        return true;
    }
    --i_;
    return false;
}

int CRegxString::ignoreSubexpMarks()
{
    int ret = 0;
    if(i_ + 1 < regx_.size()){
        ret = Tools::IsSubexpMark(&regx_[i_]);
        if(ret)
            i_ += 2;
    }
    return ret;
}
