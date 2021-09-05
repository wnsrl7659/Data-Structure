/******************************************************************************/
/*!
\file   BSTree.cpp
\author Junki Kim
\par    email: junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #4
\date   6/28/2019
\brief
  This is the cpp file for BSTree.
*/
/******************************************************************************/

/******************************************************************************/
/*!

   default constructor

*/
/******************************************************************************/
template <typename T>
BSTree<T>::BSTree(ObjectAllocator *oa, bool ShareOA)
{
	root_ = 0;
	size_ = 0;
	
	if (oa)
	{
		oa_ = oa;
		OAcheck = ShareOA;
		freecheck = false;
	}
	else
	{
		OAConfig config(true);
		oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
		OAcheck = ShareOA;
		freecheck = true;
	}
}
/******************************************************************************/
/*!

  copy helper recursive function

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::CopyAll(BinTree &tree, const BinTree &rhs)
{
	tree = make_node(rhs->data);
	tree->balance_factor = rhs->balance_factor;
	tree->count = rhs->count;

	if (rhs->left)
	{
		CopyAll(tree->left, rhs->left);
	}

	if (rhs->right)
	{
		CopyAll(tree->right, rhs->right);
	}
}
/******************************************************************************/
/*!

  copy constructor

*/
/******************************************************************************/
template <typename T>
BSTree<T>::BSTree(const BSTree& rhs)
{
	if (rhs.OAcheck)
	{
		oa_ = rhs.oa_;
		OAcheck = rhs.OAcheck;
		freecheck = false;
	}
	else
	{
		OAConfig config(true);
		oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
		OAcheck = false;
		freecheck = true;
	}
	size_ = rhs.size_;
	if (rhs.height() != -1)
	{
		CopyAll(root_, rhs.root());
	}
}
/******************************************************************************/
/*!

  assignment operator

*/
/******************************************************************************/
template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs)
{
	if (root_)
	{
		DeleteAll(root_);
	}

	if (rhs.OAcheck)
	{
		oa_ = rhs.oa_;
		OAcheck = rhs.OAcheck;
		freecheck = false;
	}
	else
	{
		OAConfig config(true);
		oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
		OAcheck = false;
		freecheck = true;
	}

	size_ = rhs.size_;

	if (rhs.height() != -1)
	{
		CopyAll(root_, rhs.root());
	}
	return *this;
}
/******************************************************************************/
/*!

  operator []. not implemented

*/
/******************************************************************************/
template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const
{
	BinTree temp = 0;
	if (index == 0)
	{
		temp = 0;
	}
	return temp;
}
/******************************************************************************/
/*!

  delete helper recursive function

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::DeleteAll(BinTree &tree)
{
	if (tree->left)
	{
		DeleteAll(tree->left);
	}
	if (tree->right)
	{
		DeleteAll(tree->right);
	}
	free_node(tree);
	tree = 0;
}
/******************************************************************************/
/*!

  destructor

*/
/******************************************************************************/
template <typename T>
BSTree<T>::~BSTree()
{
	if (root_)
	{
		DeleteAll(root_);
	}
	if (freecheck)
	{
		delete oa_;
	}
}

/******************************************************************************/
/*!

  Insert helper recursive function

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::InsertItem(BinTree& tree, const T& Data)
{
	if (tree == 0)
	{
		tree = make_node(Data);
		size_++;
	}
	else if (Data < tree->data)
		InsertItem(tree->left, Data);
	else if (Data > tree->data)
		InsertItem(tree->right, Data);
}
/******************************************************************************/
/*!

  insert the node

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::insert(const T& value)
{
	InsertItem(root_, value);
}
/******************************************************************************/
/*!

  delete helper function 

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::DeleteItem(BinTree &tree, T Data)
{
	if (tree == 0)
		return;
	else if (Data < tree->data)
		DeleteItem(tree->left, Data);
	else if (Data > tree->data)
		DeleteItem(tree->right, Data);
	else if (Data == tree->data)
	{
		if (tree->left == 0)
		{
			BinTree temp = tree;
			tree = tree->right;
			if (temp == root_)
			{
				root_ = tree;
			}
			free_node(temp);
		}
		else if (tree->right == 0)
		{
			BinTree temp = tree;
			tree = tree->left;
			if (temp == root_)
			{
				root_ = tree;
			}
			free_node(temp);
		}
		else
		{
			BinTree pred = 0;
			find_predecessor(tree, pred);
			tree->data = pred->data;
			DeleteItem(tree->left, tree->data);
		}
	}
}
/******************************************************************************/
/*!

  remove a node

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::remove(const T& value)
{
	DeleteItem(root_, value);
}
/******************************************************************************/
/*!

  clear every node

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::clear()
{
	DeleteAll(root_);
}
/******************************************************************************/
/*!

  find helper function

*/
/******************************************************************************/
template <typename T>
bool BSTree<T>::ItemExists(BinTree tree, const T& Data, unsigned &compare) const
{
	compare++;
	if (tree == 0)
	{
		return false;
	}
	else if (Data == tree->data)
	{
		return true;
	}
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

  find exist item by value

*/
/******************************************************************************/
template <typename T>
bool BSTree<T>::find(const T& value, unsigned &compares) const
{
	return ItemExists(root_, value, compares);
}
/******************************************************************************/
/*!

  checking empty

*/
/******************************************************************************/
template <typename T>
bool BSTree<T>::empty() const
{
	if (root_)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/******************************************************************************/
/*!

  checking size of the node.

*/
/******************************************************************************/
template <typename T>
unsigned int BSTree<T>::size() const
{
	return size_;
}
/******************************************************************************/
/*!

  checking helper function

*/
/******************************************************************************/
template <typename T>
int BSTree<T>::CheckHeight(BinTree tree) const
{
	if (tree == 0)
		return -1;

	if (CheckHeight(tree->left) > CheckHeight(tree->right))
		return CheckHeight(tree->left) + 1;
	else
		return CheckHeight(tree->right) + 1;
}
/******************************************************************************/
/*!

  checking height of the node.

*/
/******************************************************************************/
template <typename T>
int BSTree<T>::height() const
{
	return CheckHeight(root_);
}

/******************************************************************************/
/*!

  return root value

*/
/******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
	return root_;
}

/******************************************************************************/
/*!

  not available

*/
/******************************************************************************/
template <typename T>
bool BSTree<T>::ImplementedIndexing()
{
	return false;
}

/******************************************************************************/
/*!

  get the root address

*/
/******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root()
{
	return root_;
}

/******************************************************************************/
/*!

  add the node

*/
/******************************************************************************/
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T& value) const
{
	BinTree mem = reinterpret_cast<BinTreeNode*>(oa_->Allocate());
	BinTree node = new (mem) BinTreeNode(value);

	node->data = value;
	node->left = 0;
	node->right = 0;

	return node;
}

/******************************************************************************/
/*!

  free the node

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::free_node(BinTree node)
{
	node->~BinTreeNode();
	oa_->Free(node);
	size_ -= 1;
}

/******************************************************************************/
/*!

  checking height of the node.

*/
/******************************************************************************/
template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
	return CheckHeight(tree);
}

/******************************************************************************/
/*!

  predecessor helper for deleting root

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree &predecessor) const
{
	predecessor = tree->left;
	while (predecessor->right != 0)
		predecessor = predecessor->right;
}

/******************************************************************************/
/*!

  rotate the node 

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::RotateRight(BinTree &tree)
{
	BinTree temp = tree;
	tree = tree->left;
	temp->left = tree->right;
	tree->right = temp;
}

/******************************************************************************/
/*!

  rotate the node

*/
/******************************************************************************/
template <typename T>
void BSTree<T>::RotateLeft(BinTree &tree)
{
	BinTree temp = tree;
	tree = tree->right;
	temp->right = tree->left;
	tree->left = temp;
}