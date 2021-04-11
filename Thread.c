#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Init.h"

int ThreadCount = 0;
int ThreadNext = 0;
int NextThreadID = 0;

Thread *ThreadQueue[MAX_THREADS] = {NULL};

// Local Scope declaration
Thread *CreateThread(op *Code,char * Name);

void InsertThread(Thread *thread);

char *DecodeStatus(int);

// Definition
void ThreadInit()
{
    static op Idle[] = {1,opLoop,opInc,opEndLoop,opHalt};
    static op prog1[] = {opStartStr,'\n','1','-','d','a','e','r','h','T',' ',opPrintStr,'9',opEmit, opHalt}; 
	static op prog2[] = {23, opLoop, '2', opEmit, opEndLoop, opHalt}; 
	static op prog3[] = {18, opLoop, '3', opEmit, opEndLoop, opHalt};
    
    InsertThread(CreateThread(Idle,"Idle"));
    InsertThread(CreateThread(prog1,"Thread 1"));
    InsertThread(CreateThread(prog2,"Thread 2"));
    InsertThread(CreateThread(prog3,"Thread 3"));
    
    ThreadDetails();
    ThreadNext = 0;
}

Thread *CreateThread(op *Code,char * Name)
{
    Thread *New = (Thread*) malloc (sizeof(Thread));
    if(New != NULL)
    {
        New->name = strdup(Name);
        New->id = NextThreadID;
        New->state = BIRTH_STATE;
        New->code = Code;
        New->ip = 0;
        New->sp = 0;

        New->Stat.create_time = GetCycle();
        New->Stat.ready_start_time = 0;
        New->Stat.ready_wait_time = 0;
        New->Stat.num_cpu_bursts = 0;
        NextThreadID = NextThreadID + 1 ;
        return New;
    }
    else
    {
        Raise("There is no memory to create thread\n");
    }
}

void MakeThreadReady(Thread *thread)
{
    thread->Stat.ready_start_time = GetCycle();
    thread->state = READY_STATE;
}

void SwitchToThread(Thread *thread)
{
    thread->Stat.ready_wait_time = thread->Stat.ready_wait_time + (GetCycle() - thread->Stat.ready_start_time);
    thread->state = RUNNING_STATE;
    ContextSwitching(thread);
    thread->Stat.num_cpu_bursts = thread->Stat.num_cpu_bursts + 1;
}

void InsertThread(Thread *thread)
{
    if(ThreadCount == MAX_THREADS)
    {
        Raise("Sorry all the threads are being used\n");
    }
    MakeThreadReady(thread);
    ThreadQueue[ThreadNext] = thread;
    ThreadNext = ThreadNext + 1;
    if(ThreadNext >= MAX_THREADS)
    {
        ThreadNext = 0;
    }
    ThreadCount = ThreadCount + 1;
}

void RemoveThread(Thread *thread)
{
    int i;
    for(i = 0; i<MAX_THREADS;i++)
    {
        Thread *th = ThreadQueue[i];
        if(th && (thread->id==th->id))
        {
            ThreadQueue[i] = NULL;
            ThreadCount = ThreadCount - 1;
            break;
        }
    }
}

char *DecodeStatus(int ID)
{
    switch (ID)
    {
        case BIRTH_STATE:
            return "BIRTH";
        case READY_STATE:
            return "Ready";
        case RUNNING_STATE:
            return "Running";
        case DEATH_STATE:
            return "Died";

        default:
            return "None";
    }
}

void ThreadDetails()
{
    printf("\n");
    printf("\n                                                                  Thread initilization table\n");
    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("+ Thread Name         ID     State     Code                Instruction Pointer     Stack               Stack Pointer");
    printf("       C.Time    S.Time    W.Time    Burst     +\n");
    printf("+==================================================================================================================================================================+\n");
    for(int i =0; i<MAX_THREADS;i++)
    {
        if(ThreadQueue[i] != NULL)
        {
            printf("+ %-20s%-7d%-10s%-20p%-24d%-20p%-20d",ThreadQueue[i]->name,ThreadQueue[i]->id,DecodeStatus(ThreadQueue[i]->state),ThreadQueue[i]->code,ThreadQueue[i]->ip,ThreadQueue[i]->stack,ThreadQueue[i]->sp);
            printf("%-10d%-10d%-10d%-9d +\n",ThreadQueue[i]->Stat.create_time,ThreadQueue[i]->Stat.ready_start_time,ThreadQueue[i]->Stat.ready_wait_time,ThreadQueue[i]->Stat.num_cpu_bursts);
        }
    }

    printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("\n");
}
