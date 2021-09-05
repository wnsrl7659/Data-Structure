/******************************************************************************/
/*!
\file   OAHashTable.h
\author Junki Kim
\par    email: junki.kim\@digipen.com
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #5
\date   7/10/2019
\brief
  This is the interface file for all member functions
  of a class called OAHashTable.

*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef OAHASHTABLEH
#define OAHASHTABLEH
//---------------------------------------------------------------------------
#include <string>
#include "Support.h"

/*!
client-provided hash function: takes a key and table size,
returns an index in the table.
*/
typedef unsigned (*HASHFUNC)(const char *, unsigned);

//! Max length of our "string" keys
const unsigned MAX_KEYLEN = 32;

//! The exception class for the hash table
class OAHashTableException
{
  private:  
    int error_code_;      //!< Code for the exception
    std::string message_; //!< Readable string describing the exception

  public:
  	//! Non-default constructor
    OAHashTableException(int ErrCode, const std::string& Message) :
        error_code_(ErrCode), message_(Message) {};

    //! Destructor
    virtual ~OAHashTableException() {
    }

    /*!
    	Retrieves exception code

    	\return
    		One of: E_ITEM_NOT_FOUND, E_DUPLICATE, E_NO_MEMORY
    */
    virtual int code() const { 
      return error_code_; 
    }

    /*!
    	Retrieve human-readable string describing the exception

    	\return
    		The description of the exception
	  */
    virtual const char *what() const {
      return message_.c_str();
    }
    //! Possible exception conditions
    enum OAHASHTABLE_EXCEPTION {E_ITEM_NOT_FOUND, E_DUPLICATE, E_NO_MEMORY};
};

//! The policy used during a deletion
enum OAHTDeletionPolicy {MARK, PACK};

//! OAHashTable statistical info
struct OAHTStats
{
  //! Default constructor
  OAHTStats() : Count_(0), TableSize_(0), Probes_(0), Expansions_(0),
                    PrimaryHashFunc_(0), SecondaryHashFunc_(0) {};
  unsigned Count_;             //!< Number of elements in the table
  unsigned TableSize_;         //!< Size of the table (total slots)
  unsigned Probes_;            //!< Number of probes performed
  unsigned Expansions_;        //!< Number of times the table grew
  HASHFUNC PrimaryHashFunc_;   //!< Pointer to primary hash function
  HASHFUNC SecondaryHashFunc_; //!< Pointer to secondary hash function
};

//! Hash table definition (open-addressing)
template <typename T>
class OAHashTable
{
  public:

    typedef void (*FREEPROC)(T); //!< client-provided free proc (we own the data)

    //! Configuration for the hash table
    struct OAHTConfig
    {
      //OAHTConfig() { SecondaryHashFunc = 0;  MaxLoadFactor = 0.5;  GrowthFactor = 2.0; Policy = PACK;  FreeProc = 0; }
    	//! Non-default constructor
      OAHTConfig(unsigned InitialTableSize, 
                 HASHFUNC PrimaryHashFunc, 
                 HASHFUNC SecondaryHashFunc = 0,
                 double MaxLoadFactor = 0.5,
                 double GrowthFactor = 2.0, 
                 OAHTDeletionPolicy Policy = PACK,
                 FREEPROC FreeProc = 0) :

        InitialTableSize_(InitialTableSize), PrimaryHashFunc_(PrimaryHashFunc), 
        SecondaryHashFunc_(SecondaryHashFunc), MaxLoadFactor_(MaxLoadFactor), 
        GrowthFactor_(GrowthFactor), DeletionPolicy_(Policy),
        FreeProc_(FreeProc) {}

      unsigned InitialTableSize_;         //!< The starting table size
      HASHFUNC PrimaryHashFunc_;          //!< First hash function
      HASHFUNC SecondaryHashFunc_;        //!< Hash function to resolve collisions
      double MaxLoadFactor_;              //!< Maximum LF before growing
      double GrowthFactor_;               //!< The amount to grow the table
      OAHTDeletionPolicy DeletionPolicy_; //!< MARK or PACK
      FREEPROC FreeProc_;                 //!< Client-provided free function
    };
      
      //! Slots that will hold the key/data pairs
    struct OAHTSlot
    {
      //! The 3 possible states the slot can be in
      enum OAHTSlot_State {OCCUPIED, UNOCCUPIED, DELETED};

      char Key[MAX_KEYLEN]; //!< Key is a string
      T Data;               //!< Client data
      OAHTSlot_State State; //!< The state of the slot
      int probes;           //!< For testing
    };

    OAHashTable(const OAHTConfig& Config); // Constructor
    ~OAHashTable();                        // Destructor

      // Insert a key/data pair into table. Throws an exception if the
      // insertion is unsuccessful.
    void insert(const char *Key, const T& Data);

      // Delete an item by key. Throws an exception if the key doesn't exist.
      // Compacts the table by moving key/data pairs, if necessary
    void remove(const char *Key);

      // Find and return data by key. Throws an exception (E_ITEM_NOT_FOUND)
      // if not found.
    const T& find(const char *Key);

      // Removes all items from the table (Doesn't deallocate table)
    void clear();

      // Allow the client to peer into the data
    OAHTStats GetStats() const;
    const OAHTSlot *GetTable() const;

  private: // Some suggestions (You don't have to use any of this.)
  
      // Initialize the table after an allocation
    void InitTable();

      // Expands the table when the load factor reaches a certain point
      // (greater than MaxLoadFactor) Grows the table by GrowthFactor,
      // making sure the new size is prime by calling GetClosestPrime
    void GrowTable();

    // Other private fields and methods...

    //! The starting table size
	unsigned InitialTableSize_;
    //! First hash function
	HASHFUNC PrimaryHashFunc_;
    //! Hash function to resolve collisions
	HASHFUNC SecondaryHashFunc_;
    //! Maximum LF before growing
	double MaxLoadFactor_;
    //! The amount to grow the table
	double GrowthFactor_;
    //! MARK or PACK
	OAHTDeletionPolicy DeletionPolicy_;
    //! saving value
	FREEPROC FreeProc_; 
    //! saving value
	OAHTStats stats_;   
    //! saving value
	OAHTSlot* table_;
};

#include "OAHashTable.cpp"

#endif
