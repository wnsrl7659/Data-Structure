/******************************************************************************/
/*!
\file   AVLTree.h
\author Junki Kim
\par    email: junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #4
\date   6/28/2019
\brief
  This is the header file for AVLTree.
*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef AVLTREEH
#define AVLTREEH
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

/*!
  Definition for the AVL Tree 
*/
template <typename T>
class AVLTree : public BSTree<T>
{
  public:
    AVLTree(ObjectAllocator *oa = 0, bool ShareOA = false);
    virtual ~AVLTree() = default; // DO NOT IMPLEMENT
    virtual void insert(const T& value) override;
    virtual void remove(const T& value) override;

	static bool ImplementedIndexing();

  private:
	  typedef typename BSTree<T>::BinTreeNode* BinTree;
    // private stuff...
	  bool OAcheck;
	  ObjectAllocator* oa_;
	  unsigned int size_;

	  int popcheck = 0;

	  std::stack<BinTree> topnode;
	  void FindTop(BinTree Tree, const T& value);
};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------
