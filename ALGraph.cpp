/******************************************************************************/
/*!
\file   ALGraph.cpp
\author Junki Kim
\par    junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment 6
\date   07/19/2019
\brief
This file contains implementation of an adjaceny list graph
that uses Directed edge and Undirected edge and Dijkstra algorithm.
and some traversing method if i have time.

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "ALGraph.h"

namespace
{
/******************************************************************************/
/*!
  \brief comparebycost
  This helper function uses for third parameter of std::sort 
  which is comparison function. it get two parameter for 
  comparing the weight(and id if weight is same)
  and return bool value depend on comparison.
  \param a
  AdjacencyInfo structure for compare with parameter b
  \param b
  other AdjacencyInfo structure for compare with parameter a
  \return
  returns the bool value depend on comparison.
*/
/******************************************************************************/
	bool comparebycost(const AdjacencyInfo &a, const AdjacencyInfo &b)
	{
		//if a has small weight, return true (Lower is better)
		if (a.weight < b.weight)
		{
			return true;
		}
		//if a and b has same weight, check id to find priority
		else if (a.weight == b.weight)
		{
			return a.id < b.id;
		}
		//if a has bigger value than b, return false 
		else
		{
			return false;
		}
	}
}


/******************************************************************************/
/*!
  \brief ALGraph
  ALGraph's default constructor. Constructs an adjaceny list graph
  base on a given size value which is maximum node.
  initializing private member variable.

  \param size
  the size of graph which is how many node is in graph.

*/
/******************************************************************************/
ALGraph::ALGraph(unsigned size)
{
	//saving size of the graph in member
	size_ = size;
	sorted = false;
	//add empty vector to AList
	for (unsigned i = 0; i < size; i++)
	{
		std::vector<AdjacencyInfo> temp;
		list_.push_back(temp);
	}
}

/******************************************************************************/
/*!
  \brief ~ALGraph
  ALGraph's default destructor. 
  surprisingly do nothing.

*/
/******************************************************************************/
ALGraph::~ALGraph()
{
}

/******************************************************************************/
/*!
  \brief AddDEdge
  This function uses for adding directed edge to given destination node from 
  given source node. also adding edge's cost depend on given weight value.
  \param source
  unsigned integer which saving node's unique id. edge will start from 
  the node which has this id.
  \param destination
  unsigned integer which saving node's unique id. edge will end at 
  the node which has this id.
  \param weight
  unsigned integer which saving edge's weight(cost).
*/
/******************************************************************************/
void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
{
	AdjacencyInfo edge;    // create new edge value
	edge.id = destination; // saving destination of the edge
	edge.weight = weight;  // saving cost of the edge

	list_[(source - 1)].push_back(edge); // saving at source node's Adjacency list.
	
	//putted the value, should sort again
	sorted = false;
	/*
	//if source node's adjacency list need sort, do it by using compare helper function.
	if (list_[(source - 1)].size() > 1)
	{
		std::sort(list_[(source - 1)].begin(), list_[(source - 1)].end(), comparebycost);
	}
	*/
}

/******************************************************************************/
/*!
  \brief AddUEdge
  This function uses for adding undirected edge between two given nodes. 
  also adding edge's cost depend on given weight value. 
  it's undirected edge, which means add 2 directed edge to each other.
  \param node1
  unsigned integer which saving node's unique id. edge will set 
  between this and node2.
  \param node2
  unsigned integer which saving node's unique id. edge will set 
  between this and node1.
  \param weight
  unsigned integer which saving edge's weight(cost).
*/
/******************************************************************************/
void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
{
	//it's undirected edge, which means add 2 directed edge to each other.
	AdjacencyInfo edge1;   
	edge1.id = node2;      
	edge1.weight = weight; 

	AdjacencyInfo edge2;
	edge2.id = node1;
	edge2.weight = weight;

	//put the edge in right position
	list_[(node1 - 1)].push_back(edge1);
	list_[(node2 - 1)].push_back(edge2);

	//putted the value, should sort again
	sorted = false;
	/*
	//if node's adjacency list need sort, do it by using compare helper function.
	if (list_[(node1 - 1)].size() > 1)
	{
		std::sort(list_[(node1 - 1)].begin(), list_[(node1 - 1)].end(), comparebycost);
	}

	if (list_[(node2 - 1)].size() > 1)
	{
		std::sort(list_[(node2 - 1)].begin(), list_[(node2 - 1)].end(), comparebycost);
	}
	*/
}


/******************************************************************************/
/*!
  \brief SearchFrom
  This function uses for traversing the graph from given start_node by 
  given method.

  \param start_node
  unsigned integer which saving node's unique id. traversing will start
  from the node which has this id.
  \param method
  The different types of traversals (DEPTH_FIRST, BREADTH_FIRST)
  \return
  return the vector which have unsigned integers, which is traversing path.
*/
/******************************************************************************/
std::vector<unsigned> ALGraph::SearchFrom(unsigned start_node, TRAVERSAL_METHOD method) const
{
	//for unused variable
	std::vector<unsigned> temp;
	if (method == DEPTH_FIRST)
	{
		temp.push_back(start_node);
	}
	else
	{
		temp.push_back(start_node);
	}
	return temp;
}

/******************************************************************************/
/*!
  \brief Dijkstra
  This function uses for getting shorteset path by using Dijkstra algorithm.
  each path will start from given start_node. 
  this function will return vector of DijkstraInfo which save shortest path
  to every other node from start_node.

  \param start_node
  unsigned integer which saving node's unique id. every path will start
  from the node which has this id.
  but if the other node is unconnected with start_node, no path.

  \return
  return the vector which have DijkstraInfo sturcture, which contain
  shortest path to every other node from start_node.
*/
/******************************************************************************/
std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const
{
	//initializing Prior Queue for Dijkstra algorithm.
	//PQ need to save node's ID and cost of it.
	//so I used AdjacencyInfo which have id and weight(cost) instead of queue.
	//all i need to do is sorting after putting value 
	//and get item from front of the vector.
	std::vector<AdjacencyInfo> priorq;
	//initialize returned data which will save shortest path to 
	//every other node from start_node.
	std::vector<DijkstraInfo> djvec;
	//bool value for checking node is evaluated or not
	bool* eval = new bool[size_];
	//checking how many nodes are evaluated. for Pseudocode
	//"While there are nodes in the graph that haven't been evaluated"
	unsigned evalcount = 0;
	//for setting unsigned value to infinity
	const unsigned INFINITY_ = static_cast<unsigned>(-1);

	
	//before all thing, check AList is sorted
	if (sorted == false)
	{
		for (unsigned i = 0; i < size_; i++)
		{
			if (list_[i].size() > 1)
			{
				std::sort(list_[i].begin(), list_[i].end(), comparebycost);
			}
		}
		sorted = true;
	}
	
	//Initializing part
	for (unsigned i = 0; i < size_; i++)
	{
		DijkstraInfo temp; // for putting value in stack

		//Initialize source to 0 cost and mark as evaluated.
		if (i == (start_node - 1))
		{
			temp.cost = 0; //Initialize source to 0 cost
			temp.path.push_back(start_node); //saving path when node is evaluated
			djvec.push_back(temp); //save info to vector
			eval[i] = true; //evaluated
			evalcount++; //check count
		}
		//Initialize all nodes to infinite cost from the source.
		else
		{
			temp.cost = INFINITY_;//Initialize all nodes to infinite cost
			djvec.push_back(temp);//save info to vector
			eval[i] = false;      //not evaluated yet
		}
	}
	//For each node, y, adjacent to source
	for (unsigned i = 0; i < list_[start_node - 1].size(); i++)
	{
		//get id and edge cost from source's neighbor
		unsigned yid = list_[start_node - 1][i].id;
		unsigned ycost = list_[start_node - 1][i].weight;

		//Relax the node. That is, set y's cost to the cost of all edges from source to y.
		djvec[yid - 1].cost = ycost;

		//because cost is changed, i need to update path depend on right cost.
		//clear the path
		djvec[yid - 1].path.clear();
		//instead of hard code, i made logic for getting all path to Y from start_node.
		//the path is [start_node] for now because its first time.
		//but i'll use this logic for later.
		for (unsigned j = 0; j < djvec[start_node - 1].path.size(); j++)
		{
			djvec[yid - 1].path.push_back(djvec[start_node - 1].path[j]);
		}
		//after getting all path from before node, put Y on it.
		//now path is [start_node -> Y].
		djvec[yid - 1].path.push_back(yid);

		//2. Place y into a priority queue based on its total cost. (Lower is better)
		//it doesn't need even if i push it back. because PQ will be sorted.
		priorq.push_back(list_[start_node - 1][i]);

		//not do this cuz AList is already sorted.
		/*
		//sorting priority queue by helper function (Lower is better)
		if (priorq.size() > 1)
		{
			std::sort(priorq.begin(), priorq.end(), comparebycost);
		}
		*/
	}
	//End For

	//While there are nodes in the graph that haven't been evaluated
	while (evalcount != size_)
	{
		//if there are nodes which is not connected with start_node,
		//the node will not evaluated forever, so i added if statement
		//if every connected node will be evaluated, i break the loop.
		if (priorq.size() == 0)
		{
			break;
		}
		//get the first item from the priority queue.
		AdjacencyInfo x = priorq.front();
		//Remove a node, x, from the PQ (lowest total cost)
		//first is lowest cost because its sorted.
		priorq.erase(priorq.begin());

		//If the node has already been evaluated
		if (eval[x.id - 1] == true)
		{
			//Discard the node - let it go~
			//Go to top of while
		}
		//Else
		else
		{
			//Mark x as evaluated.
			eval[x.id - 1] = true;
			evalcount++; //increase count
			
			//For each neighbor, y, of x
			for (unsigned i = 0; i < list_[x.id - 1].size(); i++)
			{
				unsigned yid = list_[x.id - 1][i].id;
				//Relax y : set y's cost to the cost of all edges from source to y.
				unsigned ycost = djvec[x.id - 1].cost + list_[x.id - 1][i].weight;

				//If new cost to reach y is less
				if (ycost < djvec[yid - 1].cost)
				{
					//Update new cost
					djvec[yid - 1].cost = ycost;
					// Update list of nodes (path) to y from source.
					//clear the path
					djvec[yid - 1].path.clear();
					//i can use logic now
					//the result path is [start_node -> ... -> X] after this for loop
					for (unsigned j = 0; j < djvec[x.id - 1].path.size(); j++)
					{
						djvec[yid - 1].path.push_back(djvec[x.id - 1].path[j]);
					}
					//the result path is [start_node -> ... -> X -> Y] after this
					djvec[yid - 1].path.push_back(yid);

					//this is the difference so it's not duplicate.
					//y has different cost. because it has 2+ edges.
					//so i made info for y with right cost.
					AdjacencyInfo y;
					y.id = yid;
					y.weight = ycost;

					//Place y in the PQ.(Lower is better)
					//it doesn't need even if i push it back. because PQ will be sorted.
					//priorq.push_back(y);
					//for better efficient sorting, i put it to front
					priorq.insert(priorq.begin(), y);

					//sorting priority queue by helper function (Lower is better)
					if (priorq.size() > 1)
					{
						//too inefficient way to sort
						//std::sort(priorq.begin(), priorq.end(), comparebycost);

						unsigned count = 0;
						//find the spot that has bigger cost than y's cost
						while (true)
						{
							// it means y has the biggest cost.
							if (priorq.size() == count)
							{
								break;
							}
							//find the spot that has bigger cost than y's cost
							if (ycost < priorq[count].weight)
							{
								break;
							}
							count++;
						}
						//sort only between front to that part
						std::sort(priorq.begin(), priorq.begin()+count, comparebycost);
					}
				}
				//End If
			}
			//End for
		}
		//End If
	}
	//End While

	//free the bool array for checking evaluated.
	delete[] eval;

	return djvec;
}

/******************************************************************************/
/*!
  \brief GetAList
  This function uses for returning ALIST variable which is adjaceny list 

  \return
  return the ALIST variable which is adjaceny list.
*/
/******************************************************************************/
ALIST ALGraph::GetAList() const
{
	if (sorted == false)
	{
		for (unsigned i = 0; i < size_; i++)
		{
			if (list_[i].size() > 1)
			{
				std::sort(list_[i].begin(), list_[i].end(), comparebycost);
			}
		}
		sorted = true;
	}
	return list_;
}

/******************************************************************************/
/*!
  \brief ImplementedSearches
  This function uses for checking if i implemented extra credit stuff

  \return
  return false if i don't have it.
*/
/******************************************************************************/
bool ALGraph::ImplementedSearches()
{
	return false;
}