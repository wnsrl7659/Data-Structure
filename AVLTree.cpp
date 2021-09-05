/******************************************************************************/
/*!
\file   AVLTree.cpp
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

/******************************************************************************/
/*!

  default constructor

*/
/******************************************************************************/
template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *oa, bool ShareOA)
{
	oa_ = oa;
	OAcheck = ShareOA;

	size_ = 0;
}


/******************************************************************************/
/*!

  stack top node step by step

*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::FindTop(BinTree Tree, const T& value)
{
	if (Tree->data == value)
	{
		
	}
	else
	{
		if (value < Tree->data)
		{
			topnode.push(Tree);
			FindTop(Tree->left, value);
		}
		else
		{
			topnode.push(Tree);
			FindTop(Tree->right, value);
		}
	}
}

/******************************************************************************/
/*!

  insert the node to AVLtree

*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::insert(const T& value)
{
	BSTree<T>::InsertItem(BSTree<T>::get_root(), value);
	
	FindTop(BSTree<T>::get_root(), value);

	while (!topnode.empty())
	{
		BinTree y = topnode.top();
		
		int left, right;

		left = BSTree<T>::CheckHeight(y->left);
		right = BSTree<T>::CheckHeight(y->right);

		int diff;

		if (left >= right)
		{
			diff = left - right;
		}
		else
		{
			diff = right - left;
		}

		if (diff >= 2)
		{
			if (left > right)
			{
				BinTree u = y->left;
				BinTree v = u->left;
				BinTree w = u->right;

				int hv = BSTree<T>::CheckHeight(v);
				int hw = BSTree<T>::CheckHeight(w);

				if (hv > hw)
				{
					BSTree<T>::RotateRight(y);
				}
				else
				{
					BSTree<T>::RotateLeft(u);
					y->left = u;
					BSTree<T>::RotateRight(y);
				}
				if (y->right == BSTree<T>::get_root())
				{
					BSTree<T>::get_root() = y;
				}
				else
				{
					topnode.pop();
					BinTree ytop = topnode.top();
					if (ytop->data < y->data)
					{
						ytop->right = y;
					}
					else
					{
						ytop->left = y;
					}
				}
			}
			else
			{
				BinTree u = y->right;
				BinTree v = u->left;
				BinTree w = u->right;

				int hv = BSTree<T>::CheckHeight(v);
				int hw = BSTree<T>::CheckHeight(w);

				if (hv > hw)
				{
					BSTree<T>::RotateRight(u);
					y->right = u;
					BSTree<T>::RotateLeft(y);
				}
				else
				{
					BSTree<T>::RotateLeft(y);
				}
				if (y->left == BSTree<T>::get_root())
				{
					BSTree<T>::get_root() = y;
				}
				else
				{
					topnode.pop();
					BinTree ytop = topnode.top();
					if (ytop->data < y->data)
					{
						ytop->right = y;
					}
					else
					{
						ytop->left = y;
					}
				}
			}
			break;
		}

		topnode.pop();
	}

	while (!topnode.empty())
	{
		topnode.pop();
	}
}

/******************************************************************************/
/*!

  remove the node from AVL tree

*/
/******************************************************************************/
template <typename T>
void AVLTree<T>::remove(const T& value)
{
	unsigned int temp;
	if (BSTree<T>::find(value, temp) == true)
	{
		FindTop(BSTree<T>::get_root(), value);
		BSTree<T>::DeleteItem(BSTree<T>::get_root(), value);
	}
	while (!topnode.empty())
	{
		BinTree y = topnode.top();

		int left, right;

		left = BSTree<T>::CheckHeight(y->left);
		right = BSTree<T>::CheckHeight(y->right);

		int diff;

		if (left >= right)
		{
			diff = left - right;
		}
		else
		{
			diff = right - left;
		}

		if (diff >= 2)
		{
			if (left > right)
			{
				BinTree u = y->left;
				BinTree v = u->left;
				BinTree w = u->right;

				int hv = BSTree<T>::CheckHeight(v);
				int hw = BSTree<T>::CheckHeight(w);

				if (hv > hw)
				{
					BSTree<T>::RotateRight(y);
				}
				else
				{
					BSTree<T>::RotateLeft(u);
					y->left = u;
					BSTree<T>::RotateRight(y);
				}
				if (y->right == BSTree<T>::get_root())
				{
					BSTree<T>::get_root() = y;
				}
				else
				{
					topnode.pop();
					popcheck = 1;
					BinTree ytop = topnode.top();
					if (ytop->data < y->data)
					{
						ytop->right = y;
					}
					else
					{
						ytop->left = y;
					}
				}
			}
			else
			{
				BinTree u = y->right;
				BinTree v = u->left;
				BinTree w = u->right;

				int hv = BSTree<T>::CheckHeight(v);
				int hw = BSTree<T>::CheckHeight(w);
				if (hv > hw)
				{
					BSTree<T>::RotateRight(u);
					y->right = u;
					BSTree<T>::RotateLeft(y);
				}
				else
				{
					BSTree<T>::RotateLeft(y);
				}
				if (y->left == BSTree<T>::get_root())
				{
					BSTree<T>::get_root() = y;
				}
				else
				{
					topnode.pop();
					popcheck = 1;
					BinTree ytop = topnode.top();
					if (ytop->data < y->data)
					{
						ytop->right = y;
					}
					else
					{
						ytop->left = y;
					}
				}
			}
		}
		if (popcheck == 1)
		{
			popcheck = 0;
		}
		else
		{
			topnode.pop();
		}
	}
}

/******************************************************************************/
/*!

  not available

*/
/******************************************************************************/
template <typename T>
bool AVLTree<T>::ImplementedIndexing()
{
	return false;
}
