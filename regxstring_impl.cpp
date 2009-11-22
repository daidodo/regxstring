#include <algorithm>
#include <cassert>

#include "tools.h"
#include "regxstring_impl.h"

#if _DZ_DEBUG
#   include <iostream>

static void printRefs(std::ostringstream & oss,const __Refs & refs)
{
    for(__Refs::const_iterator i = refs.begin();i != refs.end();++i)
        std::cout<<"\t"<<oss.str().substr(i->first,i->second);
}

#   define _OSS_OUT(msg) {  \
        std::cout<<msg<<" : "<<oss.str(); \
        printRefs(oss,refs);    \
        std::cout<<std::endl;}

#else
#   define _OSS_OUT(str)
#endif

static const char * const SEP = "  ";

static std::string sep(int lvl)
{
    std::string ret;
    while(lvl-- > 0)
        ret += SEP;
    return ret;
}

struct __IsNull
{
    bool operator ()(__NodeBase * n) const{
        return 0 == n;
    }
};

//struct __NodeBase
__NodeBase::~__NodeBase()
{}

int __NodeBase::Repeat(int ch)
{
    return 1;
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

__NodeBase * __Edge::Optimize()
{
    return REP_NULL;
}

void __Edge::RandString(std::ostringstream & oss,__Refs & refs) const
{
    _OSS_OUT("__Edge");
}

void __Edge::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<(begin_ ? "BEGIN" : "END")<<"\n";
}

//struct __Text
__Text::__Text(int ch)
    : str_(1,ch)
{}

__NodeBase * __Text::Optimize()
{
    return (str_.empty() ? REP_NULL : 0);
}

void __Text::RandString(std::ostringstream & oss,__Refs & refs) const
{
    oss<<str_;
    _OSS_OUT("__Text");
}

void __Text::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Text("<<str_<<")\n";
}

//class __Charset
__Charset::__Charset()
    : inc_(1)
{}

__Charset::__Charset(const std::string & str,bool include)
    : str_(str)
    , inc_(include)
{}

__NodeBase * __Charset::Optimize()
{
    if(!inc_)
        reverse();
    if(str_.empty())
        return REP_NULL;
    inc_ = str_.size();
    return 0;
}

void __Charset::RandString(std::ostringstream & oss,__Refs & refs) const
{
    assert(inc_ == str_.size());
    oss<<str_[rand() % inc_];
    _OSS_OUT("__Charset");
}

void __Charset::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Charset(INCLUDE"
        <<", "<<str_<<")\n";
}

void __Charset::Exclude()
{
    inc_ = 0;
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
    , min_(min < _REPEAT_MAX ? min : _REPEAT_MAX)
    , max_(max < _REPEAT_MAX ? max : _REPEAT_MAX)
{}

__Repeat::~__Repeat(){
    if(node_)
        delete node_;
}

__NodeBase * __Repeat::Optimize()
{
    min_ &= _REPEAT_MAX;
    max_ &= _REPEAT_MAX;
    if(!node_ || (min_ > max_) || (!min_ && !max_))
        return REP_NULL;
    __NodeBase * r = node_->Optimize();
    if(r == REP_NULL)
        return REP_NULL;
    else if(r){
        delete node_;
        node_ = r;
    }
    if(1 == max_ && 1 == min_){
        r = node_;
        node_ = 0;
        return r;
    }
    max_ -= min_ - 1;
    return 0;
}

void __Repeat::RandString(std::ostringstream & oss,__Refs & refs) const
{
    for(int t = min_ + rand() % max_;t > 0;t--)
        node_->RandString(oss,refs);
    _OSS_OUT("__Repeat");
}

void __Repeat::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Repeat["<<min_<<", "<<(min_ + max_ - 1)<<"]\n";
    ++lvl;
    if(node_)
        node_->Debug(out,lvl);
    else
        out<<sep(lvl)<<"NULL\n";
}

int __Repeat::Repeat(int ch)
{
    if(canRepeat()){
        switch(ch){
            case '?':min_ |= _NON_GREEDY;return 2;break;
            case '+':min_ |= _PROSSESSIVE;return 2;break;
            default:;
        }
    }
    return 0;
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

__NodeBase * __Seq::Optimize()
{
    if(seq_.empty())
        return REP_NULL;
    for(__Con::iterator i = seq_.begin(),e = seq_.end();i != e;++i)
        if(*i){
            __NodeBase * r = (*i)->Optimize();
            if(r == REP_NULL)
                *i = 0;
            else if(r){
                delete *i;
                *i = r;
            }
        }
    seq_.erase(std::remove_if(seq_.begin(),seq_.end(),__IsNull()),seq_.end());
    if(seq_.empty())
        return REP_NULL;
    if(seq_.size() == 1){
        __NodeBase * r = seq_[0];
        seq_.clear();
        return r;
    }
    return 0;
}

void __Seq::RandString(std::ostringstream & oss,__Refs & refs) const
{
    for(__Con::const_iterator i = seq_.begin(),e = seq_.end();i != e;++i)
        (*i)->RandString(oss,refs);
    _OSS_OUT("__Seq");
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
{
    if(!Tools::IsSubexpMark(mark_))
        mark_ |= INDEX;
}

__Group::~__Group()
{
    if(node_)
        delete node_;
}

__NodeBase * __Group::Optimize()
{
    if(!node_ || mark_ == '!')
        return REP_NULL;
    __NodeBase * r = node_->Optimize();
    if(r == REP_NULL)
        return REP_NULL;
    else if(r){
        delete node_;
        node_ = r;
    }
    switch(mark_){
        case ':':
        case '=':
        case '>':{
            r = node_;
            node_ = 0;
            return r;}
        default:;
    }
    mark_ = (mark_ & (INDEX - 1)) - 1;
    return 0;
}

void __Group::RandString(std::ostringstream & oss,__Refs & refs) const
{
    assert(node_);
    assert(0 <= mark_ && mark_ < MAX_GROUPS);
    if(mark_ >= refs.size())
        refs.resize(mark_ + 1);
    refs.back() = __Refs::value_type(oss.str().size(),std::string::npos);
    node_->RandString(oss,refs);
    assert(mark_ < refs.size());
    refs[mark_].second = oss.str().size() - refs[mark_].first;
    _OSS_OUT("__Group");
}

void __Group::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Group(";
    switch(mark_){
        case ':':out<<"?:";break;
        case '=':out<<"?=";break;
        case '!':out<<"?!";break;
        case '>':out<<"?>";break;
        default:out<<(mark_ & (INDEX - 1));
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

__NodeBase * __Select::Optimize()
{
    if(sel_.empty())
        return REP_NULL;
    for(__Con::iterator i = sel_.begin(),e = sel_.end();i != e;++i)
        if(*i){
            __NodeBase * r = (*i)->Optimize();
            if(r == REP_NULL)
                *i = 0;
            else if(r){
                delete *i;
                *i = r;
            }
        }
    sel_.erase(std::remove_if(sel_.begin(),sel_.end(),__IsNull()),sel_.end());
    if(sel_.empty())
        return REP_NULL;
    if(sel_.size() == 1){
        __NodeBase * r = sel_[0];
        sel_.clear();
        return r;
    }
    return 0;
}

void __Select::RandString(std::ostringstream & oss,__Refs & refs) const
{
    if(!sel_.empty()){
        __NodeBase * n = sel_[rand() % sel_.size()];
        if(n)
            n->RandString(oss,refs);
    }
    _OSS_OUT("__Ref");
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

__NodeBase * __Ref::Optimize()
{
    --index_;
    return 0;
}

void __Ref::RandString(std::ostringstream & oss,__Refs & refs) const
{
    assert(index_ < refs.size());
    __Refs::const_reference ref = refs[index_];
    std::string str = oss.str();
    if(ref.first < str.size())
        oss<<str.substr(ref.first,ref.second);
    _OSS_OUT("__Ref("<<index_<<")");
}

void __Ref::Debug(std::ostream & out,int lvl) const
{
    out<<sep(lvl)<<"Ref("<<index_<<")\n";
}
