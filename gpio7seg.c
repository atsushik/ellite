#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include <pthread.h>
#include <time.h>
#include "gpio7seg.h"

int SegLoopFlg = 1;

const int number[11][7] = {{1,1,1,1,1,1,0}, //0
                           {0,1,1,0,0,0,0}, //1
                           {1,1,0,1,1,0,1}, //2
                           {1,1,1,1,0,0,1}, //3
                           {0,1,1,0,0,1,1}, //4
                           {1,0,1,1,0,1,1}, //5
                           {1,0,1,1,1,1,1}, //6
                           {1,1,1,0,0,1,0}, //7
                           {1,1,1,1,1,1,1}, //8
                           {1,1,1,1,0,1,1}, //9
                           {0,0,0,0,0,0,0}};//brank 

int value[4] = {10,10,10,10}; //all brank

/*
int main(void) {
    time_t timer;
    struct tm *local;

    if(0!=initSignalCatch()){
        printf("Signal Catch Setting Error.\n");
        return 0;
    }

    initGpioFor7Seg();

    start7SegLoopTherad();

    while(SegLoopFlg){
        timer = time(NULL);
        local = localtime(&timer);
        value[0]=local->tm_min/10;
        value[1]=local->tm_min%10;
        value[2]=local->tm_sec/10;
        value[3]=local->tm_sec%10;
        sleep(1);
    }

    SegLoopFlg=0;

    pthread_join(thread_id1,&result);

    return 0;
}

int initSignalCatch(){
    if (SIG_ERR == signal(SIGHUP, sigcatch)) return 1;
    if (SIG_ERR == signal(SIGINT, sigcatch)) return 1;
    if (SIG_ERR == signal(SIGTERM, sigcatch)) return 1;
    return 0;
}
*/

void sigcatch(int sig){
    SegLoopFlg=0;
}

int initGpioFor7Seg(){
    // Initialize WiringPi
    if(wiringPiSetupGpio() == -1) return 1;

    // Set GPIO pins to output mode
    pinMode(SEG_A, OUTPUT);
    pinMode(SEG_B, OUTPUT);
    pinMode(SEG_C, OUTPUT);
    pinMode(SEG_D, OUTPUT);
    pinMode(SEG_E, OUTPUT);
    pinMode(SEG_F, OUTPUT);
    pinMode(SEG_G, OUTPUT);
    pinMode(SEG_H, OUTPUT);
    pinMode(SEG_1, OUTPUT);
    pinMode(SEG_2, OUTPUT);
    pinMode(SEG_3, OUTPUT);
    pinMode(SEG_4, OUTPUT);

    digitalWrite(SEG_A, 0);
    digitalWrite(SEG_B, 0);
    digitalWrite(SEG_C, 0);
    digitalWrite(SEG_D, 0);
    digitalWrite(SEG_E, 0);
    digitalWrite(SEG_F, 0);
    digitalWrite(SEG_G, 0);
    digitalWrite(SEG_H, 0);
    digitalWrite(SEG_1, 0);
    digitalWrite(SEG_2, 0);
    digitalWrite(SEG_3, 0);
    digitalWrite(SEG_4, 0);

    return 0;
}

void start7SegLoopTherad(){
    pid_t   p_pid;
    int status;

    p_pid = getpid();

    status = pthread_create(&thread_id1,NULL,SegLoop,(void *)NULL);
    if(status != 0){
        fprintf(stderr,"pthread_create : %s\n", strerror(status));
    }else{
        fprintf(stderr,"7SegLoopThread start!\n", strerror(status));
    }
}

void *SegLoop(void *arg){
    pid_t pid;
    pthread_t thread_id;

    pid = getpid();
    thread_id=pthread_self();

    while(1){
        digitalWrite(SEG_A, number[value[0]][0]);
        digitalWrite(SEG_B, number[value[0]][1]);
        digitalWrite(SEG_C, number[value[0]][2]);
        digitalWrite(SEG_D, number[value[0]][3]);
        digitalWrite(SEG_E, number[value[0]][4]);
        digitalWrite(SEG_F, number[value[0]][5]);
        digitalWrite(SEG_G, number[value[0]][6]);
        digitalWrite(SEG_1, 1);
        delay(U_DELAY);
        digitalWrite(SEG_1, 0);

        digitalWrite(SEG_A, number[value[1]][0]);
        digitalWrite(SEG_B, number[value[1]][1]);
        digitalWrite(SEG_C, number[value[1]][2]);
        digitalWrite(SEG_D, number[value[1]][3]);
        digitalWrite(SEG_E, number[value[1]][4]);
        digitalWrite(SEG_F, number[value[1]][5]);
        digitalWrite(SEG_G, number[value[1]][6]);
        digitalWrite(SEG_2, 1);
        delay(U_DELAY);
        digitalWrite(SEG_2, 0);

        digitalWrite(SEG_A, number[value[2]][0]);
        digitalWrite(SEG_B, number[value[2]][1]);
        digitalWrite(SEG_C, number[value[2]][2]);
        digitalWrite(SEG_D, number[value[2]][3]);
        digitalWrite(SEG_E, number[value[2]][4]);
        digitalWrite(SEG_F, number[value[2]][5]);
        digitalWrite(SEG_G, number[value[2]][6]);
        digitalWrite(SEG_3, 1);
        delay(U_DELAY);
        digitalWrite(SEG_3, 0);

        digitalWrite(SEG_A, number[value[3]][0]);
        digitalWrite(SEG_B, number[value[3]][1]);
        digitalWrite(SEG_C, number[value[3]][2]);
        digitalWrite(SEG_D, number[value[3]][3]);
        digitalWrite(SEG_E, number[value[3]][4]);
        digitalWrite(SEG_F, number[value[3]][5]);
        digitalWrite(SEG_G, number[value[3]][6]);
        digitalWrite(SEG_4, 1);
        delay(U_DELAY);
        digitalWrite(SEG_4, 0);

        digitalWrite(SEG_4, 0);

        if(!SegLoopFlg) break;
    }

    // Turn off LED
    digitalWrite(SEG_A, 0);
    digitalWrite(SEG_B, 0);
    digitalWrite(SEG_C, 0);
    digitalWrite(SEG_D, 0);
    digitalWrite(SEG_E, 0);
    digitalWrite(SEG_F, 0);
    digitalWrite(SEG_G, 0);
    digitalWrite(SEG_H, 0);
    digitalWrite(SEG_1, 0);
    digitalWrite(SEG_2, 0);
    digitalWrite(SEG_3, 0);

    return (arg);
}
