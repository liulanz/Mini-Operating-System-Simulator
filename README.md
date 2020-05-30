# Mini-Operating-System-Simulator

## To run this program

```bash
g++ -o simulator main.cpp
./simulator
```
**While the program is running**

1. You will enter a number for how big is the RAM size on this simulated computer
1. You will enter a number for how many hard disk drivers on this simulated computer
1. You can now start enter commands as listed below
- `q` : quit the program
- `A [num]` : A new common process has been created with num bytes of memory (e.g. `A 100`)
- `AR [num]` : A new real time process has been created with num bytes of memory (e.g. `AR 100`)
- `Q` : the time slice has ended for the currently running process
- `t` : currently running process terminates
- `d [num]` : the process that currently uses the CPU requests the hard disk #number (e.g. `d 0`)
- `D [num]` : the hard disk #number has finished the work for one process (e.g. `D 0`)
- `S r` : shows what process is currently using the CPU and what processes are waiting on both levels of the ready-queue.
- `S i` :  shows what processes are currently using the hard disks and what processes are waiting to use them. 
- `S m` : shows the state of memory

**Some rules for this simulator followed:** 
1. If a common process is currently using the CPU and a real process arrives, then the common process gets preemptied and sent to the head of the common processes ready-queue.
1. Processes' PIDs starts with 1
1. Hard disk number starts with 0
1. Memory holes are implemented

