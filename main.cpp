/* Liulan Zheng          *
 * CSCI 340 Home project */

#include "Cpu.hpp"
#include "Memory.hpp"
#include <memory> 
#include <iostream>
#include <deque>
#include <sstream>
#include <string>
  
using namespace std;

void DisplayProcesses(shared_ptr<Cpu> &cpu, vector<Process>&& commonP, vector<Process>&& realP) {
    string type, status;
    if(!cpu->IsBusy()){ //if no process is currently using the cpu
        cout << "There's no process that is currently using the CPU" << endl;
        return; 
    }
    cout << endl;
    cout << "PID \t TYPE \t \t STATUS" << endl;
    cout << "-------------------------------- " << endl;

    //print current running process
    status = "Running";
    if(cpu->GetCurrentProcess()->getReal())
        type = "Real time";
    else
        type = "Common\t";
    cout << cpu->GetCurrentProcess()->getID() << "\t" << type << "\t" << status << endl << endl;

    // print all processes that are in ready queues
    type = "Real time";
    status = "Waiting";
    cout << "[[Real-Time Ready Queue]]: " << endl;
    for(int i = 0; i < realP.size(); i++) {
        cout << realP[i].getID() << "\t" << type << "\t" << status << endl;
    }
    cout << "[[Ready Queue]]: " << endl;
    type = "Common\t";
    for(int i = 0; i < commonP.size(); i++) {
        cout << commonP[i].getID() << "\t" << type << "\t" << status << endl;
    }
    cout << endl;
}

void StopCurrentProcess(shared_ptr<Cpu> &cpu, deque<Process> & readyQ, deque<Process> & RealQ) {
    if(!cpu ->IsBusy()) {
        cout << "No process is currently using the cpu. " << endl;
    }
    else {
        if(cpu->GetCurrentProcess()->getReal()) {//if current running process is real time process
            RealQ.push_back(*cpu->GetCurrentProcess());
            const shared_ptr<Process> p = make_shared<Process> (RealQ.front());
            cpu->SetCurrentProcess(p);
            RealQ.pop_front();
        }
        else{
            readyQ.push_back(*cpu->GetCurrentProcess());
            const shared_ptr<Process> p = make_shared<Process> (readyQ.front());
            cpu->SetCurrentProcess(p);
            readyQ.pop_front();
        }
    }
}
/** @pre: there's a process that is currently using the CPU 
 *  this function is called when current running process needs to use hard disk
**/
void CpuNextProcess(shared_ptr<Cpu> &cpu,  deque<Process>  & readyQ,  deque<Process> & RealQ) {
    if(cpu->GetCurrentProcess()->getReal()) {//if current running process is real time process
        if(!RealQ.empty()) {//let next real time process uses the cpu if there's one else let a common process uses the cpu
            const shared_ptr<Process> p = make_shared<Process> (RealQ.front());
            cpu->SetCurrentProcess(p);
            RealQ.pop_front();
        }
        else if (!readyQ.empty()) {
            const shared_ptr<Process> p = make_shared<Process> (readyQ.front());
            cpu->SetCurrentProcess(p);
            readyQ.pop_front();
        }
        else // else when both queues are empty, that means there's no current running process
            cpu -> updataBusy(false);
    }
    else { //if current running process is common process
        if(!readyQ.empty()) 
        {
            const shared_ptr<Process> p = make_shared<Process> (readyQ.front());
            cpu->SetCurrentProcess(p);
            readyQ.pop_front();
        }
         else // else when both queues are empty, that means there's no current running process
            cpu -> updataBusy(false);
    }
}
void DisplayHardDisk(vector<pair<int, deque<Process>>> hard_disk, int hdd_size) {
    cout << endl;
    cout << "Hard Disk      PID \t Status" << endl;
    cout << "-------------------------------- " << endl;
    string status;
    for(int i = 0; i < hdd_size; i++) {
        deque<Process> q = hard_disk[i].second;
        for (int k = 0; k < q.size(); k++){
            if(k == 0) {
                status = "Using HDD";
            }
            cout << "HDD #" << hard_disk[i].first << "\t\t" << q[k].getID()<< "\t" << status << endl;
            status = "In I/O Queue";
        }
    }
    cout << endl;
}

void UseCpu( deque<Process> & RealQ,  deque<Process> & readyQ, shared_ptr<Cpu> cpu)
{
    if(!cpu->IsBusy()) { // cpu is free 
        if(!RealQ.empty()){  // if there's real time process in ready queue, let it uses the cpu
            const shared_ptr<Process> p = make_shared<Process> (RealQ.front());
            cpu->SetCurrentProcess(p);
            RealQ.pop_front(); // process leaves ready queue because it's using cpu right now
            cpu -> updataBusy(true);
        }
        else if(!readyQ.empty()){  //let common process use cpu when there's no real time process waiting
            const shared_ptr<Process> p = make_shared<Process> (readyQ.front());
            cpu->SetCurrentProcess(p);
            readyQ.pop_front();
            cpu -> updataBusy(true); 
        }
        // else no process in ready queues, cpu remains free
    }
    else if(!(cpu->GetCurrentProcess() -> getReal()) && !RealQ.empty()) {// if there's real time process in ready queue, and current running process is common process){
        readyQ.push_front(*cpu->GetCurrentProcess());  // common process gets preempted and sends to the head of the common processes ready-queue
        const shared_ptr<Process> p = make_shared<Process> (RealQ.front());
        cpu->SetCurrentProcess(p);
        RealQ.pop_front(); // process leaves ready queue because it's using cpu right now
    }   
}

bool AddProcess(deque<Process> & ready_queue, const Process p, Memory& ram) {
    if(ram.addProcess(p)) {
        ready_queue.push_back(p);
        return true;
    }
    else{
        cout << "Not enough memory in RAM!" << endl;
        return false;
    }
}
int main() {
    
    unsigned int ram_size, hdd_num, number;
    int id = 1;
    bool real_time = true;
    string input_command;
    deque<Process> ready_queue;
    deque<Process> real_ready_queue;
    
    shared_ptr<Cpu> cpu (new Cpu);
    cout << "How much RAM (in bytes) should the simulated computer use? " << endl;
    cin >> ram_size;
    Memory ram = Memory (ram_size);
    
    cout << "How many HDDs should the simulated computer use? " << endl;
    cin >> hdd_num;
    vector<pair<int, deque<Process>>> hard_disk;
    for(int i = 0; i < hdd_num; i++) {
        hard_disk.push_back( {i, {} } );
    }
    
    cout << "You can start enter the commands: " << endl;
    cout << ">>> Enter [q] : to quit " << endl;
   
    while ( cin >> input_command )
    {
        if (input_command == "q")
            break;
        else if (input_command == "t") { //terminates currently running process
             if(!cpu->IsBusy())
                cout << "No process is currently using the cpu. " << endl;
            else {
                ram.deleteProcess(cpu->GetCurrentProcess());
                // current process gets removed from cpu and next process uses cpu
                CpuNextProcess(cpu, ready_queue, real_ready_queue);
            } 
        }
        else if (input_command == "Q") { //time slice has ended for the currently running process
     
            StopCurrentProcess(cpu, ready_queue, real_ready_queue);
        }
        else if(input_command == "AR") { //real process
            cin >> number;
            const Process p = Process(real_time, number, id);
            if(AddProcess(real_ready_queue, p, ram)){
                // Make sure current running process is real time process
                UseCpu(real_ready_queue, ready_queue, cpu);   
                id++;
            }
        }
        else if (input_command == "A" ) {  //common process
            cin >> input_command;
            if(!isdigit(input_command[0])) { // Check for any invalid command such as "A R"  
                getline(cin, input_command);
                cout << "Invalid command. Please re-enter your command." << endl;
            }
            else {
                stringstream ss (input_command);
                ss >> number;
                const Process p = Process(!real_time, number, id);
                if(AddProcess(ready_queue, p, ram)){
                    UseCpu(real_ready_queue, ready_queue, cpu); 
                    id++; 
                }
            } 
        }        
        else if (input_command== "d") { //The process that currently uses the CPU requests the hard disk #number.
            cin >> number;
            if (number >= hdd_num || number < 0)
                cout << "Invalid hard disk number!" << endl;
            else {
                if(!cpu->IsBusy())
                    cout << "No process is currently in the system (i.e. no process is using the cpu right now). " << endl;
                else {
                    hard_disk[number].second.push_back(*(cpu->GetCurrentProcess())); //current running process goes to I/O queue
                    // current process gets removed from cpu and next process uses cpu
                    CpuNextProcess(cpu, ready_queue, real_ready_queue);
                }
            }
        }
        else if (input_command== "D") { // The hard disk #number has finished the work for one process.
            cin >> number;
            if (number >= hdd_num || number < 0)
                cout << "Invalid hard disk number!" << endl;
            else {
                if(!hard_disk[number].second.empty())
                {
                    if(hard_disk[number].second.front().getReal()) {//if the process is real
                        real_ready_queue.push_back(hard_disk[number].second.front());
                        hard_disk[number].second.pop_front();
                        // Make sure current running process is real time process
                        UseCpu( real_ready_queue ,  ready_queue, cpu);  
                    }
                    else {
                        ready_queue.push_back(hard_disk[number].second.front());
                        hard_disk[number].second.pop_front();
                    }
                }
                // else this hard disk is free right now (i.e. this hard disk is not serving reading/writing request right now)
            }     
        }
        else if (input_command== "S") {
            cin >> input_command;
            if (input_command == "r") 
                DisplayProcesses(cpu, {ready_queue.begin(), ready_queue.end()}, {real_ready_queue.begin(), real_ready_queue.end()});
            else if (input_command== "i") {
                DisplayHardDisk(hard_disk, hdd_num);
            }
            else if(input_command== "m") {
                ram.DisplayProcesses();
            }
            else {
                cout << "Invalid command. Please re-enter your command." << endl;
            }     
        }
        else {  
            getline(cin, input_command);
            cout << "Invalid command. Please re-enter your command." << endl;
        }    
    }
    return 0;
}