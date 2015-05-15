
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <thread>
#include <stack>
#include <fstream>
#include <list>
#include <math.h>
#include <float.h>
#include <time.h>
#include <ctime>
#include <cstdlib>
using namespace std;

//randomly generate nodes' coordinates, the first one is the gateway node,located in the middle
void randomize_nodes(vector<pair<float, float> > &nodes){
  int n=nodes.size();
  nodes[0].first = 5;
  nodes[0].second = 5;
    
  srand(time(0));
  for(int i=1;i<n;i++){
    nodes[i].first = rand()%100;
    nodes[i].first /=10;
    nodes[i].second = rand()%100;
    nodes[i].second /=10;
  }
}

//the distance matrix generated according to nodes' coordinates, if distance is larger than 3, two nodes cannot communicate
vector<vector<float> > distanceMetricGenerate(vector<pair<float, float> > &nodes){
  int num_nodes = nodes.size();
  vector<vector<float> > distance(num_nodes,vector<float>(num_nodes,0));
  for(int i=0;i<num_nodes;++i){
    for (int j=0;j<num_nodes; ++j) {
      float d_square = (nodes[i].first-nodes[j].first)*(nodes[i].first-nodes[j].first)+(nodes[i].second-nodes[j].second)*(nodes[i].second-nodes[j].second);
      float d = sqrt(d_square);
      if(d<3){
	distance[i][j]=d;
      }
      else
	distance[i][j]=15;
    }
  }
    
    
  return distance;
}


//Part of Dijkstra, looking for the node to process
int min_path(vector<float> &shortest_distance,vector<bool> &visited){
  int result = -1;
  float distance = 15;
  for(int i=0;i<shortest_distance.size();++i){
    if(visited[i]==false && shortest_distance[i]<distance){
      result = i;
      distance = shortest_distance[i];
    }
  }
  return result;
}



//regular Dijkstra and parent nodes information storing
void dijkstra(vector<vector<float> > &distance, vector<float> &shortest_distance, vector<vector<int> > &predecessor){
  int n=distance.size();
  vector<bool> visited(n,false);
    
  for(int i=0;i<n;++i){
    int current_node = min_path(shortest_distance,visited);
    if(current_node == -1)
      break;
    int current_distance = shortest_distance[current_node];
        
    visited[current_node] = true;
    for(int j=0;j<n;++j){
      if(visited[j]==false && distance[current_node][j]>0 && distance[current_node][j]<15 && current_distance + distance[current_node][j] < shortest_distance[j]){
	shortest_distance[j] = current_distance + distance[current_node][j];
	predecessor[j].insert(predecessor[j].begin(),current_node);
      }
    }
  }
}


//Depth First Search, looking for all paths that have enough battery, among which the first one would be the shortest path
void DFS(vector<vector<int> > &predecessor, vector<bool> &valid_node, int current_node, vector<int> &path,vector<vector<int> > &all_path){
  if(current_node==0){
    all_path.push_back(path);
    return;
  }
  if(!predecessor[current_node].empty()){
    for(int i=0;i<predecessor[current_node].size();++i){
      if(valid_node[predecessor[current_node][i]]){
	path.push_back(predecessor[current_node][i]);
	DFS(predecessor, valid_node, predecessor[current_node][i],path,all_path);
	path.pop_back();
      }
    }
  }
}

//Once the shortest path is found, we are going to consume the battery of the nodes along that path, if a node runs out of battery, or all its parent nodes run out of battery, the node fails
void power_consume(vector<int> &path, vector<bool> &valid_node, vector<float> &battery, vector<vector<float> > &distance, vector<vector<int> > &predecessor){
  for(int i=path.size()-2; i>=0; --i){
    battery[path[i]] -= distance[path[i]][path[i+1]]*distance[path[i]][path[i+1]]*0.01;
    if(battery[path[i]]<0){
      valid_node[path[i]]=false;
    }
    if(!valid_node[path[i]] && i-1>=0 && predecessor[path[i-1]].back()==path[i]){
      valid_node[path[i-1]]=false;
    }
  }
}

int main(){
  int memory = 8;                 //memory size, have values 1,2,4 and 8 in simulation
  int signal_total =400;          //number of signals, have values 200 and 400 in simulation
    
  int num_nodes = 60;             //fixed value
  int signal_received=0;
    
    
  vector<pair<float, float> > nodes(num_nodes,pair<float, float>{0,0});       //vector to store nodes' coordinates
  vector<vector<int> > predecessor(num_nodes,vector<int>());                  //vector to store each node's parent nodes
    
  vector<float> shortest_distance(num_nodes,15);                              //used in Dijkstra
  shortest_distance[0]=0;
    
  vector<bool> valid_node(num_nodes,true);                                    //whether a node has enough battery and at least one valid parent node
  vector<float> battery(num_nodes,1.0);                                       //battery infomation
    
    
    
  randomize_nodes(nodes);                                                     //coordinates generating
  vector<vector<float> > distance = distanceMetricGenerate(nodes);            //distance matrix generating
    
  dijkstra(distance,shortest_distance,predecessor);                           //Dijkstra Algorithm, parents node information being stored
    
  for(int i=0;i<predecessor.size();++i){                                  //Resize the vector of parent nodes in order to meet the memory size requirement
    if(predecessor[i].size()>memory)
      predecessor[i].resize(memory);
  }
    
  for(int i=0;i<signal_total;++i){
    int random =rand()%num_nodes;
    if(random ==0)
      continue;
    if(!valid_node[random])
      continue;
    vector<int> path;
    path.push_back(random);
    vector<vector<int> > all_path;
    DFS(predecessor,valid_node,random,path,all_path);                   //depth first search, looking for all available paths
        
    if(all_path.size()>0){
      signal_received++;                                              //successful trasmission
      path=all_path[0];                                               //path is the shortest path
    }
    power_consume(path,valid_node,battery,distance,predecessor);        //consume the battery of the nodes in the shortes path
  }
    
  float ratio = 100*signal_received/signal_total;                         //transmisson rate for this single case
    
  cout<<"The signal transmission ratio for this condition (Number of Signals: "<<signal_total<<", Memory Size: "<<memory<<") is "<<ratio<<"%."<<endl;
  return 0;
}
