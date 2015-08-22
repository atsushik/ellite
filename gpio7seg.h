#ifndef __GPIO7SEG__
#define __GPIO7SEG__

// Define GPIO pin number
#define SEG_A 0
#define SEG_B 1
#define SEG_C 4
#define SEG_D 17
#define SEG_E 21
#define SEG_F 22
#define SEG_G 10
#define SEG_H 9
#define SEG_1 23
#define SEG_2 24
#define SEG_3 25
#define SEG_4 11

#define U_DELAY 1

extern int SegLoopFlg;
extern const int number[11][7];
extern int value[4];

pthread_t thread_id1;
void *result;

//int initSignalCatch();
void sigcatch(int sig);
int initGpioFor7Seg();
void start7SegLoopTherad();
void *SegLoop(void *arg);

#endif
