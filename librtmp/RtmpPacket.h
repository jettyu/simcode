#ifndef CRTMP_PACKET_H
#define CRTMP_PACKET_H

#include <librtmp/rtmp_sys.h>
#include <librtmp/log.h>

namespace simcode
{
namespace rtmp
{

class CRtmpPacket
{
public:
    CRtmpPacket(int size)
    {
        packet_=(RTMPPacket*)malloc(sizeof(RTMPPacket));
        RTMPPacket_Alloc(packet_,size);

    }
    ~CRtmpPacket()
    {
        Free();
    }

    void Reset()
    {
        RTMPPacket_Reset(packet_);

        packet_->m_hasAbsTimestamp = 0;
        packet_->m_nChannel = 0x04;
    }
    void Fill(uint32_t infoFiled2,
              uint8_t headerType,
              uint32_t timeStamp,
              uint8_t packetType,
              uint32_t bodySize,
              char *body)
    {
        Reset();
        packet_->m_nInfoField2 = infoFiled2;
        packet_->m_headerType = headerType;
		packet_->m_nTimeStamp = timeStamp;
		packet_->m_packetType = packetType;
		packet_->m_nBodySize  = bodySize;
		packet_->m_body = body;
    }

    RTMPPacket* getPacket() const
    {
        return packet_;
    }

    void Free()
    {
        if (packet_)
        {
            RTMPPacket_Free(packet_);
            free(packet_);
            packet_=NULL;
        }
    }

private:
    RTMPPacket*	packet_;
};

}
}

#endif // CRTMP_PACKET_H
