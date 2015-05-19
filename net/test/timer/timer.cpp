#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <simcode/net/EPollPoller.h>
#include <simcode/net/Socket.h>
#include <simcode/net/Timer.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/EventLoopThreadPool.h>
using namespace simcode;
using namespace simcode::net;

#define handle_error(msg) \
	   do { perror(msg); exit(EXIT_FAILURE); } while (0)

void printTime(int i)
{  
     printf("hello world!|i=%d\n", i);
}


int main(void)
{

   EventLoopThreadPool loopPool;
   loopPool.setThreadNum(1);
   loopPool.start();
   EventLoop* ioLoop = loopPool.getNextLoop();
   if (ioLoop) ioLoop->runAfter(0, simex::bind(printTime, 1));
   getchar();
   return 1;

   EventLoop loop;
   loop.runAfter(0, simex::bind(printTime, 1));
   loop.runEvery(1, simex::bind(printTime, 2));
   loop.loop();
   //EPollPoller poller;
   //Timer timer(simex::bind(&EPollPoller::removeEvent, &poller, _1));
   //timer.setTimer(simex::bind(printTime, 3), 0);
   //poller.addEvent(timer.timerfd(), simex::bind(&Timer::handleEvent, &timer, _1));
   //Timer timer1(simex::bind(&EPollPoller::removeEvent, &poller, _1));
   //timer1.setTimer(simex::bind(printTime, 4), 0);
   //poller.addEvent(timer1.timerfd(), simex::bind(&Timer::handleEvent, &timer1, _1));
   //while(1) poller.poll(16);
   return 0;
}
