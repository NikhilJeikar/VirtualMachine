#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>

#include "Init.h"


static interrupt_handler InteruptVectorTable[MAX_INTERRUPTS];
static int InteruptQueue[MAX_INTERRUPTS];

static Thread *CurrentThread;

static int IQHead;
static int IQRear;
static int IQCount;

static bool InterruptState = true;
static big ClockCycle = 0;

//Local Scope Declaration
void Fatal();
void Pass();

void SetInterrupt(int Interrupt);
int GetInterrupt();

int Clock();
op NextOP();
void Interpreter(op opCode);

void Push(op OPCode);
op Pop();
void PrintChar(char Char);
int EndLoop(op *code);


// Definition
// Handlers
void Pass()
{

}

void Fatal()
{
    printf("Machine has been killed -_-\n");
    exit(1);
}

// Interrupt
void EnableInterrupt()
{
 InterruptState = true;   
}

void DisableInterrupt()
{
    InterruptState = false;
}

interrupt_handler SetHandler(int index,interrupt_handler new_handler)
{
	if (index >= 0 && index < MAX_INTERRUPTS)
	{
	
		interrupt_handler old_handler = InteruptVectorTable[index];
		InteruptVectorTable[index] = new_handler;
		
		return old_handler;
		
	}
	
	return NULL;
}

void SetInterrupt(int Interrupt)
{
    if(IQCount >= MAX_INTERRUPTS)
    {
       
    }
    else
    {
        IQRear = IQRear + 1;
        if(IQRear >= MAX_INTERRUPTS )
        {
            IQRear = 0;
        }
        InteruptQueue[IQRear] = Interrupt;
        IQCount = IQCount + 1;
    }
}

int GetInterrupt()
{
    if(IQCount == 0)
    {
        return intNone;
    }
    else
    {
        int ret;
        ret = InteruptQueue[IQHead];
        IQCount = IQCount - 1;
        IQHead = IQHead + 1;
        if(IQHead >= MAX_INTERRUPTS)
        {
            IQHead = 0;
        }
        return ret;
    }
}

// Machine
void MachineInit()
{
    for (int i = 0; i < MAX_INTERRUPTS; i++)
    {
        InteruptVectorTable[i] = Pass;
    }
    InteruptVectorTable[intFatalError] =  Fatal;

    IQRear = -1;
    IQHead = IQCount = 0;
}

big GetCycle()
{
    return ClockCycle;
}

int Clock()
{
    static int QuantaCycle = 0;
    ClockCycle = ClockCycle + 1;
    QuantaCycle = QuantaCycle + 1;
    if(QuantaCycle == Quanta)
    {
        QuantaCycle = 0;
        SetInterrupt(intTimer);
    }
}

void machine()
{
    while (1)
    {
        int i = GetInterrupt();
        Clock();
        if (i>=0 && i<MAX_INTERRUPTS)
        {
            if (InterruptState)
            {
                InteruptVectorTable[i]();

                Clock();
            }
        }    
        Interpreter(NextOP());
        Clock();    
    }   

}

void ContextSwitching(Thread *thread)
{
    CurrentThread = thread;
}

op NextOP()
{
    op Next = opNop;
    static int i = 0;
    if(CurrentThread)
    {
        Next = CurrentThread->code[CurrentThread->ip];
        if (Next != opHalt)
        {
            CurrentThread->ip = CurrentThread->ip + 1;
        }
    }
    return Next;
}

void Interpreter(op opCode)
{
    //printf("%d\n",opCode);
    Thread *Current = CurrentThread;    
    if(!Current)
    {
        SetInterrupt(intFatalError);
    }
    if(opCode > 0)
    {
     Push(opCode);
    }
    else
    {
        int LoopIndex,StrStart;
        switch (opCode)
        {
        case opNop:
            break;
        
        case opInc:
            Push(Pop() + 1);
            break;

        case opDec:
            Push(Pop() - 1);
            break;

        case opEmit:
            PrintChar((char)Pop());
            break;

        case opLoop:
            LoopIndex = Pop();
            if(LoopIndex > 0)
            {
                LoopIndex = LoopIndex - 1;
                int LoopIP = Current->ip - 1;
                Push(LoopIP);
                Push(LoopIndex);
            }
            else
            {
                Current->ip = Current->ip + EndLoop(&Current->code[Current->ip]);
            }
            break;

        case opEndLoop:
            LoopIndex = Pop();
            Current->ip = Pop();
            Push(LoopIndex);
            break;

        case opHalt:
            Current->state = DEATH_STATE;
            SetInterrupt(intTimer);
            break;

        case opStartStr:
            Push(opStartStr);
            break;

        case opPrintStr:
            while (Current->stack[Current->sp  - 1]!= opStartStr)
            {
                PrintChar((char)Pop());
            }
            Pop();
            break;
        
        case opStat:
            ThreadDetails();
            break;
        default:
            SetInterrupt(intInvalidOpcode);
            break;
        }
    }
}

// Custom Op
void Push(op OPCode)
{
    Thread *Current = CurrentThread;

    if(CurrentThread->sp < StackSize(CurrentThread->stack))
    {
        CurrentThread->stack[CurrentThread->sp++] = OPCode;
    }
    else
    {
        SetInterrupt(intStackOverflow);
    }

}

op Pop()
{
    Thread *Current = CurrentThread;
    op ret = 0;
    if(Current->sp >0)
    {
        ret = Current->stack[--Current->sp];
    }
    else
    {
        SetInterrupt(intStackUnderflow);
    }
    return ret;
}

void PrintChar(char Char)
{
    printf("%c",Char);
}

int EndLoop(op *code)
{
    op Cur;
    int SkipLength = 0;
    int Depth = 1;
    Cur = *code;
    while (Depth && Cur != opHalt)
    {
        SkipLength = SkipLength + 1;
        Cur = code[SkipLength];
        if(Cur == opLoop)
        {
            Depth = Depth + 1;
        }
        if(Cur == opEndLoop)
        {
            Depth = Depth - 1;
        }
        if(Depth == 0)
        {
            break;
        }
    }
    SkipLength = SkipLength + 1;
    if (Depth > 0)
    {
        SetInterrupt(intFatalError); 
    }
    
    return SkipLength;
}
