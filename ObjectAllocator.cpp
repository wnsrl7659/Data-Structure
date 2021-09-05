/******************************************************************************/
/*!
\file   ObjectAllocator.cpp
\author Junki Kim
\par    email: junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #1
\date   1/26/2019
\brief  
  This is the implementation file for ObjectAllocator,
  as specified in specification file ObjectAllocator.h.  
*/
/******************************************************************************/
#include <cstring>
#include "ObjectAllocator.h"

/******************************************************************************/
/*!

  class ObjectAllocator's constructor

*/
/******************************************************************************/
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config)
{
  // initializing value 
  PageList_ = nullptr;
  FreeList_ = nullptr;

  ObjectSize_ = ObjectSize;
  //PageSize_ = config.ObjectsPerPage_;
  PageSize_ = config.ObjectsPerPage_ * ObjectSize + sizeof(void*)
              + (config.ObjectsPerPage_ * 2 * config.PadBytes_)
              + (config.ObjectsPerPage_ * config.HBlockInfo_.size_);
  PadBytes_ = config.PadBytes_;
  ObjectsPerPage_ = config.ObjectsPerPage_;
  MaxPages_ = config.MaxPages_;
  Alignment_ = config.Alignment_;
  LeftAlignSize_ = config.LeftAlignSize_;
  InterAlignSize_ = config.InterAlignSize_;
  HBlockInfo_ = config.HBlockInfo_;
  UseCPPMemManager_ = config.UseCPPMemManager_;
  DebugOn_ = false;
  
  allocate_new_page();

  // check allocation
  if (!PageList_)
  {
      throw OAException(OAException::E_NO_MEMORY,
      "allocation failed E_NO_MEMORY");
  }
}

/******************************************************************************/
/*!

  class ObjectAllocator's destructor

*/
/******************************************************************************/
ObjectAllocator::~ObjectAllocator()
{
  GenericObject* temppage;
  GenericObject* nextpage;

  temppage = PageList_;
  nextpage = nullptr;

  // delete every page
  while (temppage)
  {
    nextpage = temppage->Next;

    delete[] temppage;

    temppage = nextpage;
  }
}

// list of own private function

/******************************************************************************/
/*!

  check object(parm) is already on the freelist

*/
/******************************************************************************/
bool ObjectAllocator::CheckFreeList(GenericObject * object) const
{
  GenericObject* temp; // temp value to search object in freelist
  
  temp = FreeList_;
    
  while (temp)
  {
	  if (temp == object)
	  {
		  return true;
	  }
	  temp = temp->Next;
  }

  return false;
}

/******************************************************************************/
/*!

  check object is on bad boundary

*/
/******************************************************************************/
bool ObjectAllocator::CheckBadBoundary(GenericObject * object) const
{
  GenericObject* temp; // temp value to search pagelist
  char* pageEnd; // position at end of page
  GenericObject* pgEnd; // end of page
  char* objecthead;
  size_t distance;

  temp = PageList_;

  while (true)
  {
	  pageEnd = reinterpret_cast<char*>(temp) + PageSize_;
	  pgEnd = reinterpret_cast<GenericObject*>(pageEnd);
	  objecthead = reinterpret_cast<char*>(temp) + sizeof(GenericObject*)
				  + PadBytes_ + HBlockInfo_.size_;
	
	  if (reinterpret_cast<GenericObject*>(objecthead) <= object 
	      && object < pgEnd)
	  {
		  break;
	  }
	  
	  temp = temp->Next;
	  
	  if (!temp)
	  {
		  return true;
	  }
  }

  distance = reinterpret_cast<char*>(object) - objecthead;
  if((distance % (ObjectSize_ + 2 * PadBytes_ + HBlockInfo_.size_)) != 0)
  {
	  return true;
  }
  else
  {
	  return false;
  }
  
  return true;
}

/******************************************************************************/
/*!

  check object is corrupted

*/
/******************************************************************************/
bool ObjectAllocator::CheckCorrupted(void * object) const
{
  char unsigned* objectpad;

  objectpad = reinterpret_cast<char unsigned*>(object) - PadBytes_;

  for (unsigned i = 0; i < PadBytes_; ++i)
  {
	  if (objectpad[i] != PAD_PATTERN)
	  {
		  return true;
	  }
  }

  objectpad = objectpad + PadBytes_ + ObjectSize_;

  for (unsigned i = 0; i < PadBytes_; ++i)
  {
	  if (objectpad[i] != PAD_PATTERN)
	  {
		  return true;
	  }
  }

  return false;
}

/******************************************************************************/
/*!

  allocate with using CPPMemManager

*/
/******************************************************************************/
void * ObjectAllocator::CPPAllocate()
{
  char* object;  // allocation target object
  GenericObject* obj; // reinterpret_casted object
  GenericObject* temp; // temp value to find last pagelist
  
  object = new char[sizeof(ObjectSize_)];
  obj = reinterpret_cast<GenericObject*>(object);
  obj->Next = nullptr;

  if (!PageList_)
  {
	  PageList_ = obj;
  }
  else
  {
	  temp = PageList_;
	  while (temp->Next)
	  {
		  temp = temp->Next;
	  }
	  temp->Next = obj;
  }

  return obj;
}

/******************************************************************************/
/*!

  free with using CPPMemManager

*/
/******************************************************************************/
void ObjectAllocator::CPPFree(GenericObject * object)
{
  GenericObject* temp; // temp value to search object in pagelist
  GenericObject* target; // target to delete
  temp = PageList_;
  
  while (temp->Next != object)
  {
	  temp = temp->Next;
  }

  target = temp->Next;
  temp->Next = target->Next;

  delete target;
  target = nullptr;
}

// list of own public function

/******************************************************************************/
/*!

  allocate with label value

*/
/******************************************************************************/
void* ObjectAllocator::Allocate(const char* label)
{
  char* header;  // position of the header
  char* object;  // allocation target object
  GenericObject* temp; // saving future freelist
  
  // check allocation with new or not
  if (UseCPPMemManager_)
  {
	  ++ObjectsInUse_;
	  if (MostObjects_ < ObjectsInUse_)
	  {
		  MostObjects_ = ObjectsInUse_;
	  }
	  ++Allocations_;
	  return CPPAllocate();
  }
  
  // if FreeList is empty, allocate new page
  if (!FreeList_)
  {
	  // check avaiable page
	  if (PagesInUse_ >= MaxPages_)
	  {
		  throw OAException(OAException::E_NO_PAGES,
		  "Allocation failed E_NO_PAGES");
	  }
	  allocate_new_page();
  }

  ++ObjectsInUse_;
  if (MostObjects_ < ObjectsInUse_)
  {
	  MostObjects_ = ObjectsInUse_;
  }
  ++Allocations_;
  --FreeObjects_;
  

  temp = FreeList_->Next;
  object = reinterpret_cast<char*>(FreeList_);
  
  header = object - PadBytes_ - HBlockInfo_.size_;
  
  // check HBlock type
  switch (HBlockInfo_.type_)
  {
	  case OAConfig::hbBasic:
		header[0] = static_cast<char>(Allocations_);
		header[HBlockInfo_.size_ - 1] = 1;
	    break;
	    
	  case OAConfig::hbExtended:
		++header[1];
		header[3] = static_cast<char>(Allocations_);
		header[HBlockInfo_.size_ - 1] = 1;
	    break;
	    
	  case OAConfig::hbExternal:
		MemBlockInfo* extheader;
		
		*reinterpret_cast<MemBlockInfo**>(header) = new MemBlockInfo;
		extheader = *reinterpret_cast<MemBlockInfo**>(header);

		extheader->label = new char[strlen(label) + 1];
		strcpy(extheader->label, label);
		extheader->alloc_num = Allocations_;
		extheader->in_use = 1;
	    break;
	    
	  default:
	    break;
  }

  // object is allocated
  for (unsigned i = 0; i < ObjectSize_; ++i)
  {
	  object[i] = ALLOCATED_PATTERN;
  }

  // FreeList_ = FreeList->Next
  FreeList_ = temp;

  return reinterpret_cast<void*>(object);  
}

/******************************************************************************/
/*!

  free the object

*/
/******************************************************************************/
void ObjectAllocator::Free(void* Object)
{
  GenericObject* obj; // reinterpret_casted object
  char* objectheader; // header position of object
  char* header;  // header position(changing)
  
  obj = reinterpret_cast<GenericObject*>(Object);
  objectheader = reinterpret_cast<char*>(Object);

  --ObjectsInUse_;
  ++Deallocations_;
  
  // check using delete
  if (UseCPPMemManager_)
  {
	  CPPFree(obj);
	  return;
  }

  // check the object is already on free list
  if (CheckFreeList(obj))
  {
	  throw OAException(OAException::E_MULTIPLE_FREE,
      "object is already on free list E_MULTIPLE_FREE");
  }

  // check the object is on bad boundary 
  if (CheckBadBoundary(obj))
  {
	  throw OAException(OAException::E_BAD_BOUNDARY,
      "address is not on block-boundary E_BAD_BOUNDARY");
  }

  // check the object have corrupted block
  if (CheckCorrupted(Object))
  {
	  throw OAException(OAException::E_CORRUPTED_BLOCK,
      "block is corrupted E_CORRUPTED_BLOCK");
  }
  
  header = objectheader - PadBytes_ - HBlockInfo_.size_;
  
  // check HBlock type
  switch (HBlockInfo_.type_)
  {
	  case OAConfig::hbBasic:
		header[0] = 0;
		header[HBlockInfo_.size_ - 1] = 0;
	    break;
	    
	  case OAConfig::hbExtended:
		header[3] = 0;
		header[HBlockInfo_.size_ - 1] = 0;
	    break;
	    
	  case OAConfig::hbExternal:
		MemBlockInfo** extheader;
		extheader = reinterpret_cast<MemBlockInfo**>(header);

		delete[] (*extheader)->label;
		(*extheader)->label = nullptr;
		delete[] *extheader;
		*extheader = nullptr;
	    break;
	    
	  default:
	    break;
  }

  // object is freed
  for (unsigned i = 0; i < ObjectSize_; ++i)
  {
	  objectheader[i] = FREED_PATTERN;
  }

  // Link the free list
  obj->Next = FreeList_;
  FreeList_ = obj;

  ++FreeObjects_;
}

/******************************************************************************/
/*!

  Calls the callback fn for each block still in use

*/
/******************************************************************************/

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
  fn(PageList_, ObjectSize_);
  return 0;
}

/******************************************************************************/
/*!

  Calls the callback fn for each block that is potentially corrupted

*/
/******************************************************************************/

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
  fn(PageList_, ObjectSize_);
  return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
  return 0;
}

bool ObjectAllocator::ImplementedExtraCredit()
{
  return false;
}

/******************************************************************************/
/*!

  set DebugOn_ value with State param

*/
/******************************************************************************/

void ObjectAllocator::SetDebugState(bool State)
{
  DebugOn_ = State;
}

/******************************************************************************/
/*!

  return freelist

*/
/******************************************************************************/

const void* ObjectAllocator::GetFreeList() const
{
  return reinterpret_cast<const void*>(FreeList_);
}

/******************************************************************************/
/*!

  return pagelist

*/
/******************************************************************************/
const void* ObjectAllocator::GetPageList() const
{
  return reinterpret_cast<const void*>(PageList_);
}

/******************************************************************************/
/*!

  return config

*/
/******************************************************************************/
OAConfig ObjectAllocator::GetConfig() const
{
  OAConfig config;
  
  config.UseCPPMemManager_ = UseCPPMemManager_;
  config.ObjectsPerPage_ = ObjectsPerPage_;
  config.MaxPages_ = MaxPages_;
  config.DebugOn_ = DebugOn_;
  config.PadBytes_ = PadBytes_;
  config.HBlockInfo_ = HBlockInfo_;
  config.Alignment_ = Alignment_;
  config.LeftAlignSize_ = LeftAlignSize_;
  config.InterAlignSize_ = InterAlignSize_;

  return config;
}

/******************************************************************************/
/*!

  return stats

*/
/******************************************************************************/
OAStats ObjectAllocator::GetStats() const
{
  // Make new stats object and copy member variables from private
  OAStats stats;

  stats.ObjectSize_ = ObjectSize_;
  stats.PageSize_ = PageSize_;
  stats.FreeObjects_ = FreeObjects_;
  stats.ObjectsInUse_ = ObjectsInUse_;
  stats.PagesInUse_ = PagesInUse_;
  stats.MostObjects_ = MostObjects_;
  stats.Allocations_ = Allocations_;
  stats.Deallocations_ = Deallocations_;

  return stats;
}

/******************************************************************************/
/*!

  allocating new page 

*/
/******************************************************************************/
void ObjectAllocator::allocate_new_page()
{
  char* newpage; 
  GenericObject* newpg;
  GenericObject* nextpage;
  char* temp;
  GenericObject* freelisttemp;
  
  // allocating with page size
  try
  {
    newpage = new char[PageSize_];
  }
  catch (std::bad_alloc &)
  {
    throw OAException(OAException::E_NO_MEMORY, 
    "No system memory available E_NO_MEMORY");
  }

  // initializing with pattern
  for (unsigned i = 0; i < PageSize_; ++i)
  {
    newpage[i] = UNALLOCATED_PATTERN;
  }
  
  // initializing pad block
  if (PadBytes_)
  {
    char* padtemp; // temp value to set padbyte
    char* pageend;

    padtemp = newpage + sizeof(GenericObject*) + HBlockInfo_.size_;
    pageend = newpage + PageSize_;

    while (true)
    {
      for (unsigned i = 0; i < PadBytes_; ++i)
      {
        padtemp[i] = PAD_PATTERN;
      }

      padtemp += PadBytes_ + ObjectSize_;

      if (padtemp >= pageend)
      {
        break;
      }

      for (unsigned i = 0; i < PadBytes_; ++i)
      {
        padtemp[i] = PAD_PATTERN;
      }

      padtemp += PadBytes_ + HBlockInfo_.size_;

      if (padtemp >= pageend)
      {
        break;
      }
    }
  }

  // initialize header block
  if (HBlockInfo_.size_)
  {
    char* headertemp;
    char* pageend;

    headertemp = newpage + sizeof(GenericObject*);
    pageend = newpage + PageSize_;

    while (headertemp < pageend)
    {
      for (unsigned i = 0; i < HBlockInfo_.size_; ++i)
      {
        headertemp[i] = 0x00;
      }

      headertemp += HBlockInfo_.size_ + 2 * PadBytes_ + ObjectSize_;
    }
  }
  
  newpg = reinterpret_cast<GenericObject*>(newpage);
  
  nextpage = PageList_;
  PageList_ = newpg;
  PageList_->Next = nextpage;

  //initializing free list
  temp = newpage + sizeof(GenericObject*) + HBlockInfo_.size_ + PadBytes_;
  freelisttemp = reinterpret_cast<GenericObject*>(temp);
  FreeList_ = freelisttemp;
  FreeList_->Next = nullptr;

  for (unsigned i = 0; i < ObjectsPerPage_ - 1; ++i)
  {
    temp += ObjectSize_ + (2 * PadBytes_) + HBlockInfo_.size_;
    freelisttemp = reinterpret_cast<GenericObject*>(temp);
    freelisttemp->Next = FreeList_;
    FreeList_ = freelisttemp;
  }

  ++PagesInUse_;
  for (unsigned i = 0; i < ObjectsPerPage_; ++i)
  {
    ++FreeObjects_;
  }  
}

void ObjectAllocator::put_on_freelist(void* Object)
{
	tempobject = Object;
}

ObjectAllocator::ObjectAllocator(const ObjectAllocator& oa)
{
  const ObjectAllocator tmp = oa;
}

ObjectAllocator& ObjectAllocator::operator=(const ObjectAllocator& oa)
{
  const ObjectAllocator tmp = oa;
  return *this;
}
