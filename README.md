# OS Project: Mini Operating System Simulator

## Project Overview
This project simulates a mini operating system kernel in C, focusing on process scheduling, memory management, and inter-process communication (IPC). The system is modular, with each component running as a separate process and communicating via IPC mechanisms.

## Architecture
- **Process Generator**: Reads process data, spawns user processes at their arrival times, and manages the clock and scheduler processes.
- **Scheduler**: Receives process information, schedules them using algorithms (Round Robin, HPF, SRTN), and controls process execution via signals.
- **Clock**: Provides a global time reference using shared memory.
- **User Processes**: Simulate execution, respond to scheduler signals, and notify the scheduler upon completion.
- **Memory Manager**: Implements a buddy memory allocation system for dynamic memory management.

### IPC Mechanisms Used
- **Message Queues**: For sending process control blocks (PCBs) and control messages between the process generator and scheduler.
- **Shared Memory**: For clock synchronization across all processes.
- **Unix Signals**: For process control (start, stop, resume, finish) between the scheduler and user processes.

## Process Lifecycle & Communication
1. **Process Generator** reads the input file and spawns user processes at their specified arrival times.
2. **PCBs** are sent to the scheduler via message queues.
3. **Scheduler** selects which process to run based on the chosen algorithm and uses signals to start/stop/resume processes.
4. **User Processes** simulate their runtime, sending signals back to the scheduler on each tick and upon completion.
5. **Clock** process maintains a shared time reference for all components.
6. **Memory Manager** allocates and deallocates memory for processes using the buddy system.

## How to Use

### Prerequisites
- Linux OS
- GCC compiler

### Build
```bash
cd kernel-sim-sudon-t/src
make clean
make
```

### Input File Format
```
#id arrival runtime priority memory_size
1   0      10      5        64
2   2      5       3        32
3   4      8       1        128
```

### Run
- **Round Robin**: `./bin/os-sim -s rr -q 2 -f processes.txt`
- **HPF**: `./bin/os-sim -s hpf -f processes.txt`
- **SRTN**: `./bin/os-sim -s srtn -f processes.txt`

### Output
- `execution_log.txt`: Timeline of process execution and idle periods
- `scheduler.log`: State transitions and scheduling decisions
- `memory.log`: Memory allocation/deallocation events
- `scheduler.perf`: Performance statistics

### Troubleshooting
- If you encounter IPC resource errors, use `ipcs` and `ipcrm` to clean up message queues and shared memory.
- Use `make clean` to remove old binaries and object files.

## Summary
This project demonstrates the use of IPC (message queues, shared memory, signals) to coordinate multiple processes in a simulated OS environment, with a focus on modularity, extendability, and educational value for operating systems concepts.
