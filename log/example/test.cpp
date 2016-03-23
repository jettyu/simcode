#include <simcode/log/logasync.h>
#include <simcode/log/logging.h>
#include <unistd.h>
using namespace simcode;
using namespace log;

int main()
{
	GlobalLogAsync::Init("all.log", 0, 1);
//	GlobalLogging::Init("all.log", 0, 1);
	LOG_DEBUG("%s", "test");
	LOG_DEBUG("%s", "test");
	LOG_DEBUG("%s", "test");
        sleep(1);
	return 0;
}
