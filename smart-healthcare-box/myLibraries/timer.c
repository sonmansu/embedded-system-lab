//timer version 5
/*
#include <stdio.h>
#include <time.h>
int main(void)
{
        clock_t start, end;
	int flag = 0; //타이머가 작동되기 전
        long i = 0;
	long deadline = 30000000;
        double pst;
        start = clock();   //현재 시간
        while (i < deadline)  //현재 시간에서 약을 먹기로 한 시간까지의 차
        {
               i++;
        }
        end = clock();	   //알람 시간에 도착
        pst = (double)(end - start) / CLK_TCK; 
	flag = 1;    //타이머 작동
        printf("time:%f\n", pst);
        return 0;
}
// https://jaimemin.tistory.com/36
*/

//timer version 4

/*
#include <time.h>
#include <stdio.h>

    time_t rawTime;
    struct tm* pTimeInfo;
 
    rawTime = time(NULL);                // 현재 시간을 받음
    pTimeInfo = localtime(&rawTime);    // 현재 시간을 struct tm에 넣음
 
    printf("time_t : %lld\n", rawTime);
 
    int year = pTimeInfo->tm_year + 1900;    //연도에는 1900 더해줌
    int month = pTimeInfo->tm_mon + 1;    // 월에는 1 더해줌
    int day = pTimeInfo->tm_mday;
    int hour = pTimeInfo->tm_hour;
    int min = pTimeInfo->tm_min;
    int sec = pTimeInfo->tm_sec;
    printf("timeInfo : %d년 %d월 %d일 %d시 %d분 %d초\n", year, month, day, hour, min, sec);
    
    time_t now;
 struct tm * ts;
 char buf [80];
 
 now = time (NULL);
 ts = localtime (& now);
 strftime (buf, sizeof (buf), "% a% Y-% m-% d% H:% M:% S% Z", ts);
    
    //https://www.ibm.com/docs/ko/i/7.3?topic=functions-strftime-convert-datetime-string
*/

//timer version 3
/*
#include <stdio.h>
#include <time.h>

int main(void)
{
	int endTime = (unsigned)time(NULL); //끝나는 시간
	endTime += 5;  //5초
	
	while(1){
		int startTime = (unsigned)time(NULL); //현재시간(while)문을 통해 점점 늘어나는 시간;
		//printf("%d second \n", endTime - startTime);
		if(endTime - startTime == 0)
		{
			printf("end!\n");
			return 0;
		}
	}
}
*/

/* Timer version 2
//gcc 옵션으로 -lrt를 줄 것
//gcc timer.c -o timer -lrt
//리눅스에서 사용한 타이머 출처:https://sinwho.tistory.com/entry/%EB%A6%AC%EB%88%85%EC%8A%A4-C%EC%96%B8%EC%96%B4-%ED%83%80%EC%9D%B4%EB%A8%B8Timer-%EC%82%AC%EC%9A%A9%EB%B2%95-%EB%B0%8F-%EC%98%88%EC%A0%9C
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
int flag = 0;
// 타이머 주기에 따라 호출될 타이머
void timer()
{
    if(flag == 0) {
    	printf("timer\n");
	flag = 1;
    }
}
 
int createTimer( timer_t *timerID, int sec, int msec )  
{  
    struct sigevent         te;  
    struct itimerspec       its;  
    struct sigaction        sa;  
    int                     sigNo = SIGRTMIN;  
   
    // Set up signal handler. 
    sa.sa_flags = SA_SIGINFO;  
    sa.sa_sigaction = timer;     // 타이머 호출시 호출할 함수 
    sigemptyset(&sa.sa_mask);  
  
    if (sigaction(sigNo, &sa, NULL) == -1)  
    {  
        printf("sigaction error\n");
        return -1;  
    }  
   
    //Set and enable alarm 
    te.sigev_notify = SIGEV_SIGNAL;  
    te.sigev_signo = sigNo;  
    te.sigev_value.sival_ptr = timerID;  
    timer_create(CLOCK_REALTIME, &te, timerID);  
   
    its.it_interval.tv_sec = sec;
    its.it_interval.tv_nsec = msec * 1000000;  
    its.it_value.tv_sec = sec;
    
    its.it_value.tv_nsec = msec * 1000000;
    timer_settime(*timerID, 0, &its, NULL);  
   
    return 0;  
}
 
int main()
{
    timer_t timerID;
    
    // 타이머를 만든다
    // 매개변수 1 : 타이머 변수
    // 매개변수 2 : second
    // 매개변수 3 : ms
    int second = 0;
    scanf("%d",&second);     //타이머를 몇 초로 할 건지를 블루통신을 통해 입력받거나 하면 될 것 같습니다.
    createTimer(&timerID, second, 0);
    
    while(1)
    {
     	if(flag == 1) {      //타이머가 울린 여부를 flag 변수를 통해 알 수 있습니다.
		flag = 0;
		break;   
	}
    }
    
}
*/
	
//timer version 1
//아래코드는 윈도우에서만 작동함.
/*
#include <stdio.h>
#include <windows.h>
#include <time.h>

int main() {
	int countTime = 5;
	while (1) {
		printf("%d초\n", countTime);
		Sleep(1000);
		countTime -= 1;

		if (countTime <= 0)
		{
			printf("end!\n");
			return 0;
		}
	}
}
*/
