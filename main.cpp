// 2hop reproduce. Implemented by Han Linghu.
#include "Graph.h"
#include "Timer.h"
#include "Index.h"
//--------------------------------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>

//read in queries
void read_queries(const std::string& query_file,
    std::vector<std::pair<unsigned, unsigned> > *queries) {
  std::ifstream qf(query_file.c_str(), std::ios::in);
  if (qf.is_open()) {
    std::string line;
    unsigned s, t;
    if (!qf.eof()) {
      while (qf.good()) {
        getline(qf, line);
        if (line.length()) {
          std::istringstream iss(line, std::istringstream::in);
          iss >> s;
          iss >> t;
          queries->push_back(std::make_pair(s, t));
        }
      }
    }
  }
}

int main(int argc, char *argv[]) {
  std::string graph_file = "", query_file = "";
  for (int i = 0; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-g") {         // graph file
      graph_file = argv[++i];
    }
    else if (arg == "-q") {  // query file
      query_file = argv[++i];
    }
  }

  //1. read in graphs and query files
  std::cout << "graph file: " << graph_file << std::endl;
  std::cout << "query file: " << query_file << std::endl;
  Graph *g_ = new Graph(graph_file);
  std::vector<std::pair<unsigned, unsigned> > queries;
  read_queries(query_file, &queries);
  
  // unsigned reachable = 0;//total # of reachabile sets, onle effective for reachability query

  //init Index 
  unsigned rt = g_->num_nodes()-1;//can be 0 in the test files.
  Index id(g_,rt);
  std::cout << "Init over ... root is: " << rt << std::endl;

  Timer t1; t1.start();
  Timer t2; t2.start();
  id.levelFilter(rt);
  id.in_score();
  id.out_score();
  // id.clear_dummy();
  id.calculate_order();
  // id.express();
  std::cout << "Order design over ..." << std::endl;
  double orderdesign = t2.stop();
  id.tol_2hop();
  double constructiontime = t1.stop();

  // id.express();
  id.storage();

  // double positiveSearchTime = 0.0;
  // double negativeSearchTime = 0.0;
  unsigned reachable = 0;//total # of reachabile sets, onle effective for reachability query
  // //reachability single test
  // Timer t; t.start(); //1334 2691
  // bool answer = id.reachability2(u,v);//single test: debug use
  // std::cout << "Reachable?: " << answer << std::endl;
  // double searchTime = t.stop();
  Timer t; t.start(); //1334 2691
  for (unsigned i=0; i<queries.size(); ++i){
    // std::cout<< queries[i].first << " " <<queries[i].second  << " round: " << i<< std::endl;
    // completePathInProcess.clear();
    // Timer t; t.start(); //1334 2691
    bool answer = id.reachability(queries[i].first,queries[i].second);
    if (answer) {
      // positiveSearchTime = positiveSearchTime + t.stop();
      ++reachable;
    }
    // else negativeSearchTime = negativeSearchTime + t.stop();
  }
  double reachabilityTime = t.stop(); 

  // std::cout<< "Total Path # is: " << completePathInProcess.size() << std::endl;
  // std::cout << "debug time: (" << id.get_debugtime() << " ms)" << std::endl;
  std::cout << "order time: (" << orderdesign << " ms)" << std::endl;
  std::cout << "construction: (" << constructiontime << " ms)" << std::endl;
  std::cout << "Reachability time: (" << reachabilityTime << " ms) and reachability answer is: " << reachable << std::endl;;
  // std::cout << "Reachability time: (" << positiveSearchTime+negativeSearchTime << " ms)" << std::endl;
  // std::cout << "positive Reachability time: (" << positiveSearchTime << " ms) negative search time (" << negativeSearchTime << " ms) and reachability answer is: " << reachable << std::endl;

  delete g_;
  return 0;
}