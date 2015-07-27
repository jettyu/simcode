#ifndef SIMCODE_NET_OUTBUFFER_H
#define SIMCODE_NET_OUTBUFFER_H
#include <string>
#include <string.h>

namespace simcode
{

class OutBuffer
{
public:
    OutBuffer():readIndex_(false),writeIndex_(true),seek_(0) {}
    std::string* mutableWriteBuf()
    {
        return &bufs_[writeIndex_];
    }
    std::string* mutableReadBuf()
    {
        return &bufs_[readIndex_];
    }
    int getSeek() const
    {
        return seek_;
    }
    int readableBytes() const
    {
        return bufs_[readIndex_].size() - seek_;
    }
    const char *peek() const
    {
        return bufs_[readIndex_].data() + seek_;
    }
    void seek(int n)
    {
        seek_ += n;
    }

    void resetSeek()
    {
        seek_ = 0;
    }
    void changeIndex()
    {
        readIndex_ = !readIndex_;
        writeIndex_ = !writeIndex_;
    }
    void append(const std::string& data)
    {
        bufs_[writeIndex_].append(data);
    }
    void append(const char* data, int len)
    {
        bufs_[writeIndex_].append(data, len);
    }
private:
    std::string bufs_[2];
    bool readIndex_;
    bool writeIndex_;
    int seek_;
};

}
#endif
