/* Liulan Zheng          *
 * CSCI 340 Home project */

#ifndef MemoryHole_hpp
#define MemoryHole_hpp
class MemoryHole{
    public:
       MemoryHole(unsigned int start, unsigned int end): start_index_(start), end_index_(end), space_( end + 1 - start ) {}
       const unsigned int GetStart() { return start_index_; }
       const unsigned int GetEnd() { return end_index_; }
       const unsigned int GetSpace() { return space_; }
       void SetStart( unsigned int start) {
           start_index_ = start; 
           space_ = end_index_ - start_index_ + 1;
        } 
       void SetEnd( unsigned int end) {
           end_index_ = end; 
           space_ = end_index_ - start_index_ + 1;
        }  
     

    private:
        unsigned int start_index_;
        unsigned int end_index_;
        unsigned int space_;
};
#endif /* MemoryHole_hpp */