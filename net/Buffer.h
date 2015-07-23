#ifndef SIMCODE_NET_BUFFER_H
#define SIMCODE_NET_BUFFER_H
#include <string>
#include <string.h>
namespace simcode
{
namespace net
{
class Buffer
{
public:
    Buffer():seek_(0) {}
    const char* peek() const;
    size_t size() const;
    size_t readableBytes() const;
    void seek(size_t n);
    size_t getSeek() const;
    void retrieve(size_t n);
    void retrieveSeek();
    std::string* mutableBuf();
    const std::string& buf() const;

    void reserve(size_t n);
    void append(const std::string& data);
    void append(const char* data, int len);

private:
    std::string buf_;
    size_t seek_;
};

inline const char* Buffer::peek() const
{
    return buf_.data()+seek_;
}

inline size_t Buffer::size() const
{
    return buf_.size();
}

inline size_t Buffer::readableBytes() const
{
    return buf_.size() - seek_;
}

inline void Buffer::seek(size_t n)
{
    seek_ += n;
}

inline size_t Buffer::getSeek() const
{
    return seek_;
}

inline void Buffer::retrieve(size_t n)
{
    if (n != 0)
    {
        int length = buf_.size()-n;
        memmove(&buf_[0], &buf_[0]+n, length);
        buf_.resize(length);
        seek_ = 0;
    }
}

inline void Buffer::retrieveSeek()
{
    retrieve(seek_);
}

inline std::string* Buffer::mutableBuf()
{
    return &buf_;
}

inline const std::string& Buffer::buf() const
{
    return buf_;
}

inline void Buffer::reserve(size_t n)
{
    buf_.reserve(n);
}

inline void Buffer::append(const std::string& data)
{
    buf_.append(data);
}

inline void Buffer::append(const char* data, int len)
{
    buf_.append(data, len);
}

} //endof namespace net
} //endof namespace simcode

#endif // SIMCODE_NET_BUFFER_H
