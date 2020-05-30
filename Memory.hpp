/* Liulan Zheng          *
 * CSCI 340 Home project */

#ifndef Memory_hpp
#define Memory_hpp
#include "Process.hpp"
#include "MemoryHole.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <deque>
#include <string>
using namespace std;


class Memory{
    public:
        Memory(unsigned int size): memory_size_ (size) {
            shared_ptr<MemoryHole>memH (new MemoryHole(0, size-1));
            memory_holes_.push_back(memH);
        }
        void DisplayProcesses() {
            cout << "PID \tM_START \t M_END " << endl;
            cout << "-------------------------------- " << endl;
            for(int i = 0; i < processes_.size(); i++) {
                cout << processes_[i].second.getID() << "\t" << processes_[i].first << "\t\t" << processes_[i].second.getSize() + processes_[i].first - 1 << endl;
            }
        }

        // insert new memory hole in memory_holes_
        void InsertMemoryHole ( shared_ptr<MemoryHole> memH) {
            vector< shared_ptr<MemoryHole>> vHoles ( memory_holes_.begin(), memory_holes_.end());
             if(vHoles.size() == 0){
                memory_holes_.push_back(memH);
                return;
            }
            int previous_start = vHoles[0]->GetStart();
            int previous_end = vHoles[0]->GetEnd();
            int count = 0;
            bool inOrder = false;
            if(vHoles.size() == 1){ 
                if(previous_start > memH->GetEnd()) {  
                    if(previous_start ==  memH->GetEnd()+1) { // e.g. memH = {10, 20},  vHoles[0] = {21, 30}
                        // can be combined in to {10, 30}
                        shared_ptr<MemoryHole>m (new MemoryHole(memH->GetStart(), previous_end)); 
                        memory_holes_.pop_front();
                        memory_holes_.push_back(m);
                    }
                    else // e.g. memH = {10, 15},  vHoles[0] = {21, 30}
                        memory_holes_.push_front(memH);
                    return;
                }
                else {
                    if(previous_end +1 == memH->GetStart()) { // e.g. memH = {21, 30},  vHoles[0] = {10, 20}
                        // can be combined in to {10, 30}
                         shared_ptr<MemoryHole> m (new MemoryHole(previous_start, memH->GetEnd()));
                        memory_holes_.pop_front();
                        memory_holes_.push_back(m);
                    }
                    else //e.g. memH = {24, 30},  vHoles[0] = {10, 20}
                        memory_holes_.push_back(memH);
                    return;
                }
            }
            while(count < vHoles.size()) {
                memory_holes_.push_back(memory_holes_.front());
                memory_holes_.pop_front();
                if(!inOrder){
                    if( (memH->GetStart() > previous_end) && (memH->GetEnd() < memory_holes_.front()->GetStart()) ) {// e.g. {B} is in between {A}, {C}
  
                        if( (memH->GetStart() == previous_end +1 ) && (memH->GetEnd()+ 1 == memory_holes_.front()->GetStart()) ) {
                            //e.g. {11, 14} is in between {0, 10}, {15, 20} 
                            //combine 3 memory holes into one e.g. {0, 20}
                            shared_ptr<MemoryHole> m (new MemoryHole(previous_start, memory_holes_.front()->GetEnd())); //{0, 20}
                            memory_holes_.pop_back();
                            memory_holes_.pop_front();
                            memory_holes_.push_back(m);
                            count++; // since size decrease by 2
                        }
                        else if(memH->GetStart() == previous_end +1 ) {
                            //e.g. {11, 13} is in between {0, 10}, {15, 20}
                            //it can be combined into {0, 13}, {15, 20}
                            shared_ptr<MemoryHole> m (new MemoryHole(previous_start, memH->GetEnd())); // {0, 13}
                            memory_holes_.pop_back();
                            memory_holes_.push_back(m);
                        }
                        else if(memH->GetEnd()+ 1 == memory_holes_.front()->GetStart()) {
                            //e.g. {5,8} is in between {0, 3}, {9, 20}
                            //it can be combined into {0,3}, {5, 20}
                            shared_ptr<MemoryHole> m (new MemoryHole(memH->GetStart(), memory_holes_.front()->GetEnd())); //{5, 20}
                            memory_holes_.push_back(m);
                            memory_holes_.pop_front();
                            count++;
                        }
                        else { 
                            // e.g. {5,8} is in between {0, 3}, {10, 20}
                            memory_holes_.push_back(memH);   
                        }
                        inOrder = true;
                    }
                    else if( (memH->GetStart() > previous_end) && (memH->GetEnd() < memH->GetEnd() == memory_size_-1)) {
                        if( (memH->GetStart() ==  previous_end +1)) {// memH = {80, 99}, memory_size_ = 100, previous = {50, 79}
                            // can be combined into {50, 99}
                              shared_ptr<MemoryHole> m ( new MemoryHole(previous_start, memH->GetEnd())); 
                             memory_holes_.pop_back();
                             memory_holes_.push_back(m);
                             count++; 
                        }
                        else{ // memH = {90, 99}, memory_size_ = 100, previous = {50, 79}
                            memory_holes_.push_back(memH);
                        }
                        inOrder = true;
                    }
                }
                count++;
            } 
        }
        void deleteProcess(const shared_ptr<Process> p){
            bool deleted = false;
            vector<pair<int, Process>> vProcesses (processes_.begin(), processes_.end());
            if(vProcesses.size() == 1) {
                processes_.pop_front();
                while(!memory_holes_.empty()){
                    memory_holes_.pop_front();
                }
                shared_ptr<MemoryHole> m (new MemoryHole(0, memory_size_-1));
                memory_holes_.push_back(m);
                return;
            }
            for(int i = 0; i < vProcesses.size(); i++) {
                if(!deleted) {
                    if(*p == processes_.front().second) { // look for Process that needs to be deleted in the list
                        shared_ptr<MemoryHole> memH ( new MemoryHole(vProcesses[i].first, vProcesses[i].second.getSize() + vProcesses[i].first -1 ));
                        processes_.pop_front();
                        InsertMemoryHole (memH);
                        i++; //since size decrease by 1
                        deleted = true;
                    }
                }
                processes_.push_back(processes_.front());
                processes_.pop_front();
            }
        }

        bool addProcess(const Process p){
            vector<shared_ptr<MemoryHole>> vHole ( memory_holes_.begin(), memory_holes_.end());
           
            for(int i = 0; i < vHole.size(); i++) {
                if(vHole[i]->GetSpace() >= p.getSize()) {
                    pair<int, Process> new_process = {vHole[i]->GetStart(), p};     
                    processes_.push_back(new_process);
                    vHole[i]->SetStart(vHole[i]->GetStart() + p.getSize());
                    if(vHole[i]->GetStart() >= memory_size_) // e.g. {100 , 99} start > end, start == memory_size_
                        memory_holes_.pop_back(); // can only happen at the end of the queue
                    return true;
                }
            }
            return false;    
        }

    private:
        deque<pair<unsigned int, Process>> processes_;
        unsigned int memory_size_;
        deque<shared_ptr<MemoryHole>> memory_holes_;

        
};
#endif /* Memory_hpp */