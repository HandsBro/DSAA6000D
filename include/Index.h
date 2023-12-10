#ifndef INDEX_H_
#define INDEX_H_
//--------------------------------------------------------------------------------------------------
#include "Graph.h"
#include "Timer.h"
//--------------------------------------------------------------------------------------------------
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <queue>
//--------------------------------------------------------------------------------------------------
// #include <boost/dynamic_bitset.hpp>
//--------------------------------------------------------------------------------------------------
struct queue_unit{
	int node;
	double score;
	queue_unit(int a, double sdg){
		node = a;
		score = sdg;
	}
	bool operator<(const queue_unit& a) const{
		return score < a.score;
	}
};

class Index {
private:
  Graph *g;
  unsigned root;
  unsigned n_;

  std::vector<std::vector<unsigned> > levelNodes;
  std::vector<unsigned> level;
  std::vector<double> inscores;
  std::vector<double> outscores;
  std::vector<int> approximated_scores;
  std::vector<unsigned> calculated_order;
  std::priority_queue<queue_unit> pq_node_score;

  std::vector<std::unordered_set<unsigned> > IN;
  std::vector<std::unordered_set<unsigned> > OUT;
  double debug_time = 0.0;
  std::unordered_set<unsigned> visited;


public:
  /// constructor
  Index(Graph *g, unsigned rt);

  /// destructor
  ~Index();

  ///public to access in the main function.
  
  void out_score();
  void in_score();
  void calculate_order();
  unsigned levelFilter(unsigned node);

  void tol_2hop();
  unsigned dfsvisit(unsigned x, std::unordered_set<unsigned> *Set);
  unsigned dfsvisit_reverse(unsigned x, std::unordered_set<unsigned> *Set);
  void express();
  void storage();
  bool reachability(unsigned u, unsigned v);
  double get_debugtime();
  void clear_dummy();

  // accessors
  inline Graph* get_graph() const {
    return g;
  }

  //argmax/min
  template<class ForwardIterator>
  inline size_t argmin(ForwardIterator first, ForwardIterator last)
  {
      return std::distance(first, std::min_element(first, last));
  }

  template<class ForwardIterator>
  inline size_t argmax(ForwardIterator first, ForwardIterator last)
  {
      return std::distance(first, std::max_element(first, last));
  }

};
//--------------------------------------------------------------------------------------------------
#endif /* INDEX_H_ */
