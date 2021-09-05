/******************************************************************************/
/*!
\file   ObjectAllocator.h
\author Junki Kim
\par    email: junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #1
\date   1/26/2019
\brief  
  This is the interface file for ObjectAllocator
  
*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef OBJECTALLOCATORH
#define OBJECTALLOCATORH
//---------------------------------------------------------------------------

#include <string>
#include <iostream>

// If the client doesn't specify these:
static const int DEFAULT_OBJECTS_PER_PAGE = 4;
static const int DEFAULT_MAX_PAGES = 3;

class OAException
{
public:
  // Possible exception codes
  enum OA_EXCEPTION
  {
    E_NO_MEMORY,      // out of physical memory (operator new fails)
    E_NO_PAGES,       // out of logical memory (max pages has been reached)
    E_BAD_BOUNDARY,   // block address is on a page, but not on any block-boundary
    E_MULTIPLE_FREE,  // block has already been freed
    E_CORRUPTED_BLOCK // block has been corrupted (pad bytes have been overwritten)
  };

  OAException(OA_EXCEPTION ErrCode, const std::string& Message) : error_code_(ErrCode), message_(Message) {};

  virtual ~OAException() {
  }

  OA_EXCEPTION code(void) const {
    return error_code_;
  }

  virtual const char *what(void) const {
    return message_.c_str();
  }
private:
  OA_EXCEPTION error_code_;
  std::string message_;
};

// ObjectAllocator configuration parameters
struct OAConfig
{
  static const size_t BASIC_HEADER_SIZE = sizeof(unsigned) + 1; // allocation number + flags
  static const size_t EXTERNAL_HEADER_SIZE = sizeof(void*);     // just a pointer

  enum HBLOCK_TYPE { hbNone, hbBasic, hbExtended, hbExternal };
  struct HeaderBlockInfo
  {
    HBLOCK_TYPE type_;
    size_t size_;
    size_t additional_;
    HeaderBlockInfo(HBLOCK_TYPE type = hbNone, unsigned additional = 0) : type_(type), size_(0), additional_(additional)
    {
      if (type_ == hbBasic)
        size_ = BASIC_HEADER_SIZE;
      else if (type_ == hbExtended) // alloc # + use counter + flag byte + user-defined
        size_ = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) + additional_;
      else if (type_ == hbExternal)
        size_ = EXTERNAL_HEADER_SIZE;
    };
  };

  // OAConfig config(newdel, 4, 2, debug, padbytes, header, alignment);
  OAConfig(bool UseCPPMemManager = false,
    unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE,
    unsigned MaxPages = DEFAULT_MAX_PAGES,
    bool DebugOn = false,
    unsigned PadBytes = 0,
    const HeaderBlockInfo &HBInfo = HeaderBlockInfo(),
    unsigned Alignment = 0) : 
    UseCPPMemManager_(UseCPPMemManager),
    ObjectsPerPage_(ObjectsPerPage),
    MaxPages_(MaxPages),
    DebugOn_(DebugOn),
    PadBytes_(PadBytes),
    HBlockInfo_(HBInfo),
    Alignment_(Alignment)
  {
    HBlockInfo_ = HBInfo;
    LeftAlignSize_ = 0;
    InterAlignSize_ = 0;
  }

  bool UseCPPMemManager_;   // by-pass the functionality of the OA and use new/delete
  unsigned ObjectsPerPage_; // number of objects on each page
  unsigned MaxPages_;       // maximum number of pages the OA can allocate (0=unlimited)
  bool DebugOn_;            // enable/disable debugging code (signatures, checks, etc.)
  unsigned PadBytes_;          // size of the left/right padding for each block
  HeaderBlockInfo HBlockInfo_; // size of the header for each block (0=no headers)
  unsigned Alignment_;      // address alignment of each block

  unsigned LeftAlignSize_;  // number of alignment bytes required to align first block
  unsigned InterAlignSize_; // number of alignment bytes required between remaining blocks
};

// ObjectAllocator statistical info
struct OAStats
{
  OAStats(void) : ObjectSize_(0), PageSize_(0), FreeObjects_(0), ObjectsInUse_(0), PagesInUse_(0),
                  MostObjects_(0), Allocations_(0), Deallocations_(0) {};

  size_t ObjectSize_;      // size of each object
  size_t PageSize_;        // size of a page including all headers, padding, etc.
  unsigned FreeObjects_;   // number of objects on the free list
  unsigned ObjectsInUse_;  // number of objects in use by client
  unsigned PagesInUse_;    // number of pages allocated
  unsigned MostObjects_;   // most objects in use by client at one time
  unsigned Allocations_;   // total requests to allocate memory
  unsigned Deallocations_; // total requests to free memory
};

// This allows us to easily treat raw objects as nodes in a linked list
struct GenericObject
{
  GenericObject *Next;
};

struct MemBlockInfo
{
  bool in_use;        // Is the block free or in use?
  char *label;        // A dynamically allocated NUL-terminated string
  unsigned alloc_num; // The allocation number (count) of this block
};

// This memory manager class 
class ObjectAllocator
{
public:
  // Defined by the client (pointer to a block, size of block)
  typedef void(*DUMPCALLBACK)(const void *, size_t);
  typedef void(*VALIDATECALLBACK)(const void *, size_t);

  // Predefined values for memory signatures
  static const unsigned char UNALLOCATED_PATTERN = 0xAA;
  static const unsigned char ALLOCATED_PATTERN = 0xBB;
  static const unsigned char FREED_PATTERN = 0xCC;
  static const unsigned char PAD_PATTERN = 0xDD;
  static const unsigned char ALIGN_PATTERN = 0xEE;

  // Creates the ObjectManager per the specified values
  // Throws an exception if the construction fails. (Memory allocation problem)
  ObjectAllocator(size_t ObjectSize, const OAConfig& config);

  // Destroys the ObjectManager (never throws)
  ~ObjectAllocator();

  // Take an object from the free list and give it to the client (simulates new)
  // Throws an exception if the object can't be allocated. (Memory allocation problem)
  void *Allocate(const char *label = 0);

  // Returns an object to the free list for the client (simulates delete)
  // Throws an exception if the the object can't be freed. (Invalid object)
  void Free(void *Object);

  // Calls the callback fn for each block still in use
  unsigned DumpMemoryInUse(DUMPCALLBACK fn) const;

  // Calls the callback fn for each block that is potentially corrupted
  unsigned ValidatePages(VALIDATECALLBACK fn) const;

  // Frees all empty pages (extra credit)
  unsigned FreeEmptyPages(void);

  // Returns true if FreeEmptyPages and alignments are implemented
  static bool ImplementedExtraCredit(void);

  // Testing/Debugging/Statistic methods
  void SetDebugState(bool State);       // true=enable, false=disable
  const void *GetFreeList(void) const;  // returns a pointer to the internal free list
  const void *GetPageList(void) const;  // returns a pointer to the internal page list
  OAConfig GetConfig(void) const;       // returns the configuration parameters
  OAStats GetStats(void) const;         // returns the statistics for the allocator

private:
  // Some "suggested" members (only a suggestion!)
  GenericObject *PageList_;           // the beginning of the list of pages
  GenericObject *FreeList_;           // the beginning of the list of objects
  void allocate_new_page(void);       // allocates another page of objects
  void put_on_freelist(void *Object); // puts Object onto the free list
  
  size_t ObjectSize_;
  size_t PageSize_;
  unsigned PadBytes_;
  unsigned ObjectsPerPage_;
  unsigned MaxPages_;
  unsigned Alignment_;
  unsigned LeftAlignSize_;  // number of alignment bytes required to align first block
  unsigned InterAlignSize_; // number of alignment bytes required between remaining blocks
  OAConfig::HeaderBlockInfo HBlockInfo_; // size of the header for each block (0=no headers)
  unsigned PagesInUse_{};    // number of pages allocated
  unsigned ObjectsInUse_{};  // number of objects in use by client
  unsigned FreeObjects_{};   // number of objects on the free list
  unsigned Allocations_{};   // total requests to allocate memory
  unsigned Deallocations_{}; // total requests to free memory
  unsigned MostObjects_{};   // most objects in use by client at one time
  bool UseCPPMemManager_;
  bool DebugOn_;
  
  void* tempobject;
  
    // Make private to prevent copy construction and assignment
  ObjectAllocator(const ObjectAllocator &oa);
  ObjectAllocator &operator=(const ObjectAllocator &oa);
  
  bool CheckFreeList(GenericObject* object) const; // check OAException E_MULTIPLE_FREE
  bool CheckBadBoundary(GenericObject* object) const; // check OAException E_BAD_BOUNDARY
  bool CheckCorrupted(void* object) const; // check OAException E_CORRUPTED_BLOCK
  void* CPPAllocate(); // use new/delete allocation
  void CPPFree(GenericObject* object); // use new/delete free
};

#endif
