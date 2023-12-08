//--------------------------------------------------------------------------------------------------
// 2-hop based on tol
//--------------------------------------------------------------------------------------------------
#include "Index.h"
// #include "omp.h"
//--------------------------------------------------------------------------------------------------
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <string.h>
#include <queue>
#include <cmath>
#include <unordered_set>
#include <queue>
//--------------------------------------------------------------------------------------------------
//! Constructor.
/*!
 * \param g pointer to graph
 * \param root dummy root of the graph.
 */
Index::Index(Graph* g_, unsigned rt){
  g = g_;
  n_ = g->num_nodes();
  root = rt;

  //level approximation.
  levelNodes = std::vector<std::vector<unsigned> >(n_);
  level = std::vector<unsigned>(n_,0);
  inscores = std::vector<int>(n_,0);
  outscores = std::vector<int>(n_,0);
  approximated_scores = std::vector<int>(n_,0);
  // calculated_order 

  IN = std::vector<std::unordered_set<unsigned> >(n_);
  OUT = std::vector<std::unordered_set<unsigned> >(n_);
  // colors = std::vector<int> (n_,0);
}
//--------------------------------------------------------------------------------------------------
//! Destructor.
Index::~Index() {
}
//--------------------------------------------------------------------------------------------------
unsigned Index::levelFilter(unsigned node){
  // std::cout<<"Reaching node: "<<node<< "levels: " << level[node] <<std::endl; //debug process
  if (level[node] != 0){
      return 0;
  }
  const std::vector<unsigned> *nb = g->get_neighbors(node);  //get the nb list of node.
  for (std::vector<unsigned>::const_iterator it = nb->begin(); it != nb->end(); ++it) {
    levelFilter(*it);
  }

  if (nb->size() == 0){
      level[node] = 1;
      levelNodes[1].push_back(node);

      // ++Ctr2;//for interval labeling. 
      // left[node] = Ctr2;
      // right[node] = Ctr2;
      // std::cout<< node << " : " << left[node]<<", "<< right[node] << std::endl;

      return 0;
  }
  else{
    unsigned maxChildren = 0;

    // ++Ctr2;//for interval
    // unsigned minleft = Ctr2;

    for (unsigned i=0; i<nb->size(); i++){
        if (level[(*nb)[i]] > maxChildren) {
            maxChildren = level[(*nb)[i]];
        }
        // if (left[(*nb)[i]] < minleft){//for interval
        //   minleft = left[(*nb)[i]];
        // }
    }
    level[node] = maxChildren + 1;
    levelNodes[maxChildren + 1].push_back(node);

    // left[node] = minleft;
    // right[node] = Ctr2;
    // std::cout<< node << " : " << left[node]<<", "<< right[node] << std::endl;

    return 0;
  }
}

void Index::out_score(){
  // std::vector<unsigned>* deg = g->get_indegrees();
  for (unsigned i = 0; i<=level[root]; ++i){
    std::vector<unsigned> levelset = levelNodes[i];
    if (i==1){//set to 0
      for (std::vector<unsigned>::iterator it=levelset.begin(); it!=levelset.end(); ++it){
        outscores[*it] = 0;
      }
    }
    else{
      for (std::vector<unsigned>::iterator it=levelset.begin(); it!=levelset.end(); ++it){
        const std::vector<unsigned> *nb = g->get_neighbors(*it); 
        for (std::vector<unsigned>::const_iterator it2=nb->begin(); it2!=nb->end();++it2){
          outscores[*it] = outscores[*it] + outscores[*it2] + 1;
          // outscores[*it] = outscores[*it] + (outscores[*it2] + 1)/(*deg)[*it2];
        }
      }
    }
  }
}

void Index::in_score(){
  // std::vector<unsigned>* deg = g->get_degrees();
  for (unsigned i = level[root]; i>=1; i--){
    std::vector<unsigned> levelset = levelNodes[i];
    if (i==level[root]){//set to 0
      for (std::vector<unsigned>::iterator it=levelset.begin(); it!=levelset.end(); ++it){
        inscores[*it] = 0;
      }
    }
    else{
      for (std::vector<unsigned>::iterator it=levelset.begin(); it!=levelset.end(); ++it){
        const std::vector<unsigned> *pd = g->get_predecessors(*it); 
        for (std::vector<unsigned>::const_iterator it2=pd->begin(); it2!=pd->end();++it2){
          inscores[*it] = inscores[*it] + inscores[*it2] + 1;
          // inscores[*it] = inscores[*it] + (inscores[*it2] + 1)/(*deg)[*it2];
        }
      }
    }
  }
}

void Index::calculate_order(){
  for (unsigned i=0; i<n_; ++i){
    int temp1 = inscores[i] * outscores[i] + inscores[i] + outscores[i];

    if (temp1 > 0) {//if not overflow
      double temp_score = (double)temp1/(double)(inscores[i] + outscores[i]);
      queue_unit temp(i, temp_score);
      pq_node_score.push(temp);
    }
    else {
      double temp_score = (double)(2147483647)/(double)(inscores[i] + outscores[i]);
      queue_unit temp(i, temp_score);
      pq_node_score.push(temp);
      std::cout << i << " node OVERFLOW!!!!!\n";
    }
    
  }

  std::cout<< "calculation done ...\n";

  for (unsigned i=0; i<n_; ++i){//extract order
    // size_t maxIndex = argmax(approximated_scores.begin(), approximated_scores.end());
    // calculated_order.push_back(maxIndex);
    // std::cout<< i << ':' << pq_node_score.top().node << ";" << pq_node_score.top().score << std::endl;
    // approximated_scores[maxIndex] = -1;//erase.
    calculated_order.push_back(pq_node_score.top().node);
    pq_node_score.pop();
  }
}

void Index::tol_2hop(){
  // std::vector<unsigned> order;//random order instead near optimal.
  // for (unsigned i=0; i<n_; ++i){
  //   order.push_back(i);
  // }
  // random_shuffle(order.begin(),order.end());
  std::unordered_set<unsigned>bPlus;
  std::unordered_set<unsigned>bMinus;
  for (unsigned i=0; i<n_; ++i){
    // Timer t; t.start();
    bPlus.clear();
    bMinus.clear();
    visited.clear();

    // unsigned currentNode = order[i];//random version
    unsigned currentNode = calculated_order[i];

    // Timer t; t.start();
    //1. 找出所有reach currentNode的点。
    dfsvisit_reverse(currentNode, &bMinus);
    //2. 找出所有currentNode 能够到达的点
    dfsvisit(currentNode, &bPlus);
    // double dfstime=t.stop();
    // std::cout<< "erase time in updating in this round is: " << dfstime << std::endl;

    //3. 更新label
    if (bPlus.find(currentNode) != bPlus.end()) bPlus.erase(currentNode);
    if (bMinus.find(currentNode) != bMinus.end()) bMinus.erase(currentNode);

    
    // Timer t; t.start();//time consuming: optimize.
    for (std::unordered_set<unsigned>::iterator itb=bPlus.begin(); itb != bPlus.end(); ++itb){
      unsigned item = *itb;
      unsigned flag = 1;
      for (std::unordered_set<unsigned>::iterator it=OUT[currentNode].begin(); it!=OUT[currentNode].end(); ++it){
        if (IN[item].find(*it) != IN[item].end()){//if intersected
          flag=0;
          break;
        }
      }
      if (flag==1) IN[item].insert(currentNode);//if no intersect.
    }
    // double dfstime=t.stop();
    // std::cout<< "transition time in this round is: " << dfstime << std::endl;

    // Timer t; t.start();//time consuming: optimize.
    for (std::unordered_set<unsigned>::iterator itb=bMinus.begin(); itb != bMinus.end(); ++itb){
      unsigned item = *itb;
      unsigned flag = 1;
      for (std::unordered_set<unsigned>::iterator it=IN[currentNode].begin(); it!=IN[currentNode].end(); ++it){
        if (OUT[item].find(*it) != OUT[item].end()){//if intersected
          flag=0;
          break;
        }
      }
      if (flag==1) OUT[item].insert(currentNode);//if no intersect.
    }
    // double dfstime=t.stop();
    // std::cout<< "transition time in this round is: " << dfstime << std::endl;


    //4. Delete the information of current node. manipulate on the original graph
    // Timer t; t.start();
    g->clear_nb(currentNode);

    for (unsigned h=0; h<n_; ++h){
      g->remove_nb(h,currentNode);
    }

    // double roundtime = t.stop();
    // debug_time = debug_time + roundtime;
    // std::cout << "round " << i << " construction time: " << roundtime << std::endl;
    // debug_time = debug_time + roundtime;
    if (i%100==0) std::cout<< i << std::endl;//std::cout<< i << "; till now total construction time: " << i << '\n';//check point
  }

  for (unsigned i=0; i<n_; ++i){
    IN[i].insert(i);
    OUT[i].insert(i);
  }
}

unsigned Index::dfsvisit(unsigned x, std::unordered_set<unsigned> *Set){
  if (Set->find(x) != Set->end()) return 0;

  Set->insert(x);

  const std::vector<unsigned> *nb = g->get_neighbors(x);

  for (unsigned i=0; i<nb->size(); ++i){
    dfsvisit((*nb)[i], Set);
  }
  
  return 0;
}

unsigned Index::dfsvisit_reverse(unsigned x, std::unordered_set<unsigned> *Set){
  if (Set->find(x) != Set->end()) return 0;

  Set->insert(x);

  const std::vector<unsigned> *pd = g->get_predecessors(x);

  for (unsigned i=0; i<pd->size(); ++i){
    dfsvisit_reverse((*pd)[i], Set);
  }
  
  return 0;
}

bool Index::reachability(unsigned u, unsigned v){
  // std::cout<< OUT[u].size()<< "; " << IN[v].size() << std::endl;
  if (u==v) return true;
  if (level[u] <= level[v]) return false;
  for (std::unordered_set<unsigned>::iterator it=IN[v].begin(); it!=IN[v].end(); ++it){
    if (OUT[u].find(*it) != OUT[u].end()) {
      // debug_time = debug_time + t.stop();
      // std::cout<<level[u] << "; " << level[v]<< '\n';
      return true;
    }
  }
  // debug_time = debug_time + t.stop();
  return false;

}

void Index::storage(){
  float totalsize = 0;
  int outs = 0;
  int ins = 0;
  for(unsigned i = 0; i<n_; ++i){
    // std::cout<< "node " << i << "in&out:" <<IN[i].size() << ";" << OUT[i].size() << std::endl;
    totalsize = totalsize + IN[i].size()*4/pow(10,6);//prior labels
    totalsize = totalsize + OUT[i].size()*4/pow(10,6);//post labels
    ins = ins + IN[i].size();
    outs = outs + OUT[i].size();
  }
  //totalsize = totalsize + n_ * 4;//level order should be ignored since we add a dummy root
  std::cout<< "index size (MB): " << totalsize << std::endl;
  std::cout<< "ins: " << ins << std::endl;
  std::cout<< "outs: " << outs << std::endl;
}

double Index::get_debugtime(){
  return debug_time;
}

void Index::express(){
  for (unsigned i=0; i<n_; ++i){
    std::cout<< i << ": level is " << level[i] << "; outscore: " << outscores[i] << "; inscore: " << inscores[i] << std::endl;
  }
}

