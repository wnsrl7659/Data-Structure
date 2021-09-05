/******************************************************************************/
/*!
\file   BSTree.h
\author Junki Kim
\par    email: junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #4
\date   6/28/2019
\brief
  This is the header file for BSTree.
*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef BSTREEH
#define BSTREEH
//---------------------------------------------------------------------------
#include <string>    // std::string
#include <stdexcept> // std::exception

#include "ObjectAllocator.h"

/*!
  The exception class for the AVL/BST classes
*/
class BSTException : public std::exception
{
  public:
    /*!
      Non-default constructor

      \param ErrCode
        The kind of exception (only one currently)

      \param Message
        The human-readable reason for the exception.
    */
    BSTException(int ErrCode, const std::string& Message) :
      error_code_(ErrCode), message_(Message) {
    };

    /*! 
      Retrieve the exception code.

      \return
        E_NO_MEMORY
    */
    virtual int code() const {
      return error_code_;
    }

    /*!
      Retrieve the message string

      \return
        The human-readable message.
    */
    virtual const char *what() const throw() {
      return message_.c_str();
    }

    //! Destructor
    virtual ~BSTException() {}

    //! The kinds of exceptions (only one currently)
    enum BST_EXCEPTION{E_NO_MEMORY};

  private:  
    int error_code_;      //!< The code of the exception
    std::string message_; //!< Readable message text
};

/*!
  The definition of the BST 
*/
template <typename T>
class BSTree
{
  public:
    //! The node structure
    struct BinTreeNode
    {
      BinTreeNode *left;  //!< The left child
      BinTreeNode *right; //!< The right child
      T data;             //!< The data
      int balance_factor; //!< optional for efficient balancing
      unsigned count;     //!< nodes in this subtree for efficient indexing

      //! Default constructor
      BinTreeNode() : left(0), right(0), data(0), balance_factor(0), count(1) {};

      //! Conversion constructor
      BinTreeNode(const T& value) : left(0), right(0), data(value), balance_factor(0), count(1) {};
    };

    //! shorthand
    typedef BinTreeNode* BinTree;

    BSTree(ObjectAllocator *oa = 0, bool ShareOA = false);
    BSTree(const BSTree& rhs);
    virtual ~BSTree();
    BSTree& operator=(const BSTree& rhs);
    const BinTreeNode* operator[](int index) const; // for r-values (Extra Credit)
    virtual void insert(const T& value);
    virtual void remove(const T& value);
    void clear();
    bool find(const T& value, unsigned &compares) const;
    bool empty() const;
    unsigned int size() const;
    int height() const;
    BinTree root() const;

    static bool ImplementedIndexing();

  protected:
    BinTree& get_root();
    BinTree make_node(const T& value) const;
    void free_node(BinTree node);
    int tree_height(BinTree tree) const;
    void find_predecessor(BinTree tree, BinTree &predecessor) const;

	void InsertItem(BinTree &tree, const T& Data);
	void DeleteItem(BinTree &tree, T Data);
	bool ItemExists(BinTree tree, const T& Data, unsigned &compare) const;
	void RotateRight(BinTree &tree);
	void RotateLeft(BinTree &tree);
	int CheckHeight(BinTree tree) const;

	void DeleteAll(BinTree &tree);
	void CopyAll(BinTree &tree, const BinTree &rhs);

  private:
    // private stuff...
	  bool OAcheck;
	  ObjectAllocator* oa_;
	  bool freecheck;
	  unsigned int size_;
	  BinTree root_;
};

#include "BSTree.cpp"

#endif
//---------------------------------------------------------------------------
