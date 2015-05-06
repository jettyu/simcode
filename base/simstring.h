#ifndef SIMCODE_STRING_H
#define SIMCODE_STRING_H
#include <string>
namespace simcode
{

class ConstString
{
public:
    ConstString(const std::string& s, size_t beginIndex=0, size_t endIndex=0):
        src_(&s),
        beginIndex_(beginIndex),
        endIndex_(endIndex)
    {
    }
    const std::string& getString() const
    {
        return *src_;
    }
    const char* getBuf() const
    {
        return &(*src_)[0] + beginIndex_;
    }
    int bufSize() const
    {
        return src_->size() - beginIndex_ - endIndex_;
    }
    size_t beginIndex() const
    {
        return beginIndex_;
    }
    size_t endIndex() const
    {
        return endIndex_;
    }
protected:
    const std::string* src_;
    size_t beginIndex_;
    size_t endIndex_;
};

class String : public ConstString
{
public:
    String(std::string* s, size_t beginIndex__=0, size_t endIndex__=0):
        ConstString(*s, beginIndex__, endIndex__)
    {
    }
    void resize(size_t len)
    {
        const_cast<std::string*>(src_)->resize(beginIndex_+len + endIndex_);
    }
    char* getMutableBuf()
    {
        return &(*const_cast<std::string*>(src_))[0] + beginIndex_;
    }
    void setBeginOffset(size_t n)
    {
        const_cast<size_t>(beginIndex_) = n;
    }
    void setEndOffset(size_t n)
    {
        const_cast<size_t>(endIndex_) = n;
    }
};

}
#endif
