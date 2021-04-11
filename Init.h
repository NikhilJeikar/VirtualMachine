#define MAX_THREADS  20
#define StackSize(E) (sizeof(E) / sizeof(E[0]))
#define MAX_INTERRUPTS  7

extern int ThreadCount;
extern int ThreadNext;
extern int NextThreadID;

extern int Quanta;

typedef void (*interrupt_handler) ();
typedef long int big;
typedef int op;


enum interrupts
{
	intNone = -1,
	intTimer,
    intIO,
	intFatalError,
	intInvalidOpcode,
	intDivideByZero,
	intStackOverflow,
	intStackUnderflow
};

enum opcodes
{
	opHalt = -9999,
	opNop,
	opInc,
	opDec,
	opEmit,
	opStartStr,
	opPrintStr,
	opLoop,
	opEndLoop,
	opStat
};

enum thread_states
{
	BIRTH_STATE,          
	READY_STATE,          
	RUNNING_STATE,        
	DEATH_STATE           
};


typedef struct
{
	int create_time;      
	int ready_start_time; 
	int ready_wait_time;  
	int num_cpu_bursts;   
}
ThreadStat;

typedef struct
{
	char *name;           
	int   id;             
	
	int   state;          

	op   *code;           
	int   ip;             
	
	op    stack[50];      
	int   sp;             
	
	ThreadStat Stat;       
}
Thread;

extern Thread *ThreadQueue[MAX_THREADS];

// Global Scope Function
//Supporters
void Raise(char*);
void Message(char*);

//Interrupts
void EnableInterrupt();
void DisableInterrupt();
interrupt_handler SetHandler(int,interrupt_handler);
void SetInterrupt(int);
int GetInterrupt();


//Machine
void MachineInit();
big GetCycle();
void machine();
void ContextSwitching(Thread*);

//Thread
void ThreadInit();
void MakeThreadReady(Thread*);
void RemoveThread(Thread*);
void SwitchToThread(Thread*);
void ThreadDetails();

//Scheduler
void Schedule();