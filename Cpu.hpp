/* Liulan Zheng          *
 * CSCI 340 Home project */

#ifndef Cpu_hpp
#define Cpu_hpp
#include "Process.hpp"
#include <memory>
using namespace std;

class Cpu {
    public:
        Cpu() {busy_ = false;}
        const shared_ptr<Process> GetCurrentProcess () { return current_running_process_; }
        const bool IsBusy() { return busy_; }
        void updataBusy(bool && status) {busy_ = status; }
        void SetCurrentProcess (const shared_ptr<Process> p) { current_running_process_ = p; }
    private:
        shared_ptr<Process> current_running_process_;
        bool busy_;
};
#endif /* Cpu_hpp */