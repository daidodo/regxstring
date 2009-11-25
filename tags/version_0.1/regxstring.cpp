#include "regxstring_impl.h"
#include "regxstring.h"

CRegxString::CRegxString(const char * regx)
    : impl_(0)
{
    ParseRegx(regx);
}

CRegxString::~CRegxString()
{
    if(impl_)
        delete impl_;
}

void CRegxString::ParseRegx(const char * regx,const Config * config)
{
    if(!regx)
        return;
    if(!impl_)
        impl_ = new REGXSTRING_NS::__CRegxString;
    impl_->ParseRegx(regx,config);
}

const char * CRegxString::Regx() const
{
    return (impl_ ? impl_->Regx().c_str() : 0);
}

const char * CRegxString::RandString()
{
    return (impl_ ? impl_->RandString().c_str() : 0);
}
const char * CRegxString::LastString() const
{
    return (impl_ ? impl_->LastString().c_str() : 0);
}

void CRegxString::Debug(std::ostream & out) const
{
    if(impl_)
        impl_->Debug(out);
}
