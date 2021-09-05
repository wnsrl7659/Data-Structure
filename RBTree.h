/******************************************************************************/
/*!
\file   RBTree.h
\author Junki Kim
\par    junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment Extra
\date   07/21/2019
\brief
This file contains interface of an Red Black Tree
that uses Almost same algorithm with BST, but also have Unique algorithm.
and some remove method if i have time.

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef RBTREEH
#define RBTREEH
//---------------------------------------------------------------------------

#include <string>

/*!
  Exception class
*/
class RBTreeException
{
  public:
    /*!
      Constructor

      \param ErrCode
        The type of exception.

      \param Message
        The text message related to the exception.
    */
    RBTreeException(int ErrCode, const std::string& Message) :
      error_code_(ErrCode), message_(Message) {
    };

    /*!
      Retrieves the error code

      \return
        The actual error code
    */
    int code() const {
      return error_code_;
    }

    /*!
      Retrieves the text message associated with the error.

      \return
        The text message.
    */
    const char *what() const {
      return message_.c_str();
    }

    /*!
      Destructor
    */
    ~RBTreeException() {}
    /*!
      Possible exceptions
    */
    enum RBTREE_EXCEPTION{E_DUPLICATE, E_NO_MEMORY};

  private:  
    int error_code_;      //!< The error code
    std::string message_; //!< The text message associated with the error
};

/*!
  Red-Black tree class
*/
template <typename T>
class RBTree
{
  public:
      /*!
        Allowable colors
      */
    enum RBCOLOR { rbRED, rbBLACK };

      /*!
        A red-black tree node structure
      */
    struct RBNode
    {
      RBNode *left;   //!< left child
      RBNode *right;  //!< right child
      RBNode *parent; //!< parent node ptr
      RBCOLOR color;  //!< color (red or black)
      T data;         //!< The actual data
    };

    RBTree();
    ~RBTree();
    void insert(const T& item);
    void remove(const T& item);
    void clear(void);
    bool find(const T& value, unsigned &compares) const;
    bool empty(void) const;
    unsigned size(void) const;
    int height(void) const;
    const RBNode *root(void) const;
    static bool ImplementedRemove(void);

  private:
	  //! size(node count) of the RBTree. keep updating
	  unsigned size_;
	  //! roots of RBTree
	  RBNode* root_;

	  //helper function
	  RBNode* make_node(const T& value) const;
	  RBNode* InsertItem(RBNode* &tree, const T& Data);

	  void DeleteAll(RBNode* &tree);

	  void RotateRight(RBNode* tree);
	  void RotateLeft(RBNode* tree);

	  bool ItemExists(RBNode* tree, const T& Data, unsigned &compare) const;
	  int  CheckHeight(RBNode* tree) const;
};

#include "RBTree.cpp"

#endif
