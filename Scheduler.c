#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Init.h"

typedef struct
{
    char *Name;
    int ID;
    int TotalTime;
    int WaitTime;
    double Percentage;
    int Burst;
}
ThreadUsage;

ThreadUsage *Table[MAX_THREADS] = {NULL};

int UsageCount = 0;

//Local Scope Declaration
void PrintUsageStat();

void UnShedule(Thread*);

Thread *RoundRobin();

void SaveUsageStat(Thread*);

void PrintUsageStat();

// Definition
void Schedule()
{
    static Thread *Current = NULL;
    Thread *Next;
    UnShedule(Current);
    Next = RoundRobin();
    if(Next == NULL)
    {
        Raise("There are No threads to run \n");
    }
    SwitchToThread(Next);
    Current = Next;
}

void UnShedule(Thread *thread)
{
    if(thread == NULL)
    {
        return;
    }
    else
    {
        switch (thread->state)
        {
        case RUNNING_STATE:
            MakeThreadReady(thread);
            break;
        case DEATH_STATE:
            SaveUsageStat(thread);
            printf("\nStatus: Terminating %s \n",thread->name);
            RemoveThread(thread);
            if(ThreadCount == 1)
            {
                Message("Status: All the thread have been Completed\n\n");
                PrintUsageStat();
            }
            break;
        }
    }
}

Thread *RoundRobin()
{
    int Next = ThreadNext;
    for(int i = 0; i<MAX_THREADS;i++)
    {
        Thread *thread = ThreadQueue[Next];
        Next = Next + 1;
        if(Next >= MAX_THREADS)
        {
            Next = 0;
            
        }
        if(thread && thread->state == READY_STATE)
        {
            ThreadNext = Next;
            return thread;
        }
    }
    return NULL;
}

void SaveUsageStat(Thread *thread)
{
    if(UsageCount == StackSize(Table))
    {
        return;
    }

    ThreadUsage *Usage = (ThreadUsage*)malloc(sizeof(ThreadUsage));
    if(Usage)
    {
        Usage->Name = strdup(thread->name);
        Usage->ID = thread->id;
        Usage->TotalTime = GetCycle() - thread->Stat.create_time;
        Usage->WaitTime = thread->Stat.ready_wait_time;
        Usage->Percentage = (thread->Stat.ready_wait_time * 100)/Usage->TotalTime;
        Usage ->Burst = thread->Stat.num_cpu_bursts;

        Table[UsageCount] = Usage;
        UsageCount = UsageCount + 1;
    }
    else
    {
        Raise("There is no memory to create table\n");
    }
}

void PrintUsageStat()
{
    printf("                               Usage Stat\n");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("+                 Round Robin Implementation    Time Quantum = %d                     +\n",Quanta);
    printf("+=====================================================================================+\n");
	printf("+ThreadName              ID        T.Time     W.Time     Ready/Wait %%       CPU Burst +\n");
    printf("+=====================================================================================+\n");
    for (int i = 0; i < UsageCount; i++)
    {
        ThreadUsage *Usage = Table[i];
        printf("+%-24s%-10d%-11d%-11d%-19lf%-10d+\n",
        Usage->Name,
        Usage->ID,
        Usage->TotalTime,
        Usage->WaitTime,
        Usage->Percentage,
        Usage->Burst);
    }
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("\n");
}