
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>

using namespace std;

/*
This function reads a line of int values into a vector function and returns that
vector.
*/
vector<long> readlineoflongs(int count) {
    // Input values
    vector<long> linevalues(count);
    for (int j = 0; j < count; j++) {
        long val;
        cin >> val;
        linevalues[j] = val;
    }
    return linevalues; // Return line values as a vector
}

// Edge struct
struct edge {
    long target;
    long weight;

    //Default constructor
    edge() {}

    //Constructor
    edge(long target, long weight) :target(target), weight(weight) {}

    //Override comparator operator
    bool operator<(const edge& rhs) const {
        return weight < rhs.weight;
    }
};

class Edge_Comparator {
public:
    bool operator()(const edge a, const edge b) {
        return (a.weight < b.weight);
    }
};

class Reversed_Edge_Comparator {
public:
    bool operator()(const edge a, const edge b) {
        return (a.weight > b.weight);
    }
};

// Graph code, at least in original part, from
// http://www.geeksforgeeks.org/graph-implementation-using-stl-for-competitive-programming-set-2-weighted-graph/
/*
Adds an undirected edge of set weight to the graph
*/
void addUndirectedEdge(vector<vector<edge>> &adj_list, int u, int v, long weight) {
    edge edge1(v, weight);
    edge edge2(u, weight);
    adj_list[u].push_back(edge1); // Add the v edge and the weight to u's list
    adj_list[v].push_back(edge2); // Add the u edge and the weight to v's list
}

/*
Adds a directed edge of set weight to the graph
*/
void addDirectedEdge(vector<vector<edge>> &adj_list, int u, int v, long weight, long water_level) {
    edge edge1(v, weight);
    adj_list[u].push_back(edge1); // Add the v edge and the weight to u's list
}

/*
Updates the weight of set edge
*/
void updateEdgeWeight(vector<vector<edge>> &adj_list, int u, int v, long new_weight) {
    for (edge &e : adj_list[u]) {
        if (e.target == v) {
            e.weight = new_weight;
        }
    }
}


bool dijkstra(const vector<vector<edge>> &graph, long &water_level, int origin, map<long, long> &control_rooms) {
    vector<long> distance(graph.size(), 2147483647); //Initialize vertices list with lengths set to max
    vector<bool> visited(graph.size(), false); //Initialize visited array
    long lowest_water_level_possible_so_far = water_level;
    long count_visited = 0;
    vector<vector<edge>> edges_removed(water_level+1, vector<edge>(0)); //Create a vector of vectors of size water level
    vector<bool> edges_removed_at_this_level(water_level+1, false); //Flag array to note edges removed

    priority_queue<edge, vector<edge>, Edge_Comparator> to_visit;
    to_visit.push(edge(origin, water_level)); //Push the origin with the current water level as weight as the first item in the queue
    distance[origin] = 0;

    //While the to_visit list still has items
    while (to_visit.size() > 0) {
        edge current_edge = to_visit.top(); //Get next edge to visit
        to_visit.pop(); //Pop it
        if (visited[current_edge.target] != true) { //If we haven't visited the node at the end of this edge yet
            if (current_edge.weight >= lowest_water_level_possible_so_far) { //If the water is potentially low enough to reach this room
                water_level = min(water_level, current_edge.weight); //Lower water level if required for the current node (we've already checked that we could get it to this level)
                visited[current_edge.target] = true; //Mark current node visited
                count_visited++; //Increment visited count

                map<long, long>::iterator control_room = control_rooms.find(current_edge.target); //Check control room map for this node
                if (control_room != control_rooms.end()) { //If it's a control room
                    lowest_water_level_possible_so_far = min(lowest_water_level_possible_so_far, control_room->second); //Set lowest water level possible to min of previous value and that enabled by this control room
                    if (edges_removed.size() > 0) { //If edges have been removed
                        for (int i = water_level; i >= lowest_water_level_possible_so_far; i--) { //As water level is lowered
                            if (edges_removed_at_this_level[i]) { //If there are removed edges at this water level
                                for (int j = 0; j < edges_removed[i].size(); j++) { //Add back removed edges that are now possible
                                    to_visit.push(edges_removed[i][j]);
                                }
                                edges_removed[i].erase(edges_removed[i].begin(), edges_removed[i].end()); //Delete this water level's edges from the edges_removed array
                                edges_removed_at_this_level[i] = false;
                            }
                        }
                    }
                }

                //Iterate through edges of next node
                for (edge next_edge : graph[current_edge.target]) {
                    distance[next_edge.target] = min(next_edge.target, distance[current_edge.target] + next_edge.weight); //Assign distance to target as min of previous distance and distance through this node
                    if (visited[next_edge.target] != true) { //If we haven't visited the node at the end of this edge yet
                        to_visit.push(next_edge); //Insert it into the to visit list
                    }
                }
            }
            else {
                edges_removed[current_edge.weight].push_back(current_edge); //Add the removed edge at its water level
                edges_removed_at_this_level[current_edge.weight] = true;
            }
        }
    }    

    bool visited_everywhere = (count_visited == graph.size()); //Check that we visited all the nodes
    return visited_everywhere;
}


int main() {
    std::ios_base::sync_with_stdio(false);

    //get test case count
    int t;
    std::cin >> t;

    //loop over all the test cases
    for (int i = 1; i <= t; i++) {
        // Input parameters line
        vector<long> params = readlineoflongs(4);
        long n = params[0];
        long m = params[1];
        long k = params[2];
        long l = params[3];

        // Initialize adjacency list graph
        vector<vector<edge>> graph(n);
        map<long,long> control_rooms;

        
        // Input group lines
        for (long j = 0; j < m; j++) {
            vector<long> cur_edge = readlineoflongs(3);
            cur_edge[0]--; //Decrement edge numberings for 0 indexing
            cur_edge[1]--;
            addUndirectedEdge(graph, cur_edge[0], cur_edge[1], cur_edge[2]); // Add edge, with water level, to adjacency list
            
        }
        // Input group lines
        for (long j = 0; j < k; j++) {
            vector<long> cur_room = readlineoflongs(2);
            cur_room[0]--; //Decrement room numberings for 0 indexing
            control_rooms.emplace(cur_room[0], cur_room[1]); //Add room to control_rooms map
        }

        //Perform Dijkstra to check explorability
        bool got_everywhere = dijkstra(graph, l, 0, control_rooms);

        //write output
        if (got_everywhere) {
            std::cout << "Case #" << i << ": " << l << std::endl;
        }
        else {
            std::cout << "Case #" << i << ": impossible" << std::endl;
        }
    }

    return 0;
}

