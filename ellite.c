#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>
#include "gpio7seg.h"

#define SERIAL_DEVICE "/dev/ttyUSB0"
#define BAUDRATE B115200
//#define SM_PWD "aaaaaaaaaaaa"
//#define SM_RBID "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
char *sm_pwd;
char *sm_rbid;

#define BUFF_SIZE 255
 
struct echonetlite_epc {
    unsigned char  EPC;
    unsigned char  PDC;
             char *EDT;
};

struct echonetlite_edata {
    unsigned char   SEOJ_CGC;
    unsigned char   SEOJ_CC;
    unsigned char   SEOJ_IC;
    unsigned char   DEOJ_CGC;
    unsigned char   DEOJ_CC;
    unsigned char   DEOJ_IC;
    unsigned char   ESV;
    unsigned char   OPC;
    struct echonetlite_epc *EPCS;
};

struct echonetlite_packet {
    unsigned char  EHD1;
    unsigned char  EHD2;
    unsigned short TID;
    struct echonetlite_edata EDATA;
};

int fd;
enum Stat {INIT, INIT_W, PWD, PWD_W, RBID, RBID_W, SCAN, SCAN_W, SREGS2, SREGS2_W,
           SREGS3, SREGS3_W, LL64, LL64_W, JOIN, JOIN_W, CONNECTED, CONNECTED_W};
enum Stat stat;
int channel=21;
int panID=0;
char addr[17]={};
char addrv6[40]={};

void *recvData(void *arg){
    pid_t pid;
    pthread_t thread_id;
    int res;
    char buf[BUFF_SIZE]={};
    char *n;
    char buf2[BUFF_SIZE]={};
    char *endptr;
    long l;

    pid=getpid();
    thread_id=pthread_self();

    while(1){
        res = read((int)fd, buf, sizeof(buf)-1);
        buf[res]='\0';
        //strcat(buff, buf);
        printf("RECV:%s", buf);
        switch(stat){
        case INIT_W:
            if(strstr(buf,"OK")>0) stat = PWD;
            break;
        case PWD_W:
            if(strstr(buf,"OK")>0) stat = RBID;
            break;
        case RBID_W:
            if(strstr(buf,"OK")>0) stat = SCAN;
            break;
        case SCAN_W:
            if(strstr(buf,"  Channel:")>0){
                sscanf(buf,"  Channel:%x",&channel);
                printf("Channel=%x\n",channel);
            }
            if(strstr(buf,"  Pan ID:")>0){
                sscanf(buf,"  Pan ID:%x",&panID);
                printf("PanID=%x\n",panID);
            }
            if(strstr(buf,"  Addr:")>0){
                sscanf(buf,"  Addr:%s",addr);
                printf("Addr=%s\n",addr);
            }
            if(panID!=0 && strstr(buf,"EVENT 22")>0) stat = SREGS2;
            if(panID==0 && strstr(buf,"EVENT 22")>0) stat = SCAN;
            break;
        case SREGS2_W:
            if(strstr(buf,"OK")>0) stat = SREGS3;
            break;
        case SREGS3_W:
            if(strstr(buf,"OK")>0) stat = LL64;
            break;
        case LL64_W:
            if(strstr(buf,":")>0){
                strncpy(addrv6, buf, sizeof(addrv6)-1);
                printf("Addrv6=%s\n",addrv6);
                stat = JOIN;
            }else{
                //printf("Addrv6 get error.\n");
            }
            break;
        case JOIN_W:
            if(strstr(buf,"EVENT 25")>0) stat = CONNECTED;
            break;
        case CONNECTED_W:
            if((n=strstr(buf, "1081000102880105FF017201E704"))>0){
                strncpy(buf2, "0x", sizeof(buf2));
                strncat(buf2, n+29, sizeof(buf2));
                buf2[9]='\0';
                printf("%sW\n", buf2);
                l=strtol(buf2, &endptr, 16);
                printf("%ldW\n", l);
                //gpio
                if(l<1000){value[0]=10;}else{value[0]=(l/1000)%10;}
                if(l<100){value[1]=10;}else{value[1]=(l/100)%10;}
                if(l<10){value[2]=10;}else{value[2]=(l/10)%10;}
                value[3]=l%10;
                //gpio
                stat = CONNECTED;
            }
            break;
        }
    }
}

void *sendData(void *arg){
    char buf[BUFF_SIZE];
    int len;
    int a;
    int timeout=0;
    
    struct echonetlite_epc see[1] = {{0xE7, 0x00, NULL}};
    struct echonetlite_edata eedata = {0x05, 0xFF, 0x01, 0x02, 0x88, 0x01,
                                       0x62, 0x01, see};
    struct echonetlite_packet ep = {0x10, 0x81, 0x0001, eedata};
    char senddata[200];
    int bytes;

    bytes = getEchonetlitePacket(senddata, &ep);


    for(a=0;a<bytes;a++){
    printf("%02X ",senddata[a]);
    }
    printf("byte=%d\n",bytes);


    while(1){
        switch(stat){
        case INIT:
            strncpy(buf, "SKINFO\r\n", sizeof(buf));
            write((int)fd, buf, strlen(buf));
            stat = INIT_W;
            sleep(1);
            break;
        case PWD:
            sm_pwd = getenv( "sm_pwd" );
            sprintf(buf, "SKSETPWD C %s\r\n", sm_pwd);
            write((int)fd, buf, strlen(buf));
            stat = PWD_W;
            sleep(1);
            break;
        case RBID:
            sm_rbid = getenv( "sm_rbid" );
            sprintf(buf, "SKSETRBID %s\r\n", sm_rbid);
            write((int)fd, buf, strlen(buf));
            stat = RBID_W;
            sleep(1);
            break;
        case SCAN:
            strncpy(buf, "SKSCAN 2 FFFFFFFF 6\r\n", sizeof(buf));
            write((int)fd, buf, strlen(buf));
            stat = SCAN_W;
            sleep(10);
            break;
        case SREGS2:
            sprintf(buf, "SKSREG S2 %x\r\n", channel);
            write((int)fd, buf, strlen(buf));
            stat = SREGS2_W;
            sleep(1);
            break;
        case SREGS3:
            sprintf(buf, "SKSREG S3 %x\r\n", panID);
            write((int)fd, buf, strlen(buf));
            stat = SREGS3_W;
            sleep(1);
            break;
        case LL64:
            sprintf(buf, "SKLL64 %s\r\n", addr);
            write((int)fd, buf, strlen(buf));
            stat = LL64_W;
            sleep(1);
            break;
        case JOIN:
            sprintf(buf, "SKJOIN %s\r\n", addrv6);
            write((int)fd, buf, strlen(buf));
            stat = JOIN_W;
            sleep(10);
            break;
        case CONNECTED:
            sprintf(buf, "SKSENDTO 1 %s 0E1A 1 %04X ", addrv6, bytes);
            len=strlen(buf);
            memcpy(buf+len, senddata, bytes);
            write((int)fd, buf, len+bytes);

            stat = CONNECTED_W;
            sleep(3);
            break;
        case CONNECTED_W:
            if(timeout++>3){
                stat = CONNECTED;
                timeout=0;
            }
            sleep(1);
            break;
        default:
            sleep(2);
            break;
        }
    }
}

void sigInt(int handler){
    char buf[BUFF_SIZE];

    strncpy(buf, "\r\n\r\nSKTERM\r\n", sizeof(buf));
    write((int)fd, buf, strlen(buf));

    //gpio stop
    sigcatch(0);

    sleep(3);

    exit(0);
}

int initSerial(){
    int fd;
    struct termios oldtio, newtio;

    signal(SIGINT, sigInt);
    signal(SIGTERM, sigInt);
    signal(SIGKILL, sigInt);

    fd = open(SERIAL_DEVICE, O_RDWR );
    if(fd < 0){perror(SERIAL_DEVICE); exit(-1);}

    tcgetattr(fd, &oldtio);
    bzero(&newtio, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CREAD;
    newtio.c_lflag = ICANON;

    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* キャラクタ間タイマを使わない */
    newtio.c_cc[VMIN]     = 1;     /* 1文字来るまで，読み込みをブロックする */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

    return fd;
}

int getEchonetlitePacket(char *buf, const struct echonetlite_packet *ep){
    int opc=0;
    int i;
    int pdc=0;
    int b=0;
    unsigned short tid=(ep->TID<<8 & 0xFF00)|(ep->TID>>8);

    memcpy(buf+b, &ep->EHD1, sizeof(ep->EHD1));
    b+=sizeof(ep->EHD1);
    memcpy(buf+b, &ep->EHD2, sizeof(ep->EHD2));
    b+=sizeof(ep->EHD2);
    memcpy(buf+b, &tid, sizeof(tid));
    b+=sizeof(tid);
    memcpy(buf+b, &ep->EDATA.SEOJ_CGC, sizeof(ep->EDATA.SEOJ_CGC));
    b+=sizeof(ep->EDATA.SEOJ_CGC);
    memcpy(buf+b, &ep->EDATA.SEOJ_CC, sizeof(ep->EDATA.SEOJ_CC));
    b+=sizeof(ep->EDATA.SEOJ_CC);
    memcpy(buf+b, &ep->EDATA.SEOJ_IC, sizeof(ep->EDATA.SEOJ_IC));
    b+=sizeof(ep->EDATA.SEOJ_IC);
    memcpy(buf+b, &ep->EDATA.DEOJ_CGC, sizeof(ep->EDATA.DEOJ_CGC));
    b+=sizeof(ep->EDATA.DEOJ_CGC);
    memcpy(buf+b, &ep->EDATA.DEOJ_CC, sizeof(ep->EDATA.DEOJ_CC));
    b+=sizeof(ep->EDATA.DEOJ_CC);
    memcpy(buf+b, &ep->EDATA.DEOJ_IC, sizeof(ep->EDATA.DEOJ_IC));
    b+=sizeof(ep->EDATA.DEOJ_IC);
    memcpy(buf+b, &ep->EDATA.ESV, sizeof(ep->EDATA.ESV));
    b+=sizeof(ep->EDATA.ESV);
    memcpy(buf+b, &ep->EDATA.OPC, sizeof(ep->EDATA.OPC));
    b+=sizeof(ep->EDATA.OPC);
    opc = ep->EDATA.OPC;
    for(i=0;i<opc;i++){
        memcpy(buf+b, &ep->EDATA.EPCS[i].EPC, sizeof(ep->EDATA.EPCS[i].EPC));
        b+=sizeof(ep->EDATA.EPCS[i].EPC);
        memcpy(buf+b, &ep->EDATA.EPCS[i].PDC, sizeof(ep->EDATA.EPCS[i].PDC));
        b+=sizeof(ep->EDATA.EPCS[i].PDC);
        pdc = ep->EDATA.EPCS[i].PDC;
        memcpy(buf+b, ep->EDATA.EPCS[i].EDT, pdc);
        b+=pdc;
    }
    return b;
}

void main(){
    pid_t p_pid;
    pthread_t thread_recv,thread_send;
    int status;
    void *result;

    struct termios oldtio,newtio;

    //gpio
    initGpioFor7Seg();
    start7SegLoopTherad();
    //gpio

    stat = INIT;

    fd = initSerial();

    p_pid=getpid();

//    printf("[%d]start\n",p_pid);

    status=pthread_create(&thread_recv,NULL,recvData,(void *)fd);
/*    if(status!=0){
        fprintf(stderr,"pthread_create : %s",strerror(status));
    }else{
        printf("[%d]thread_recv=%d\n",p_pid,thread_recv);
    }
*/
    status=pthread_create(&thread_send,NULL,sendData,(void *)fd);
/*    if(status!=0){
        fprintf(stderr,"pthread_create : %s",strerror(status));
    }else{
        printf("[%d]thread_send=%d\n",p_pid,thread_send);
    }
*/
    pthread_join(thread_recv,&result);
    printf("[%d]thread_recv = %d end\n",p_pid,thread_recv);
    pthread_join(thread_send,&result);
    printf("[%d]thread_send = %d end\n",p_pid,thread_send);

    //gpio
    pthread_join(thread_id1,&result);
    //gpio

    printf("[%d]end\n",p_pid);
}
