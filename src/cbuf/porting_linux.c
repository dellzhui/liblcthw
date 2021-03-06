#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h> 
#include <time.h>
#include "common.h"

int InspurPorting_Sleep(unsigned int time)
{
    usleep(time * 1000);
    return 0;
}
int InspurPorting_Init_Cycle_Buffer(void)  
{
    U64 size = BUFFSIZE;
    U64 ret = 0;

    ret = size & (size - 1); /*must an integer power of 2*/  
    if (ret)
    {
        printf("size = "U64FORMAT", not support\n", size);
        return -1;
    }
    
    fifo = (Cycle_Buffer *) malloc(sizeof(Cycle_Buffer));  
    if (!fifo)
    {
        printf("can not malloc for fifo");
        return -1;  
    }
    memset(fifo, 0, sizeof(Cycle_Buffer));
    
    fifo->buf = (DATABUF *) malloc(size * sizeof(DATABUF));  
    if(!fifo->buf)
    {
        printf("can not malloc for fifo->buf");
        if(fifo)
        {
            free(fifo);
        }
        return -1;
    }
    memset(fifo->buf, 0, size * sizeof(DATABUF));
    
    fifo->size = size;  
    fifo->in = 0;  
    fifo->out = 0;
    pthread_mutex_init(&fifo->lock, NULL); 
    
    return 0;
}  

void * InspurPorting_Thread_Read(void *arg)  
{  
    DATABUF *data = NULL;
    U64 len = 0;
    U64 n;
    int i = 0;
    
    pthread_detach(pthread_self());

    data = (DATABUF *)calloc(BUFFSIZE, sizeof(DATABUF));
    if(data == NULL)
    {
        printf("%d:can not calloc for data\n", __LINE__);
        return NULL;
    }
    for (;;) {
        U64 sum_b = 0, count_b = 0; 
        U64 sum_c = 0, count_c = 0;
        char flag = '\0';
        
        pthread_mutex_lock(&fifo->lock);
        len = fifo->in - fifo->out;
        n = InspurPorting_Fifo_Get(data, len);
        printf("len:"U64FORMAT", n:"U64FORMAT"\n", len, n);
        for(i = 0; i < len; i++)
        {
            if(data[i].flag == 1)
            {
                flag = 'B';
            }
            else if(data[i].flag == 2)
            {
                flag = 'C';
            }
            printf(U64FORMAT"_%c ", data[i].data, flag);
            if(data[i].flag == 1)
            {
                sum_b += data[i].data;
                count_b++;
            }
            else if(data[i].flag == 2)
            {
                sum_c += data[i].data;
                count_c++;
            }
        }
        printf("\nFrom B:count="U64FORMAT", sum="U64FORMAT"\nFrom C:count="U64FORMAT", sum="U64FORMAT"\n\n", count_b, sum_b, count_c, sum_c);
        pthread_mutex_unlock(&fifo->lock);  
        InspurPorting_Sleep(3 * 1000);
    }
    if(data != NULL)
    {
        free(data);
        data = NULL;
    }
    return NULL;  
}  

void * InspurPorting_Thread_Write(void *arg)  
{  
    DATABUF data = {0, 0};
    int timeshift = 0;
    memcpy(&data.flag, arg, sizeof(data.flag));
    
    srand((U64)time(NULL));
    pthread_detach(pthread_self());  
    for (;;) {
        while(fifo->in == fifo->size + fifo->out)
        {
            int time_shift = (rand() % 50 + 50);
            printf("buf is full, sleep %d ms\n", time_shift);
            InspurPorting_Sleep(time_shift);
        }
        pthread_mutex_lock(&fifo->lock);
        data.data = rand() % 65535;
        timeshift = rand() % 100 + 100;
        InspurPorting_Fifo_Put(&data, 1);  
        pthread_mutex_unlock(&fifo->lock);
        //printf("flag:"U64FORMAT", data:"U64FORMAT", usleep %d ms\n", data.flag, data.data, timeshift);
        InspurPorting_Sleep(timeshift);
    }  
    return NULL;  
} 
