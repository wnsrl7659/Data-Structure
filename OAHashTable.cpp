/******************************************************************************/
/*!
\file   OAHashTable.cpp
\author Junki Kim
\par    email: junki.kim\@digipen.com
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #5
\date   7/10/2019
\brief
  This is the implementation file for all member functions
  of a class called OAHashTable.

*/
/******************************************************************************/

/******************************************************************************/
/*!

  Constructor

  \param Config
	Config variable
*/
/******************************************************************************/
template <typename T>
OAHashTable<T>::OAHashTable(const OAHTConfig& Config)
{
	InitialTableSize_ = Config.InitialTableSize_;
	PrimaryHashFunc_ = Config.PrimaryHashFunc_;
	SecondaryHashFunc_ = Config.SecondaryHashFunc_;
	MaxLoadFactor_ = Config.MaxLoadFactor_;
	GrowthFactor_ = Config.GrowthFactor_;
	DeletionPolicy_ = Config.DeletionPolicy_;
	FreeProc_ = Config.FreeProc_;

	InitTable();
}
/******************************************************************************/
/*!

  Destructor

*/
/******************************************************************************/
template <typename T>
OAHashTable<T>::~OAHashTable()
{
	delete[] table_;
}

/******************************************************************************/
/*!

  Adds an item.

  \param Key
	key value
  \param Data
	data value
  \exception class
	exception class

*/
/******************************************************************************/
template <typename T>
void OAHashTable<T>::insert(const char *Key, const T& Data)
{
	double lf = ((double)stats_.Count_ + 1.0) / (double)stats_.TableSize_;

	if (lf > MaxLoadFactor_)
	{
		GrowTable();
	}

	unsigned stride = 1;
	unsigned index = 0;
	if (PrimaryHashFunc_)
	{
		index = PrimaryHashFunc_(Key, stats_.TableSize_);
		if (SecondaryHashFunc_)
		{
			stride = SecondaryHashFunc_(Key, stats_.TableSize_ - 1) + 1;
		}
	}
	int probe = 0;
	int i = index;
	int firstdeleted = -1;
	while (true)
	{
		stats_.Probes_++;
		probe++;
		if (table_[i].State == OAHTSlot::UNOCCUPIED)
		{
			if (firstdeleted != -1)
			{
				i = firstdeleted;
			}
			strcpy(table_[i].Key, Key);
			table_[i].Data = Data;
			table_[i].State = OAHTSlot::OCCUPIED;
			table_[i].probes = probe;
			stats_.Count_++;
			break;
		}
		else if (table_[i].State == OAHTSlot::OCCUPIED)
		{
			if (strcmp(table_[i].Key, Key) == 0)
			{
				throw(OAHashTableException(OAHashTableException::E_DUPLICATE, "The key already exists."));
				break;
			}
			i += stride;
			i = i % stats_.TableSize_;
		}
		else if (table_[i].State == OAHTSlot::DELETED)
		{
			if (firstdeleted == -1)
			{
				firstdeleted = i;
			}
			i += stride;
			i = i % stats_.TableSize_;
		}
	}
}

/******************************************************************************/
/*!

  Finds an item.

  \param Key
	key value
  \return
    data value by key
  \exception class
	exception class
*/
/******************************************************************************/
template <typename T>
const T& OAHashTable<T>::find(const char *Key)
{
	unsigned stride = 1;
	unsigned index = 0;

	if (PrimaryHashFunc_)
	{
		index = PrimaryHashFunc_(Key, stats_.TableSize_);
		if (SecondaryHashFunc_)
		{
			stride = SecondaryHashFunc_(Key, stats_.TableSize_ - 1) + 1;
		}
	}

	unsigned int i = index;
	while (true)
	{
		stats_.Probes_++;

		if (table_[i].State == OAHTSlot::UNOCCUPIED)
		{
			throw(OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Item not found in table."));
			break;
		}
		else if (table_[i].State == OAHTSlot::OCCUPIED)
		{
			if (strcmp(table_[i].Key, Key) == 0)
			{
				return table_[i].Data;
			}
			i += stride;
			i = i % stats_.TableSize_;
			if (i == index)
			{
				throw(OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Item not found in table."));
				break;
			}
		}
		else if (table_[i].State == OAHTSlot::DELETED)
		{
			i += stride;
			i = i % stats_.TableSize_;
			if (i == index)
			{
				throw(OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Item not found in table."));
				break;
			}
		}
	}
}

/******************************************************************************/
/*!

  Removes an item.

  \param Key
	key value
  \exception class
	exception class
*/
/******************************************************************************/
template <typename T>
void OAHashTable<T>::remove(const char *Key)
{
	unsigned stride = 1;
	unsigned index = 0;

	if (PrimaryHashFunc_)
	{
		index = PrimaryHashFunc_(Key, stats_.TableSize_);
		if (SecondaryHashFunc_)
		{
			stride = SecondaryHashFunc_(Key, stats_.TableSize_ - 1) + 1;
		}
	}

	unsigned int i = index;
	while (true)
	{
		stats_.Probes_++;
		if (table_[i].State == OAHTSlot::UNOCCUPIED)
		{
			throw(OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Key not in table."));
			return;
		}
		else if (table_[i].State == OAHTSlot::OCCUPIED)
		{
			if (strcmp(table_[i].Key, Key) == 0)
			{
				break;
			}
			i += stride;
			i = i % stats_.TableSize_;
			if (i == index)
			{
				throw(OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Key not in table."));
				break;
			}
		}
		else if (table_[i].State == OAHTSlot::DELETED)
		{
			i += stride;
			i = i % stats_.TableSize_;
			if (i == index)
			{
				throw(OAHashTableException(OAHashTableException::E_ITEM_NOT_FOUND, "Key not in table."));
				break;
			}
		}
	}

	if (FreeProc_)
	{
		FreeProc_(table_[i].Data);
	}
	else
	{
		table_[i].Data = 0;
	}
	memset(table_[i].Key, 0, MAX_KEYLEN);
	table_[i].probes = 0;
	stats_.Count_--;
	if (DeletionPolicy_ == MARK)
	{
		table_[i].State = OAHTSlot::DELETED;
	}
	else
	{
		table_[i].State = OAHTSlot::UNOCCUPIED;

		int j = (i + stride) % stats_.TableSize_;
		int cycle = i;
		int flag = 0;
		while (true)
		{
			//stats_.Probes_++;

			if (table_[j].State == OAHTSlot::UNOCCUPIED)
			{
				return;
			}
			else if (cycle == j && flag == 1)
			{
				return;
			}
			else
			{
				stats_.Count_--;
				table_[j].State = OAHTSlot::UNOCCUPIED;
				insert(table_[j].Key, table_[j].Data);

				flag = 1;
				j += stride;
				j = j % stats_.TableSize_;
			}
		}
	}
}

/******************************************************************************/
/*!

  clear all table

*/
/******************************************************************************/
template <typename T>
void OAHashTable<T>::clear()
{
	for (unsigned i = 0; i < stats_.TableSize_; i++)
	{
		if (FreeProc_ && table_[i].Data)
		{
			FreeProc_(table_[i].Data);
		}
		else
		{
			table_[i].Data = 0;
		}
		memset(table_[i].Key, 0, MAX_KEYLEN);
		table_[i].State = OAHTSlot::UNOCCUPIED;
		table_[i].probes = 0;
	}
	stats_.Count_ = 0;
}
/******************************************************************************/
/*!

  return stats

  \return
	stats value
*/
/******************************************************************************/
template <typename T>
OAHTStats OAHashTable<T>::GetStats() const
{
	return stats_;
}

/******************************************************************************/
/*!

  return table

  \return
	table value
*/
/******************************************************************************/
template <typename T>
const typename OAHashTable<T>::OAHTSlot* OAHashTable<T>::GetTable() const
{
	return table_;
}

/******************************************************************************/
/*!

  table create

  \exception class
	exception class
*/
/******************************************************************************/
template <typename T>
void OAHashTable<T>::InitTable()
{
	try
	{
		table_ = new OAHTSlot[InitialTableSize_];
	}
	catch (const std::exception& e)
	{
		throw(OAHashTableException(OAHashTableException::E_NO_MEMORY, e.what()));
	}
	stats_.TableSize_ = InitialTableSize_;
	stats_.PrimaryHashFunc_ = PrimaryHashFunc_;
	stats_.SecondaryHashFunc_ = SecondaryHashFunc_;

	for (unsigned i = 0; i < stats_.TableSize_; i++)
	{
		table_[i].Data = 0;
		memset(table_[i].Key, 0, MAX_KEYLEN);
		table_[i].State = OAHTSlot::UNOCCUPIED;
		table_[i].probes = 0;
	}
}


/******************************************************************************/
/*!

  expend a table.

  \exception class
	exception class
*/
/******************************************************************************/
template <typename T>
void OAHashTable<T>::GrowTable()
{
	unsigned int newsize = GetClosestPrime((unsigned int)(stats_.TableSize_ * GrowthFactor_ + 0.99));

	OAHTSlot* temp = table_;
	unsigned int oldsize = stats_.TableSize_;

	try
	{
		table_ = new OAHTSlot[newsize];
	}
	catch (const std::exception& e)
	{
		throw(OAHashTableException(OAHashTableException::E_NO_MEMORY, e.what()));
	}

	stats_.TableSize_ = newsize;
	stats_.Count_ = 0;

	for (unsigned i = 0; i < stats_.TableSize_; i++)
	{
		table_[i].Data = 0;
		memset(table_[i].Key, 0, MAX_KEYLEN);
		table_[i].State = OAHTSlot::UNOCCUPIED;
		table_[i].probes = 0;
	}

	for (unsigned i = 0; i < oldsize; i++)
	{
		if (temp[i].State == OAHTSlot::OCCUPIED)
		{
			insert(temp[i].Key, temp[i].Data);
		}
	}

	delete[] temp;

	stats_.Expansions_++;
}