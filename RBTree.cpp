/******************************************************************************/
/*!
\file   RBTree.cpp
\author Junki Kim
\par    junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment Extra
\date   07/21/2019
\brief
This file contains implementation of an Red Black Tree
that uses Almost same algorithm with BST, but also have Unique algorithm.
and some remove method if i have time.

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

/******************************************************************************/
/*!
  \brief RBTree
  RBTree's default constructor. Initialize root and size of the tree

*/
/******************************************************************************/
template <typename T>
RBTree<T>::RBTree()
{
	root_ = NULL;
	size_ = 0;
}

/******************************************************************************/
/*!
  \brief ~RBTree
  RBTree's default destructor.
  call clear function for check every node is deleted well.

*/
/******************************************************************************/
template <typename T>
RBTree<T>::~RBTree()
{
	clear();
}

/******************************************************************************/
/*!
  \brief make_node
  This helper function uses for create RBNode by given value. 
  check the exception for memory allocation. 
  initialize all data. and return created node.
  \param value
  templated typename T for data of node.
  \return
  return a pointer to created RBNode.
*/
/******************************************************************************/
template <typename T>
typename RBTree<T>::RBNode* RBTree<T>::make_node(const T& value) const
{
	RBNode* node; //pointer to RBNode
	//check bad allocation by try and catch
	try
	{
		node = new RBNode;
	}
	catch (const std::bad_alloc&)
	{
		throw (RBTreeException(RBTreeException::E_NO_MEMORY,
			"Not enough memory!"));
	}
	//initialize all data
	node->data = value;
	node->left = 0;
	node->right = 0;
	node->parent = 0;
	//return node
	return node;
}

/******************************************************************************/
/*!
  \brief InsertItem
  This recursive helper function uses for go to right node for adding item.
  the difference from BST is that this function also link node's parent.
  and also for getting X location for using it in algorithm, 
  it return pointer to created node.
  \param tree
  pointer to RBNode, keep checking the location in recursive function
  \param Data
  templated typename T value for data of created node.
  \return
  return a pointer to created RBNode.
*/
/******************************************************************************/
template <typename T>
typename RBTree<T>::RBNode* RBTree<T>::InsertItem(RBNode* &tree, const T& Data)
{
	//it only happens when create roots
	if (tree == 0)
	{
		tree = make_node(Data);
		size_++;
		return tree;
	}
	//check the node should go left or right
	else if (Data < tree->data)
	{
		//node should go left
		//if next left node is the location for created node
		//create node here for link parent
		if (tree->left == 0)
		{
			//create node
			tree->left = make_node(Data);
			//increase size
			size_++;
			//link parent to created node
			tree->left->parent = tree;
			//return created node for using it 
			return tree->left;
		}
		else
		{
			//if left node is not location for create
			//keep go to next location
			return InsertItem(tree->left, Data);
		}
	}
	else if (Data > tree->data)
	{
		//node should go right
		//if next right node is the location for created node
		//create node here for link parent
		if (tree->right == 0)
		{
			//create node
			tree->right = make_node(Data);
			//increase size
			size_++;
			//link parent to created node
			tree->right->parent = tree;
			//return created node for using it 
			return tree->right;
		}
		else
		{
			//if right node is not location for create
			//keep go to next location
			return InsertItem(tree->right, Data);
		}
	}
	//never happen
	//for avoid warning?
	return 0;
}

/******************************************************************************/
/*!
  \brief insert
  This function uses for insert the node to RBTree.
  this function use unsal insert method like BST. 
  However, it also set the color of the node in tree depend on RBTree algorithm.
  \param item
  templated typename T value for data of created node.
*/
/******************************************************************************/
template <typename T>
void RBTree<T>::insert(const T& item)
{
	//initialize pointer to RBNode X, Y for following algorithm.
	RBNode* X;
	RBNode* Y;
	unsigned compare = 0;

	//check duplicate
	if (find(item, compare))
	{
		//throw
		throw (RBTreeException(RBTreeException::E_DUPLICATE,
				"Duplicate item inserted"));
		return;
	}

	//Insert new node into RBTree in usual manner(it's a BST)
	//Set new node's color to RED and call it X
	X = InsertItem(root_, item);

	//The root of the tree is BLACK. Technically, the root may be RED, 
	//but to keep the algorithm simple and ensure 
	//that everyone's trees look identical, we'll require the root to be BLACK.
	if (root_ == X)
	{
		X->color = rbBLACK;
	}
	//if it's not root, new node is always red.
	//Newly inserted nodes are marked as RED
	else
	{
		X->color = rbRED;
	}

	// The Red/Black property may have been destroyed
	// so we have to restore it
	//while (X is not the Root) and (X's Parent is RED)
	while((X != root_) && (X->parent->color == rbRED)) 
	{
		//if (X's Parent is a LEFT child)
		if (X->parent->data < X->parent->parent->data)
		{
			// If X's Parent is a LEFT, Uncle must be a RIGHT
			//Set Y to X's Uncle (Uncle is Grandparent's RIGHT child)
			Y = X->parent->parent->right;
			
			//if (Y is RED)
			if ((Y != NULL) && Y->color == rbRED)
			{
				// Case #1, Change the tri-node's colors
				//Set X's Parent's color to BLACK
				X->parent->color = rbBLACK;
				//Set Y's color to BLACK
				Y->color = rbBLACK;
				//Set X's Grandparent's color to RED
				X->parent->parent->color = rbRED;
				//Set X to Grandparent
				X = X->parent->parent;
			}
			else // Y is BLACK
			{
				// and X is RIGHT, Case #2
				// move X up and rotate it (rotates child into Parent's spot)
				//if (X is a RIGHT child)
				if(X->data > X->parent->data)
				{
					//set X to parent
					X = X->parent;
					//Rotate LEFT about new X
					RotateLeft(X);
				}

				// Case #3
				//Set X's Parent's color to BLACK
				X->parent->color = rbBLACK;
				//Set X's Grandparent's color to RED
				X->parent->parent->color = rbRED;
				//Rotate RIGHT about X's Grandparent;
				RotateRight(X->parent->parent);
			}
		}
		else // X's Parent is a RIGHT child (symmetrical to above code)
		{
			// If X's Parent is a RIGHT, Uncle must be a LEFT
			//Set Y to X's Uncle (Uncle is Grandparent's LEFT child)
			Y = X->parent->parent->left;

			if ((Y != NULL) && Y->color == rbRED)
			{
				// Case #1, Change the tri-node's colors
				//Set X's Parent's color to BLACK
				X->parent->color = rbBLACK;
				//Set Y's color to BLACK
				Y->color = rbBLACK;
				//Set X's Grandparent's color to RED
				X->parent->parent->color = rbRED;
				//Set X to Grandparent
				X = X->parent->parent;
			}
			else
			{
				// and X is LEFT, Case #2
				// move X up and rotate it (rotates child into Parent's spot)
				//if (X is a LEFT child)
				if (X->data < X->parent->data)
				{
					//set X to parent
					X = X->parent;
					//Rotate RIGHT about new X
					RotateRight(X);
				}

				// Case #3
				//Set X's Parent's color to BLACK
				X->parent->color = rbBLACK;
				//Set X's Grandparent's color to RED
				X->parent->parent->color = rbRED;
				//Rotate LEFT about X's Grandparent;
				RotateLeft(X->parent->parent);
			}
		}
		//keep update root's color to black.
		root_->color = rbBLACK;
	}
}

/******************************************************************************/
/*!
  \brief remove
  This function uses for remove the node from RBTree.
  \param item
  templated typename T value for data of deleted node.
*/
/******************************************************************************/
template <typename T>
void RBTree<T>::remove(const T& item)
{
	//avoid unused variable
	T temp = item;
	T tempt = item;
	if (temp == tempt)
	{
		temp = tempt;
		tempt = temp;
	}
}

/******************************************************************************/
/*!
  \brief DeleteAll
  This recursive helper function uses for delete all node in RBTree.
  reach end of the node, and delete it step by step.
  \param tree
  pointer to RBNode, keep checking the location in recursive function
*/
/******************************************************************************/
template <typename T>
void RBTree<T>::DeleteAll(RBNode* &tree)
{
	//if there's  data, keep go for reaching end of the node.
	if (tree->left)
	{
		DeleteAll(tree->left);
	}
	if (tree->right)
	{
		DeleteAll(tree->right);
	}
	//if tree's children is deleted, delete tree and set it to null.
	delete tree;
	tree = 0;
}

/******************************************************************************/
/*!
  \brief clear
  This function uses for clear all node in RBTree.
  call recursive helper function to clear it.
*/
/******************************************************************************/
template <typename T>
void RBTree<T>::clear(void)
{
	//check the tree is not empty
	if (root_)
	{
		//call recursive helper function to clear it
		DeleteAll(root_);
	}
	size_ = 0;
}

/******************************************************************************/
/*!
  \brief ItemExists
  This recursive helper function uses for help finding node which have
  given data.
  reach end of the node until find data.
  \param tree
  pointer to RBNode, keep checking the location in recursive function
  \param Data
  templated typename T given value to find
  \param compare
  unsigned integer for counting how many times compare
  \return
  return a bool value to check find or not
*/
/******************************************************************************/
template <typename T>
bool RBTree<T>::ItemExists(RBNode*  tree, const T& Data, unsigned &compare) const
{
	//keep updating compare count
	compare++;
	//if it reach end of the node, it means there's no item in this tree
	if (tree == 0)
	{
		return false;
	}
	//if it found the item, return true
	else if (Data == tree->data)
	{
		return true;
	}
	//if the data is different, keep search by BST method. RBTree is BST.
	else if (Data < tree->data)
	{
		return ItemExists(tree->left, Data, compare);
	}
	else
	{
		return ItemExists(tree->right, Data, compare);
	}
}

/******************************************************************************/
/*!
  \brief find
  This function uses for finding node which have given data.
  call recursive helper function to find it.
  \param value
  templated typename T given value to find
  \param compares
  unsigned integer for counting how many times compare
  \return
  return a bool value to check find or not
*/
/******************************************************************************/
template <typename T>
bool RBTree<T>::find(const T& value, unsigned &compares) const
{
	//reset compare count
	compares = 0;
	//check emptyness
	if (root_ == NULL)
	{
		return false;
	}
	//call recursive helper function to find it.
	return ItemExists(root_, value, compares);
}

/******************************************************************************/
/*!
  \brief empty
  This function uses for checking node's emptyness
  \return
  return a bool value to check empty or not
*/
/******************************************************************************/
template <typename T>
bool RBTree<T>::empty(void) const
{
	//if root is null, it means tree is empty
	if (root_ == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************/
/*!
  \brief size
  This function uses for checking tree's size (node count)
  \return
  return an unsigned integer which have size(node count)
*/
/******************************************************************************/
template <typename T>
unsigned RBTree<T>::size(void) const
{
	return size_;
}

/******************************************************************************/
/*!
  \brief CheckHeight
  This recursive helper function uses for checking tree's height
  \param tree
  pointer to RBNode, keep checking the location in recursive function
  \return
  return an integer which have tree's height
*/
/******************************************************************************/
template <typename T>
int RBTree<T>::CheckHeight(RBNode* tree) const
{
	//following cs280's note

	//if tree reach the end of the node, return -1 and
	//start calculation
	if (tree == 0)
		return -1;

	//check left and right child's height and 
	//check height for higher children
	if (CheckHeight(tree->left) > CheckHeight(tree->right))
		return CheckHeight(tree->left) + 1;
	else
		return CheckHeight(tree->right) + 1;
}

/******************************************************************************/
/*!
  \brief height
  This function uses for checking tree's height
  \return
  return an integer which have tree's height
*/
/******************************************************************************/
template <typename T>
int RBTree<T>::height(void) const
{
	return CheckHeight(root_);
}

/******************************************************************************/
/*!
  \brief root
  This recursive helper function uses for returning root of RBTree
  \return
  return a pointer to root
*/
/******************************************************************************/
template <typename T>
const typename RBTree<T>::RBNode* RBTree<T>::root(void) const
{
	return root_;
}

/******************************************************************************/
/*!
  \brief ImplementedRemove
  This function uses for checking remove usable
  \return
  return boolian depend on implemetation
*/
/******************************************************************************/
template <typename T>
bool RBTree<T>::ImplementedRemove(void)
{
	return false;
}

/******************************************************************************/
/*!
  \brief RotateRight
  This helper function uses for rotate given node to right.
  Using same technique as BST, however there's some difference.
  Node of RBTree has parent, so link it correctly after rotation.
  \param tree
  the rotation target node
*/
/******************************************************************************/
template <typename T>
void RBTree<T>::RotateRight(RBNode* tree)
{
	//bool value to check rotated node is root or not.
	bool rootflag = false;
	//check rotated node is root or not.
	if (tree == root_)
	{
		rootflag = true;
	}
	//same technique as BST
	RBNode* temp = tree;
	tree = tree->left;
	temp->left = tree->right;
	tree->right = temp;

	//link rotated node's parent correctly
	tree->parent = temp->parent;
	temp->parent = tree;
	if (temp->left)
	{
		temp->left->parent = temp;
	}

	//if root was changed, update new root
	if (rootflag == true)
	{
		root_ = tree;
	}
	//link rotated node's parent correctly
	if(tree != root_)
	{
		if (tree->data < tree->parent->data)
		{
			tree->parent->left = tree;
		}
		else
		{
			tree->parent->right = tree;
		}
	}
}

/******************************************************************************/
/*!
  \brief RotateLeft
  This helper function uses for rotate given node to left.
  Using same technique as BST, however there's some difference.
  Node of RBTree has parent, so link it correctly after rotation.
  \param tree
  the rotation target node
*/
/******************************************************************************/
template <typename T>
void RBTree<T>::RotateLeft(RBNode* tree)
{
	//bool value to check rotated node is root or not.
	bool rootflag = false;
	//check rotated node is root or not.
	if (tree == root_)
	{
		rootflag = true;
	}
	//same technique as BST
	RBNode* temp = tree;
	tree = tree->right;
	temp->right = tree->left;
	tree->left = temp;

	//link rotated node's parent correctly
	tree->parent = temp->parent;
	temp->parent = tree;
	if (temp->right)
	{
		temp->right->parent = temp;
	}
	//if root was changed, update new root
	if (rootflag == true)
	{
		root_ = tree;
	}
	//link rotated node's parent correctly
	if (tree != root_)
	{
		if (tree->data < tree->parent->data)
		{
			tree->parent->left = tree;
		}
		else
		{
			tree->parent->right = tree;
		}
	}
}