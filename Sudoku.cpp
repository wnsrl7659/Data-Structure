/******************************************************************************/
/*!
\file   Sudoku.cpp
\author Junki Kim
\par    email: junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment #3
\date   6/17/2019
\brief
  This is the implementation file for Sudoku.
*/
/******************************************************************************/
#include "Sudoku.h"


/******************************************************************************/
/*!

  constructor. get all value from parameter and initialize board.

*/
/******************************************************************************/
Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback)
{
	cb = callback;
	type = stype;
	stats.basesize = basesize;
	totalsize = basesize * basesize * basesize * basesize;
	board = new char[totalsize];
	check = new int[totalsize];
	for (int i = 0; i < totalsize; i++)
	{
		board[i] = '.';
		check[i] = 0;
	}
}

/******************************************************************************/
/*!

  destructor. delete all memory

*/
/******************************************************************************/
Sudoku::~Sudoku()
{
	delete[] board;
	delete[] check;
}

/******************************************************************************/
/*!

  The client (driver) passed the board in the values parameter
  copy all value, and if value is set, change const array to 1

*/
/******************************************************************************/
void Sudoku::SetupBoard(const char *values, int size)
{
	if (totalsize == size)
	{
		for (int i = 0; i < totalsize; i++)
		{
			check[i] = 0;
		}

		for (int i = 0; i < size; i++)
		{
			if (values[i] != '.')
			{
				check[i] = 1;
			}
			board[i] = values[i];
		}
	}
}

/******************************************************************************/
/*!

  private function.
  Check conflict about the index param
  type parameter (1 = check row conflict, 2 = column, 3 = box)

*/
/******************************************************************************/
int Sudoku::Checking(int index, int type)
{
	int size = stats.basesize * stats.basesize;

	int row = index / size;
	int col = index % size;

	//check row conflict
	if (type == 1)
	{
		for (int i = 0; i < size; i++)
		{
			if (row * size + i != index)
			{
				if (board[index] == board[row * size + i])
				{
					return row * size + i;
				}
			}
		}
	}
	//check column conflict
	else if (type == 2)
	{
		for (int i = 0; i < size; i++)
		{
			if (index != col + i * size)
			{
				if (board[index] == board[col + i * size])
				{
					return col + i * size;
				}
			}
		}
	}
	//check box conflict
	else
	{
		int boxrow, boxcol;
		boxrow = row / stats.basesize;
		boxcol = col / stats.basesize;

		for (int i = 0; i < stats.basesize; i++)
		{
			for (int j = 0; j < stats.basesize; j++)
			{
				if (index != boxrow * size * stats.basesize + boxcol * stats.basesize + i * size + j)
				{
					if (board[index] == board[boxrow * size * stats.basesize + boxcol * stats.basesize + i * size + j])
					{
						return boxrow * size * stats.basesize + boxcol * stats.basesize + i * size + j;
					}
				}
				
			}
		}

	}
	return -1;
}
/******************************************************************************/
/*!

  private function.
  recursive function for solving sudoku

*/
/******************************************************************************/
Sudoku::MessageType Sudoku::Solving(int index, char start)
{
	//check sell is const
	if (check[index] == 1)
	{
		if (index + 1 == totalsize)
		{
			return MSG_FINISHED_OK;
		}
		MessageType mtemp = Solving(index + 1, start);
		return mtemp;
	}
	else
	{
		for (char i = start; i < start + stats.basesize * stats.basesize; i++)
		{
			int dup[3] = { -1, -1, -1 };
			int conflictflag = 0;

			//You will send MSG_ABORT_CHECK immediately before you place a value or remove a value. 
			//If this call returns true, you will terminate the search.
			bool abtch = cb(*this, board, MSG_ABORT_CHECK, stats.moves, stats.basesize, index, board[index], dup);
			if (abtch == true)
			{
				return MSG_ABORT_CHECK;
			}
			//Place a value at the specified location
			board[index] = i;
			//Increment the number of moves taken.
			stats.moves++;
			
			//conflict check
			for (int j = 0; j < 3; j++)
			{
				dup[j] = Checking(index, j + 1);

				if (dup[j] >= 0)
				{
					conflictflag = 1;
				}
			}
			//After you place a value on the board, you will send MSG_PLACING.
			cb(*this, board, MSG_PLACING, stats.moves, stats.basesize, index, board[index], dup);

			//If there is no conflict with any neighboring values then
			if (conflictflag == 0)
			{
				//If this was the last position left on the board:
				//The algorithm has finished, nothing left to do but to perform the proper callback.
				if (index + 1 == totalsize)
				{
					stats.placed++;
					//If, after placing a value you have filled the board, you will send MSG_FINISHED_OK and terminate the search.
					return MSG_FINISHED_OK;
				}
				//If there are more empty cells:
				else
				{
					//call place_move recursively with the location of the next empty cell.
					stats.placed++;
					MessageType mtemp = Solving(index + 1, start);
					
					if (mtemp == MSG_FINISHED_OK || mtemp == MSG_ABORT_CHECK)
					{
						return mtemp;
					}
					
					//if return value need backtracking but there is maximum number in this cell. call back track again
					if (mtemp == MSG_REMOVING && i == '1' + 16 * type + stats.basesize * stats.basesize - 1)
					{
						abtch = cb(*this, board, MSG_ABORT_CHECK, stats.moves, stats.basesize, index, board[index], dup);
						if (abtch == true)
						{
							return MSG_ABORT_CHECK;
						}
						board[index] = '.';
						stats.placed--;
						stats.backtracks++;
						cb(*this, board, MSG_REMOVING, stats.moves, stats.basesize, index, board[index], dup);
						return MSG_REMOVING;
					}
				}
			}
			//If there is a conflict:
			else
			{
				//If the value is 9:
				//Backtrack by removing the 9
				if (i == '1' + 16 * type + stats.basesize * stats.basesize - 1)
				{
					//You will send MSG_ABORT_CHECK immediately before you place a value or remove a value. 
					//If this call returns true, you will terminate the search.
					abtch = cb(*this, board, MSG_ABORT_CHECK, stats.moves, stats.basesize, index, board[index], dup);
					if (abtch == true)
					{
						return MSG_ABORT_CHECK;
					}

					board[index] = '.';
					stats.placed--;
					stats.backtracks++;
					//After removing a value from the board, you will send MSG_REMOVING.
					cb(*this, board, MSG_REMOVING, stats.moves, stats.basesize, index, board[index], dup);
					return MSG_REMOVING;
				}
				//If the value is less than 9:
				//Remove that value, increment it, and place it back in the cell and check for conflicts.
			}
		}
	}
	//will not happen
	return MSG_ABORT_CHECK;
}
/******************************************************************************/
/*!

  Once the board is setup, this will start the search for the solution

*/
/******************************************************************************/
void Sudoku::Solve()
{
	int tempa[3] = { -1, -1, -1 };
	MessageType message;

	//When you start the algorithm (the client calls Solve), you will send MSG_STARTING.
	cb(*this, board, MSG_STARTING, stats.moves, stats.basesize, 0, board[0], tempa);

	message = Solving(0, (char)('1' + 16 * type));
	
	if (message == MSG_REMOVING)
	{
		//according to my solution, i did backtracking first cell if there is no solution
		//so, its fake backtracking, i delete count;
		stats.backtracks--;
		stats.placed++;
		//If you do not find a solution after exhaustively checking, you will send MSG_FINISHED_FAIL.
		//if first empty cell call removing(back track), it means there is no solution
		message = MSG_FINISHED_FAIL;
	}

	cb(*this, board, message, stats.moves, stats.basesize, 0, board[0], tempa);
}

/******************************************************************************/
/*!

  For debugging with the driver
  return current board

*/
/******************************************************************************/
// For debugging with the driver
const char* Sudoku::GetBoard() const
{
	return board;
}

/******************************************************************************/
/*!

  For debugging with the driver
  return current stats

*/
/******************************************************************************/
Sudoku::SudokuStats Sudoku::GetStats() const
{
	return stats;
}