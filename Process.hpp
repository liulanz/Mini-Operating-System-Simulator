/* Liulan Zheng          *
 * CSCI 340 Home project */

#ifndef Process_hpp
#define Process_hpp
class Process{
    public:
        Process(bool isReal, unsigned int size, int id): isReal_(isReal), process_size_(size), pid_(id) {}
        int getID() const{ return pid_; }
        bool getReal() const { return isReal_; }
        unsigned int getSize() const { return process_size_; }
        bool operator==(const Process &rhs) const {
            return (isReal_ == rhs.isReal_ 
                && process_size_ == rhs.process_size_
                && pid_ == rhs.pid_ );
        }
    private:
        bool isReal_;
        unsigned int process_size_;
        int pid_;
};
#endif /* Process_hpp */