#include <algorithm>
#include <cassert>

#include "regxstring_impl.h"

static const char * const SEP = "  ";

static std::string sep(int lvl)
{
    std::string ret;
    while(lvl-- > 0)
        ret += SEP;
    return ret;
}

//struct __NodeBase
__NodeBase::~__NodeBase()
{}

bool __NodeBase::CanRepeat(int ch)
{
    return true;
}

void __NodeBase::AppendNode(__NodeBase * node)
{
    assert(0);
}

//struct __Edge
__Edge::__Edge(int ch)
{
    switch(ch){
        case '^':begin_ = true;break;
        case '$':begin_ = false;break;
        default:assert(0);
    }
}

void __Edge::RandString(std::ostringstream & oss,__Refs & refs) const
{}

void __Edge::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<(begin_ ? "BEGIN" : "END")<<"\n";
}

//struct __Text
__Text::__Text(int ch)
    : str_(1,ch)
{}

void __Text::RandString(std::ostringstream & oss,__Refs & refs) const
{
    oss<<str_;
}

void __Text::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Text("<<str_<<")\n";
}

//class __Charset
__Charset::__Charset()
    : inc_(true)
{}

__Charset::__Charset(const std::string & str,bool include)
    : str_(str)
    , inc_(include)
{}

void __Charset::RandString(std::ostringstream & oss,__Refs & refs) const
{
    assert(inc_);
    if(!str_.empty())
        oss<<str_[rand() % str_.size()];
}

void __Charset::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Charset("
        <<(inc_ ? "INCLUDE" : "EXCLUDE")
        <<", "<<str_<<")\n";
}

void __Charset::Exclude()
{
    inc_ = false;
}

void __Charset::AddChar(int ch)
{
    str_.push_back(ch);
}

void __Charset::AddRange(int from,int to)
{
    for(;from <= to;++from)
        str_.push_back(from);
}

void __Charset::AddRange(__Charset * node)
{
    if(!node)
        return;
    unite(*node);
}

void __Charset::Unique()
{
    inc_ ? unique() : reverse();
}

void __Charset::unite(__Charset & node)
{
    if(!node.inc_)
        node.reverse();
    str_ += node.str_;
}

void __Charset::reverse()
{
    const int _CHAR_MIN = 32;
    const int _CHAR_MAX = 126;
    unique();
    std::string s;
    s.swap(str_);
    int c = _CHAR_MIN;
    size_t i = 0,e = s.size();
    for(;c <= _CHAR_MAX && i < e;++i){
        int ch = s[i];
        if(c < ch)
            AddRange(c,ch - 1);
        c = std::max(ch + 1,_CHAR_MIN);
    }
    if(c <= _CHAR_MAX)
        AddRange(c,_CHAR_MAX);
    inc_ = !inc_;
}

void __Charset::unique()
{
    if(!str_.empty()){
        std::sort(str_.begin(),str_.end());
        str_.erase(std::unique(str_.begin(),str_.end()),str_.end());
    }
}

//class __Repeat
__Repeat::__Repeat(__NodeBase * node,int ch)
    : node_(node)
    , min_(0)
    , max_(0)
{
    switch(ch){
        case '?':min_ = 0;max_ = 1;break;
        case '+':min_ = 1;max_ = INFINITE;break;
        case '*':min_ = 0;max_ = INFINITE;break;
        default:;
    }
}

__Repeat::__Repeat(__NodeBase * node,int min,int max)
    : node_(node)
    , min_(min)
    , max_(max)
{}


__Repeat::~__Repeat(){
    if(node_)
        delete node_;
}

void __Repeat::RandString(std::ostringstream & oss,__Refs & refs) const
{
    const int INF_MAX = 5;
    assert(0 <= min_ && (INFINITE == max_ || min_ <= max_) && node_);
    int t = (max_ == INFINITE ? INF_MAX : max_);
    t = min_ + rand() % (t - min_ + 1);
    while(t-- > 0)
        node_->RandString(oss,refs);
}

void __Repeat::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Repeat["<<min_<<", ";
    if(max_ == INFINITE)
        out<<"INF";
    else
        out<<max_;
    out<<"]\n";
    ++lvl;
    if(node_)
        node_->Debug(out,lvl);
    else
        out<<sep(lvl)<<"NULL\n";
}

bool __Repeat::CanRepeat(int ch)
{
    return false;
}

//class __Seq
__Seq::__Seq(__NodeBase * node)
{
    seq_.push_back(node);
}

__Seq::~__Seq(){
    for(__Con::const_iterator i = seq_.begin(),e = seq_.end();i != e;++i)
        delete *i;
}

void __Seq::RandString(std::ostringstream & oss,__Refs & refs) const
{
    for(__Con::const_iterator i = seq_.begin(),e = seq_.end();i != e;++i)
        (*i)->RandString(oss,refs);
}

void __Seq::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Seq("<<seq_.size()<<")\n";
    ++lvl;
    for(__Con::const_iterator i = seq_.begin(),e = seq_.end();i != e;++i){
        if(*i)
            (*i)->Debug(out,lvl);
        else
            out<<sep(lvl)<<"NULL\n";
    }
}

void __Seq::AppendNode(__NodeBase * node)
{
    __Text * cur = dynamic_cast<__Text *>(node);
    if(cur && !seq_.empty()){
        __Text * prev = dynamic_cast<__Text *>(seq_.back());
        if(prev){
            *prev += *cur;
            delete node;
            return;
        }
    }
    seq_.push_back(node);
}

//class __Group
__Group::__Group(__NodeBase * node,int mark)
    : node_(node)
    , mark_(mark)
{}

__Group::~__Group()
{
    if(node_)
        delete node_;
}

void __Group::RandString(std::ostringstream & oss,__Refs & refs) const
{
    if(node_){
        switch(mark_){
            case '!':break;
            case ':':
            case '=':node_->RandString(oss,refs);break;
            default:{
                size_t begin = oss.str().size();
                refs.push_back(std::make_pair(begin,std::string::npos));
                node_->RandString(oss,refs);
                refs.back().second = oss.str().size() - begin;
            }
        }
    }
}

void __Group::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Group(";
    switch(mark_){
        case ':':out<<"?:";break;
        case '=':out<<"?=";break;
        case '!':out<<"?!";break;
        default:out<<mark_;
    }
    out<<")\n";
    ++lvl;
    if(node_)
        node_->Debug(out,lvl);
    else
        out<<sep(lvl)<<"NULL\n";
}

//class __Select
__Select::__Select(__NodeBase * node)
{
    sel_.push_back(node);
}

__Select::~__Select()
{
    for(__Con::const_iterator i = sel_.begin(),e = sel_.end();i != e;++i)
        delete *i;
}

void __Select::RandString(std::ostringstream & oss,__Refs & refs) const
{
    if(!sel_.empty()){
        __NodeBase * n = sel_[rand() % sel_.size()];
        if(n)
            n->RandString(oss,refs);
    }
}

void __Select::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Select("<<sel_.size()<<")\n";
    ++lvl;
    for(__Con::const_iterator i = sel_.begin(),e = sel_.end();i != e;++i)
        if(*i)
            (*i)->Debug(out,lvl);
        else
            out<<sep(lvl)<<"NULL\n";
}

void __Select::AppendNode(__NodeBase * node)
{
    sel_.push_back(node);
}

//class __Ref
__Ref::__Ref(int index)
    : index_(index)
{}

void __Ref::RandString(std::ostringstream & oss,__Refs & refs) const
{
    if(!index_){
        std::string s = oss.str();
        oss<<s;
    }else if(size_t(index_) <= refs.size()){
        __Refs::const_reference ref = refs[index_ - 1];
        std::string str = oss.str();
        if(ref.first < str.size())
            oss<<str.substr(ref.first,ref.second);
    }
}

void __Ref::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Ref("<<index_<<")\n";
}
