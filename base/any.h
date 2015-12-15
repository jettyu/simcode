#ifndef SIMCODE_BASE_ANY_H
#define SIMCODE_BASE_ANY_H
#include <stdio.h>
#include <algorithm>
#include <typeinfo>
namespace simcode
{
namespace simex
{

class any
{
public:
    any():content_(NULL) {}
    template<typename ValueType>
    any(const ValueType& v):content_(new holder<ValueType>(v)) {}
    any(const any& a):content_(a.content_ ? a.content_->clone() : NULL) {}

    ~any()
    {
        delete content_;
    }

    any& swap(any& s)
    {
        std::swap(content_, s.content_);
        return *this;
    }

    any& operator=(const any& s)
    {
        any(s).swap(*this);
        return *this;
    }

    template<typename ValueType>
    any& operator=(const ValueType& v)
    {
        any(v).swap(*this);
        return *this;
    }

    bool empty() const
    {
        return content_ == NULL;
    }

    void clear()
    {
        any().swap(*this);
    }

    const std::type_info& type() const
    {
        return content_ ? content_->type() : typeid(void);
    }

private:
    class baseholder
    {
    public:
        virtual ~baseholder() {}
        virtual const std::type_info& type() const = 0;
        virtual baseholder* clone() const = 0;
    };
    template<typename ValueType>
    class holder : public baseholder
    {
    public:
        holder(const ValueType& v):value_(v) {}
        virtual const std::type_info& type() const
        {
            return typeid(ValueType);
        }
        virtual baseholder* clone() const
        {
            return new holder(value_);
        }
        ValueType& value()
        {
            return value_;
        }
    private:
        ValueType value_;
    };

public:
    baseholder* content()
    {
        return content_;
    }

private:
    baseholder* content_;
};

template<typename ValueType>
inline ValueType* any_cast(any *a)
{
    return a && a->type()==typeid(ValueType) ?
           &static_cast<any::holder<ValueType> *>(a->content())->value() : NULL;
}

template<typename ValueType>
inline ValueType* any_cast(const any* a)
{
    return any_cast<ValueType>(const_cast<any*>(a));
}

template<typename ValueType>
inline ValueType any_cast(const any& a)
{
    return *any_cast<ValueType>(&a);
}

template<typename ValueType>
inline ValueType* unsafe_any_cast(any* a)
{
    return &static_cast<any::holder<ValueType> *>(a->content())->value();
}

template<typename ValueType>
inline ValueType* unsafe_any_cast(const any* a)
{
    return unsafe_any_cast<ValueType>(const_cast<any*>(a));
}

}
}

#endif // SIMCODE_BASE_ANY_H
