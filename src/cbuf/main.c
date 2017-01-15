#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include "common.h"

Cycle_Buffer *fifo = NULL;  

U64 InspurPorting_Fifo_Get(DATABUF *buf, U64 len)  
{
    U64 l; 

    if(buf == NULL)
    {
        printf("input wrong, buf = 0x%x\n", (unsigned int)buf);
        return 0;
    }
    
    len = min(len, fifo->in - fifo->out);  
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));  
    memcpy(buf, fifo->buf + (fifo->out & (fifo->size - 1)), l * sizeof(DATABUF));
    memcpy(buf + l, fifo->buf, (len - l) * sizeof(DATABUF));
    fifo->out += len; 
    return len;  
}  

U64 InspurPorting_Fifo_Put(DATABUF *buf, U64 len)  
{  
    U64 l;  
    len = min(len, fifo->size - fifo->in + fifo->out);  
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    if(l > 0)
    {
        memcpy(fifo->buf + (fifo->in & (fifo->size - 1)), buf, l * sizeof(DATABUF));
    }
    
    if((len - l) > 0)
    {
        memcpy(fifo->buf, buf + l, (len - l) * sizeof(DATABUF)); 
    }
    fifo->in += len;  
    return len;  
}  

int main(void)  
{ 
#ifdef _WIN32
	HANDLE wtid_B, wtid_C, rtid;
#endif
    int ret;  
    U64 flag_B = 1, flag_C = 2;

    ret = InspurPorting_Init_Cycle_Buffer();  
    if (ret != 0)
    {
        return ret;  
    }
#if defined(unix) 
    pthread_t wtid_B, wtid_C, rtid; 
    pthread_create(&wtid_B, NULL, InspurPorting_Thread_Write, (void *)&flag_B);  
    InspurPorting_Sleep(1000);
    pthread_create(&wtid_C, NULL, InspurPorting_Thread_Write, (void *)&flag_C);
    pthread_create(&rtid, NULL, InspurPorting_Thread_Read, NULL); 
    pthread_exit(NULL); 
#elif defined(_WIN32)
    wtid_B = CreateThread(NULL, 0, InspurPorting_Thread_Write, (LPVOID)&flag_B, 0, NULL);
    InspurPorting_Sleep(1000);
    wtid_C = CreateThread(NULL, 0, InspurPorting_Thread_Write, (LPVOID)&flag_C, 0, NULL);
	InspurPorting_Sleep(1000);
    rtid = CreateThread(NULL, 0, InspurPorting_Thread_Read, NULL, 0, NULL);
    while(1)
	{
		InspurPorting_Sleep(1000);
	}
#else
#error "Un_Support OS type"
#endif
    return 0;
}

