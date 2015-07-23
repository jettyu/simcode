#include <simcode/librtmp/RtmpConn.h>

#ifdef WIN32
#define fseeko fseeko64
#define ftello ftello64
#include <io.h>
#include <fcntl.h>
#define	SET_BINMODE(f)	setmode(fileno(f), O_BINARY)
#else
#define	SET_BINMODE(f)
#endif

#define RD_SUCCESS		0
#define RD_FAILED		1
#define RD_INCOMPLETE		2
#define RD_NO_CONNECT		3

#define DEF_TIMEOUT	30	/* seconds */
#define DEF_BUFTIME	(10 * 60 * 60 * 1000)	/* 10 hours default */
#define DEF_SKIPFRM	0
#include <stdlib.h>

using namespace simcode;
using namespace rtmp;

static int handleRecv(RTMP * rtmp,		// connected RTMP object
                      uint32_t dSeek, uint32_t dStopOffset, double duration, int bResume,
                      char *metaHeader, uint32_t nMetaHeaderSize,
                      char *initialFrame, int initialFrameType, uint32_t nInitialFrameSize,
                      int nSkipKeyFrames, int bStdoutMode, int bLiveStream, int bRealtimeStream, int bHashes, int bOverrideBufferTime,
                      uint32_t bufferTime, double *percent, RtmpRecvCallback* recv_callback_);

bool RtmpConn::handleRead()
{
    int nStatus = RD_SUCCESS;
    double percent = 0;
    double duration = 0.0;

    int nSkipKeyFrames = DEF_SKIPFRM;	// skip this number of keyframes when resuming

    int bOverrideBufferTime = FALSE;	// if the user specifies a buffer time override this is true
    int bStdoutMode = TRUE;	// if true print the stream directly to stdout, messages go to stderr
    int bResume = FALSE;		// true in resume mode
    uint32_t dSeek = 0;		// seek position in resume mode, 0 otherwise
    uint32_t bufferTime = DEF_BUFTIME;

    // meta header and initial frame for the resume mode (they are read from the file and compared with
    // the stream we are trying to continue
    char *metaHeader = 0;
    uint32_t nMetaHeaderSize = 0;

    // video keyframe for matching
    char *initialFrame = 0;
    uint32_t nInitialFrameSize = 0;
    int initialFrameType = 0;	// tye: audio or video

    int port = -1;
    int bLiveStream = FALSE;	// is it a live stream? then we can't seek/resume
    int bRealtimeStream = FALSE;  // If true, disable the BUFX hack (be patient)
    int bHashes = FALSE;		// display byte counters not hashes by default

    uint32_t dStopOffset = 0;

    nStatus != handleRecv(rtmp_, dSeek, dStopOffset, duration, bResume,
                          metaHeader, nMetaHeaderSize, initialFrame,
                          initialFrameType, nInitialFrameSize, nSkipKeyFrames,
                          bStdoutMode, bLiveStream, bRealtimeStream, bHashes,
                          bOverrideBufferTime, bufferTime, &percent, recv_callback_);
    free(initialFrame);
    initialFrame = NULL;
    if (nStatus != RD_INCOMPLETE || !RTMP_IsTimedout(rtmp_) || bLiveStream)
        return false;
}

static int handleRecv(RTMP * rtmp,		// connected RTMP object
                      uint32_t dSeek, uint32_t dStopOffset, double duration, int bResume,
                      char *metaHeader, uint32_t nMetaHeaderSize,
                      char *initialFrame, int initialFrameType, uint32_t nInitialFrameSize,
                      int nSkipKeyFrames, int bStdoutMode, int bLiveStream, int bRealtimeStream, int bHashes, int bOverrideBufferTime,
                      uint32_t bufferTime, double *percent,
                      RtmpRecvCallback* recv_callback_)	// percentage downloaded [out]
{
    int32_t now, lastUpdate;
    int bufferSize = 64 * 1024;
    char *buffer;
    int nRead = 0;
    unsigned long lastPercent = 0;
    rtmp->m_read.timestamp = dSeek;

    *percent = 0.0;

    if (rtmp->m_read.timestamp)
    {
        RTMP_Log(RTMP_LOGDEBUG, "Continuing at TS: %d ms\n", rtmp->m_read.timestamp);
    }

    if (bLiveStream)
    {
        RTMP_LogPrintf("Starting Live Stream\n");
    }
    else
    {
        // print initial status
        // Workaround to exit with 0 if the file is fully (> 99.9%) downloaded
        if (duration > 0)
        {
            if ((double) rtmp->m_read.timestamp >= (double) duration * 999.0)
            {
                RTMP_LogPrintf("Already Completed at: %.3f sec Duration=%.3f sec\n",
                               (double) rtmp->m_read.timestamp / 1000.0,
                               (double) duration / 1000.0);
                return RD_SUCCESS;
            }
            else
            {
                *percent = ((double) rtmp->m_read.timestamp) / (duration * 1000.0) * 100.0;
                *percent = ((double) (int) (*percent * 10.0)) / 10.0;
            }
        }
        else
        {

        }
        if (bRealtimeStream)
            RTMP_LogPrintf("  in approximately realtime (disabled BUFX speedup hack)\n");
    }

    if (dStopOffset > 0)
        RTMP_LogPrintf("For duration: %.3f sec\n", (double) (dStopOffset - dSeek) / 1000.0);

    if (bResume && nInitialFrameSize > 0)
        rtmp->m_read.flags |= RTMP_READ_RESUME;
    rtmp->m_read.initialFrameType = initialFrameType;
    rtmp->m_read.nResumeTS = dSeek;
    rtmp->m_read.metaHeader = metaHeader;
    rtmp->m_read.initialFrame = initialFrame;
    rtmp->m_read.nMetaHeaderSize = nMetaHeaderSize;
    rtmp->m_read.nInitialFrameSize = nInitialFrameSize;

    buffer = (char *) malloc(bufferSize);

    now = RTMP_GetTime();
    lastUpdate = now - 1000;
    do
    {
        nRead = RTMP_Read(rtmp, buffer, bufferSize);
        //RTMP_LogPrintf("nRead: %d\n", nRead);
        if (nRead > 0)
        {
            if (recv_callback_) recv_callback_->run(buffer, nRead);
            //RTMP_LogPrintf("write %dbytes (%.1f kB)\n", nRead, nRead/1024.0);
            if (duration <= 0)	// if duration unknown try to get it from the stream (onMetaData)
                duration = RTMP_GetDuration(rtmp);

            if (duration > 0)
            {
                // make sure we claim to have enough buffer time!
                if (!bOverrideBufferTime && bufferTime < (duration * 1000.0))
                {
                    bufferTime = (uint32_t) (duration * 1000.0) + 5000;	// extra 5sec to make sure we've got enough

                    RTMP_Log(RTMP_LOGDEBUG,
                             "Detected that buffer time is less than duration, resetting to: %dms",
                             bufferTime);
                    RTMP_SetBufferMS(rtmp, bufferTime);
                    RTMP_UpdateBufferMS(rtmp);
                }
                *percent = ((double) rtmp->m_read.timestamp) / (duration * 1000.0) * 100.0;
                *percent = ((double) (int) (*percent * 10.0)) / 10.0;
                if (bHashes)
                {
                    if (lastPercent + 1 <= *percent)
                    {
                        RTMP_LogStatus("#");
                        lastPercent = (unsigned long) *percent;
                    }
                }
                else
                {
                    now = RTMP_GetTime();
                    if (abs(now - lastUpdate) > 200)
                    {

                        lastUpdate = now;
                    }
                }
            }
            else
            {
                now = RTMP_GetTime();
                if (abs(now - lastUpdate) > 200)
                {
                    if (bHashes)
                        RTMP_LogStatus("#");
                    else
                        ;
                    lastUpdate = now;
                }
            }
        }
        else
        {
#ifdef _DEBUG
            RTMP_Log(RTMP_LOGDEBUG, "zero read!");
#endif
            if (rtmp->m_read.status == RTMP_READ_EOF)
                break;
        }

    }
    while (!RTMP_ctrlC && nRead > -1 && RTMP_IsConnected(rtmp) && !RTMP_IsTimedout(rtmp));
    free(buffer);
    if (nRead < 0)
        nRead = rtmp->m_read.status;

    /* Final status update */
    if (!bHashes)
    {
        if (duration > 0)
        {
            *percent = ((double) rtmp->m_read.timestamp) / (duration * 1000.0) * 100.0;
            *percent = ((double) (int) (*percent * 10.0)) / 10.0;
        }
        else
        {

        }
    }

    RTMP_Log(RTMP_LOGDEBUG, "RTMP_Read returned: %d", nRead);

    if (bResume && nRead == -2)
    {
        RTMP_LogPrintf("Couldn't resume FLV file, try --skip %d\n\n",
                       nSkipKeyFrames + 1);
        return RD_FAILED;
    }

    if (nRead == -3)
        return RD_SUCCESS;

    if ((duration > 0 && *percent < 99.9) || RTMP_ctrlC || nRead < 0
            || RTMP_IsTimedout(rtmp))
    {
        return RD_INCOMPLETE;
    }

    return RD_SUCCESS;
}
