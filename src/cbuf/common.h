#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(unix)
#include <pthread.h>
#define U64FORMAT "%llu"
typedef unsigned long long U64;
#elif defined(_WIN32)
#include <windows.h>
#define U64FORMAT "%I64u"
typedef unsigned __int64 U64;
#else
#error "Un_Support OS type"
#endif

#define BUFFSIZE 32  
#define min(x, y) ((x) < (y) ? (x) : (y))

typedef struct tag_DATABUF
{
    U64 flag;
    U64 data;
}DATABUF;

typedef struct tagCycle_Buffer {  
    DATABUF *buf;
    U64 size;  
    U64 in;  
    U64 out;
#if defined(unix)
    pthread_mutex_t lock;
#elif defined(_WIN32)
    HANDLE lock;
#endif
}Cycle_Buffer;  

extern Cycle_Buffer *fifo;

U64 InspurPorting_Fifo_Get(DATABUF *buf, U64 len);
U64 InspurPorting_Fifo_Put(DATABUF *buf, U64 len);
int InspurPorting_Init_Cycle_Buffer(void);
int InspurPorting_Sleep(unsigned int time);

#if defined(unix)
void * InspurPorting_Thread_Read(void *arg);
void * InspurPorting_Thread_Write(void *arg);
#elif defined(_WIN32)
DWORD WINAPI InspurPorting_Thread_Read(LPVOID arg);
DWORD WINAPI InspurPorting_Thread_Write(LPVOID arg);
#else
#error "Un_Support OS type"
#endif

#ifdef __cplusplus
}
#endif
#endif

