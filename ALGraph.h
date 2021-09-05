/******************************************************************************/
/*!
\file   ALGraph.h
\author Junki Kim
\par    junki.kim\@digipen.edu
\par    DigiPen login: junki.kim
\par    Course: CS280
\par    Assignment 6
\date   07/19/2019
\brief
This file contains interface of an adjaceny list graph
that uses Directed edge and Undirected edge and Dijkstra algorithm.
and some traversing method if i have time.

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
//---------------------------------------------------------------------------
#ifndef ALGRAPHH
#define ALGRAPHH
//---------------------------------------------------------------------------
#include <vector>    //std::vector
#include <algorithm> //std::sort

/*!
  Information used to communicate with the driver after Dijkstra's algorithm
  has run. There will typically be one of these per node in the graph.
*/
struct DijkstraInfo
{
  unsigned cost;              //!< The cost to reach this node
  std::vector<unsigned> path; //!< The shortest path that led to this node
};

/*!
  Stores information for the adjacency lists
*/
struct AdjacencyInfo
{
  unsigned id;     //!< The node's unique ID
  unsigned weight; //!< The weigth of the edge leading to this node
};

//! Convenient shorthand for an adjacency list type
typedef std::vector<std::vector<AdjacencyInfo> > ALIST;


/*!
  Stores information for the node
*/

/*!
  Class that represents a graph.
*/
class ALGraph
{
  public:
    //! The different types of traversals
    enum TRAVERSAL_METHOD {DEPTH_FIRST, BREADTH_FIRST};

    ALGraph(unsigned size);
    ~ALGraph();
    void AddDEdge(unsigned source, unsigned destination, unsigned weight);
    void AddUEdge(unsigned node1, unsigned node2, unsigned weight);

    std::vector<unsigned> SearchFrom(unsigned start_node, TRAVERSAL_METHOD method) const;
    std::vector<DijkstraInfo> Dijkstra(unsigned start_node) const;
    ALIST GetAList() const;
    static bool ImplementedSearches();

  private:
	  //! adjacency list for saving all node and edge data
	  mutable ALIST list_;
	  //! unsigned integer for saving maximum node count
	  unsigned size_;
	  //! flag for checking AList is sorted
	  mutable bool sorted;
};

#endif
