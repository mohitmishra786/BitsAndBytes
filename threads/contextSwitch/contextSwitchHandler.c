#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/* CPU registers structure */
struct cpu_registers {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi;
    uint64_t rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip;
    uint64_t rflags;
    uint16_t cs, ds, es, fs, gs, ss;
    bool fpu_used;
    uint32_t mxcsr;
    uint16_t fpu_cw;
    uint8_t fpu_state[512] __attribute__((aligned(16)));
};

/* Memory management context */
struct mm_context {
    uint64_t pgd;
    uint64_t context_id;
    uint64_t start_code, end_code;
    uint64_t start_data, end_data;
    uint64_t start_brk, brk;
    uint64_t start_stack;
    uint64_t arg_start, arg_end;
    uint64_t env_start, env_end;
};

struct file {
    int fd;
};

struct io_context {
    int nr_files;
    struct file *files[256];
    int cwd_fd;
    uint64_t umask;
};

struct sched_info {
    int policy;
    int priority;
    uint64_t time_slice;
    uint64_t total_time;
    uint64_t start_time;
    uint64_t last_ran;
    int processor;
    uint64_t preempt_count;
};

enum process_state {
    TASK_RUNNING = 0,
    TASK_INTERRUPTIBLE = 1,
    TASK_UNINTERRUPTIBLE = 2,
    TASK_STOPPED = 4,
    TASK_ZOMBIE = 8
};

struct sighand_struct {
    void (*handlers[32])(int);
};

/* Process Control Block structure */
struct pcb {
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    gid_t gid;
    char name[16];  // Added name field for better identification
    
    enum process_state state;
    struct cpu_registers regs;
    struct mm_context mm;
    struct io_context io;
    struct sched_info sched;
    
    struct pcb *parent;
    struct pcb *children;
    struct pcb *siblings;
    
    sigset_t blocked;
    sigset_t pending;
    struct sighand_struct *sighand;
    
    unsigned long flags;
    
    struct rusage rusage;
    struct rusage rusage_children;
    
    uint64_t voluntary_switches;
    uint64_t involuntary_switches;
};

/* Performance statistics structure */
struct perf_stats {
    uint64_t context_switches;
    uint64_t cache_misses;
    uint64_t tlb_misses;
    uint64_t branch_misses;
    uint64_t avg_switch_time;
    uint64_t max_switch_time;
};

#define MAX_PROCESSES 4
static struct perf_stats global_stats = {0};
static struct pcb *current_process = NULL;
static struct pcb processes[MAX_PROCESSES];
static int num_processes = 0;

static inline uint64_t get_current_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

static uint64_t calculate_time_slice(struct pcb *process) {
    // Give each process a smaller time slice (between 1-5 ticks based on priority)
    return 5 - (process->sched.priority % 5);
}

static void save_cpu_state(struct cpu_registers *regs) {
    // Simulate saving CPU state with some dummy values
    regs->rax = rand() % 1000;
    regs->rbx = rand() % 1000;
    regs->rcx = rand() % 1000;
}

static void restore_cpu_state(struct cpu_registers *regs) {
    // Simulate restoring CPU state
    printf("Restoring CPU state: RAX=%lu, RBX=%lu, RCX=%lu\n", 
           regs->rax, regs->rbx, regs->rcx);
}

static void save_fpu_state(struct cpu_registers *regs) {
    if (regs->fpu_used) {
        regs->mxcsr = 0x1F80;
    }
}

static void restore_fpu_state(struct cpu_registers *regs) {
    if (regs->fpu_used) {
        (void)regs;
    }
}

static void switch_mm_context(struct pcb *prev, struct pcb *next) {
    if (prev->mm.pgd != next->mm.pgd) {
        printf("Memory context switch: %s (PGD %lx) -> %s (PGD %lx)\n", 
               prev->name, prev->mm.pgd, next->name, next->mm.pgd);
    }
}

struct pcb* scheduler_select_next(void) {
    static int current_index = 0;
    int starting_index = current_index;
    
    do {
        current_index = (current_index + 1) % num_processes;
        if (processes[current_index].state != TASK_RUNNING) {
            continue;
        }
        return &processes[current_index];
    } while (current_index != starting_index);
    
    return current_process; // Fallback to current if no other process is ready
}

static void update_process_accounting(struct pcb *prev, struct pcb *next) {
    uint64_t current_time = get_current_time();
    prev->sched.total_time += current_time - prev->sched.last_ran;
    next->sched.last_ran = current_time;
    next->sched.time_slice = calculate_time_slice(next);
    
    if (prev->state == TASK_INTERRUPTIBLE) {
        prev->voluntary_switches++;
    } else {
        prev->involuntary_switches++;
    }
}

static void update_perf_stats(uint64_t switch_start_time) {
    uint64_t switch_time = get_current_time() - switch_start_time;
    
    global_stats.context_switches++;
    global_stats.avg_switch_time = 
        (global_stats.avg_switch_time * (global_stats.context_switches - 1) + 
         switch_time) / global_stats.context_switches;
    
    if (switch_time > global_stats.max_switch_time) {
        global_stats.max_switch_time = switch_time;
    }
}

int context_switch(struct pcb *prev, struct pcb *next) {
    uint64_t switch_start_time = get_current_time();
    
    printf("\nContext switch: %s (PID %d) -> %s (PID %d)\n", 
           prev->name, prev->pid, next->name, next->pid);
    
    save_cpu_state(&prev->regs);
    save_fpu_state(&prev->regs);
    
    prev->state = TASK_INTERRUPTIBLE;
    next->state = TASK_RUNNING;
    
    switch_mm_context(prev, next);
    update_process_accounting(prev, next);
    
    restore_fpu_state(&next->regs);
    restore_cpu_state(&next->regs);
    
    current_process = next;
    update_perf_stats(switch_start_time);
    
    return 0;
}

void timer_interrupt_handler(void) {
    struct pcb *prev = current_process;
    struct pcb *next;
    
    printf("\nTimer interrupt: Current process = %s\n", prev->name);
    
    // Force a switch after time slice expires or every few cycles
    if (--prev->sched.time_slice == 0 || (rand() % 3 == 0)) {
        printf("Time slice expired for %s\n", prev->name);
        next = scheduler_select_next();
        
        if (next != prev) {
            context_switch(prev, next);
        } else {
            prev->sched.time_slice = calculate_time_slice(prev);
            printf("No other processes ready, continuing with %s\n", prev->name);
        }
    } else {
        printf("Remaining time slice for %s: %lu\n", 
               prev->name, prev->sched.time_slice);
    }
}

static void initialize_process(struct pcb *proc, const char *name, int priority) {
    proc->pid = getpid() + num_processes;
    proc->ppid = getppid();
    proc->uid = getuid();
    proc->gid = getgid();
    snprintf(proc->name, sizeof(proc->name), "%s", name);
    
    proc->state = TASK_RUNNING;
    proc->sched.priority = priority;
    proc->sched.time_slice = calculate_time_slice(proc);
    proc->sched.last_ran = get_current_time();
    
    // Simulate different memory contexts
    proc->mm.pgd = (uint64_t)rand() << 32 | rand();
    proc->regs.fpu_used = (rand() % 2) == 0;
    
    num_processes++;
}

int main(void) {
    // Adding seed for random number generation
    srand(time(NULL));
    
    printf("Context Switch Simulation Starting\n");
    printf("==================================\n\n");
    
    // Initialize multiple processes with different priorities
    initialize_process(&processes[0], "Init", 2);
    initialize_process(&processes[1], "Web_Server", 1);
    initialize_process(&processes[2], "Database", 0);
    initialize_process(&processes[3], "Backup", 3);
    
    current_process = &processes[0];
    
    printf("Initialized Processes:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("- %s (PID: %d, Priority: %d, Initial time slice: %lu)\n", 
               processes[i].name, processes[i].pid, 
               processes[i].sched.priority,
               processes[i].sched.time_slice);
    }
    printf("\nStarting simulation...\n");
    printf("==================================\n");
    
    // Simulate several timer interrupts
    for (int i = 0; i < 10; i++) {
        printf("\nSimulation Cycle %d\n", i + 1);
        printf("-----------------\n");
        
        timer_interrupt_handler();
        
        printf("\nStatistics after cycle %d:\n", i + 1);
        printf("Context switches: %lu\n", global_stats.context_switches);
        printf("Average switch time: %lu ns\n", global_stats.avg_switch_time);
        printf("Maximum switch time: %lu ns\n", global_stats.max_switch_time);
        
        usleep(100000); // Sleep for 100ms between simulations
    }
    
    printf("\nSimulation Complete\n");
    printf("==================================\n");
    printf("Final Statistics:\n");
    printf("Total context switches: %lu\n", global_stats.context_switches);
    printf("Final average switch time: %lu ns\n", global_stats.avg_switch_time);
    printf("Maximum switch time: %lu ns\n", global_stats.max_switch_time);
    
    return 0;
}