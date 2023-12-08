//--------------------------------------------------------------------------------------------------
// PI Reachability&HcPE Index
//--------------------------------------------------------------------------------------------------
#ifndef GRAPH_H_
#define GRAPH_H_
//--------------------------------------------------------------------------------------------------
#include <deque>
#include <map>
#include <string>
#include <vector>
//--------------------------------------------------------------------------------------------------
class Graph {
private:
  unsigned n, m;
  std::vector<std::vector<unsigned> > nb; // successors
  std::vector<std::vector<unsigned> > pd; // predecessors
  std::vector<unsigned> deg;              // number of outgoing edges
  std::vector<unsigned> indeg;            // number of incoming edges
  std::deque<unsigned> leaves;            // leaf nodes
  std::vector<unsigned> roots;            // root nodes
public:
  Graph(const std::string& filename);
  ~Graph();

  const std::vector<unsigned>* get_neighbors(unsigned node) const;

  std::vector<unsigned>* get_roots();

  void clear_nb(unsigned v);
  void remove_nb(unsigned node, unsigned target);

  inline std::deque<unsigned>* get_leaves() {
    return &leaves;
  }

  inline std::vector<unsigned>* get_degrees() {
    return &deg;
  }

  inline std::vector<unsigned>* get_indegrees() {
    return &indeg;
  }

  const inline std::vector<unsigned>* get_predecessors(unsigned v) const {
    return &pd[v];
  }

  inline unsigned num_nodes() const {
    return n;
  }
};
//--------------------------------------------------------------------------------------------------
#endif /* GRAPH_H_ */
