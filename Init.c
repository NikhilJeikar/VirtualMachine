#include <stdio.h>
#include <stdlib.h>

#include "Init.h"

int Quanta = 4;

//Local Scope Declaration
void TimerInterrupt();
void Init();


void Raise(char *Msg)
{
    printf("\nError: %s\n",Msg);
    exit(1);
}

void Message(char *Msg)
{
    printf("%s",Msg);
}

void TimerInterrupt()
{
    DisableInterrupt();
    Schedule();
    EnableInterrupt();
}

void Init()
{
    SetHandler(intTimer,TimerInterrupt);
    ThreadInit();
    Message("Status: Executing Threads\n");
    Schedule();
}

void main()
{
    printf("Enter the time quantum: ");
    scanf("%d",&Quanta);
    printf("\n");
    MachineInit();
    Init();
    machine();
}
