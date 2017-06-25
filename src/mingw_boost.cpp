// More References
// http://marko-editor.com/articles/graph_connected_components/
// http://programmingexamples.net/wiki/Boost/BGL/CreateGraph
// https://stackoverflow.com/questions/5056520/algorithm-for-selecting-all-edges-and-vertices-connected-to-one-vertex

#include <iostream>
#include <string>
#include <iterator>
#include <vector>		// std::vector
#include <deque>		// std::deque
#include <algorithm>    // std::find

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/breadth_first_search.hpp>

using namespace std;

struct Vertex {
	enum {
		Operation, Unavailability, Dummy
	} type;
	string label;
	double starting = 0;
};

struct Edge {
	enum {
		Routing, Resource, Release
	} type;
	int idResource;
	double weight;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
		Vertex, Edge> graph_t;
typedef typename boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
typedef typename boost::graph_traits<graph_t>::edge_descriptor edge_t;

void print_graph_routing(const graph_t &graph);
void print_graph_resources(const graph_t &graph);
void print_graph_resource(const graph_t &graph, const int &resource);
void print_graph(const graph_t &graph);
bool topologicalSort(const graph_t &graph, deque<vertex_t> &topologicalSorted);
void longestPath(graph_t &graph, const deque<vertex_t> &topologicalSorted);
void longestPathOnlyOneEdge(graph_t &graph,
		const deque<vertex_t> &topologicalSorted);
void print_adjacencyList(const graph_t &graph);

int main() {
	graph_t graph;
	//store all vertices
	std::vector<vertex_t> vertices;

	//create vertices and add
	vertex_t s, o11, o21, o31, o41, o12, o22, o32, t;
	s = boost::add_vertex(graph);

	o11 = boost::add_vertex(graph);
	o21 = boost::add_vertex(graph);
	o31 = boost::add_vertex(graph);
	o41 = boost::add_vertex(graph);

	o12 = boost::add_vertex(graph);
	o22 = boost::add_vertex(graph);
	o32 = boost::add_vertex(graph);

	t = boost::add_vertex(graph);

	//acess vertices and change properties
	//dummy nodes
	graph[s].label = "s";
	graph[s].type = Vertex::Dummy;
	graph[t].label = "t";
	graph[t].type = Vertex::Dummy;

	//job 1 nodes
	graph[o11].label = "11";
	graph[o11].type = Vertex::Operation;
	graph[o21].label = "21";
	graph[o21].type = Vertex::Operation;
	graph[o31].label = "31";
	graph[o31].type = Vertex::Operation;
	graph[o41].label = "41";
	graph[o41].type = Vertex::Operation;

	//job 2 nodes
	graph[o12].label = "12";
	graph[o12].type = Vertex::Operation;
	graph[o22].label = "22";
	graph[o22].type = Vertex::Operation;
	graph[o32].label = "32";
	graph[o32].type = Vertex::Operation;

	//create edges conecting vertices
	//dummy and routing
	edge_t es11, es12, e1121, e1131, e2141, e3141, e41t, e1222, e2232, e32t;
	//resource 1
	edge_t e1122, e2241;
	//resource 2
	edge_t e1221;
	//resource 3
	edge_t e3132;

	bool b;

	//s adjacency
	boost::tie(es11, b) = boost::add_edge(s, o11, graph);
	boost::tie(es12, b) = boost::add_edge(s, o12, graph);
	graph[es11].weight = 0;
	graph[es11].type = Edge::Release;
	graph[es12].weight = 0;
	graph[es12].type = Edge::Release;

	//job 1 routing
	boost::tie(e1121, b) = boost::add_edge(o11, o21, graph);
	boost::tie(e1131, b) = boost::add_edge(o11, o31, graph);
	boost::tie(e2141, b) = boost::add_edge(o21, o41, graph);
	boost::tie(e3141, b) = boost::add_edge(o31, o41, graph);
	boost::tie(e41t, b) = boost::add_edge(o41, t, graph);
	graph[e1121].weight = 8;
	graph[e1121].type = Edge::Routing;
	graph[e1131].weight = 8;
	graph[e1131].type = Edge::Routing;
	graph[e2141].weight = 5;
	graph[e2141].type = Edge::Routing;
	graph[e3141].weight = 2;
	graph[e3141].type = Edge::Routing;
	graph[e41t].weight = 4;
	graph[e41t].type = Edge::Routing;

	//job 2 routing
	boost::tie(e1222, b) = boost::add_edge(o12, o22, graph);
	boost::tie(e2232, b) = boost::add_edge(o22, o32, graph);
	boost::tie(e32t, b) = boost::add_edge(o32, t, graph);
	graph[e1222].weight = 3;
	graph[e1222].type = Edge::Routing;
	graph[e2232].weight = 3;
	graph[e2232].type = Edge::Routing;
	graph[e32t].weight = 5;
	graph[e32t].type = Edge::Routing;

	//resource 1
	boost::tie(e1122, b) = boost::add_edge(o11, o22, graph);
	boost::tie(e2241, b) = boost::add_edge(o22, o41, graph);
	graph[e1122].weight = 8;
	graph[e1122].type = Edge::Resource;
	graph[e1122].idResource = 1;
	graph[e2241].weight = 3;
	graph[e2241].type = Edge::Resource;
	graph[e2241].idResource = 1;

	//resource 2
	boost::tie(e1221, b) = boost::add_edge(o12, o21, graph);
	graph[e1221].weight = 3;
	graph[e1221].type = Edge::Resource;
	graph[e1221].idResource = 2;

	//resource 3
	boost::tie(e3132, b) = boost::add_edge(o31, o32, graph);
	graph[e3132].weight = 2;
	graph[e3132].type = Edge::Resource;
	graph[e3132].idResource = 3;

	//custom print graph with edges weight
	print_graph_routing(graph);
	print_graph_resources(graph);
	print_graph_resource(graph, 1);

	//print ajacency list
	print_adjacencyList(graph);

	//store topological order
	deque<vertex_t> topologicalSorted;

	//perform topological sort
	if (topologicalSort(graph, topologicalSorted)) {
		cout << "The graph is directed acyclic!" << endl;
		cout << "Topological order: ";
		for (int i = 0; i < topologicalSorted.size(); i++) {
			cout << graph[topologicalSorted.at(i)].label << " ";
		}
		cout << endl;
	}
	longestPath(graph, topologicalSorted);
}

//print all edges correspondents to routing constraints
void print_graph_routing(const graph_t &graph) {
	cout << "Routing Edges:" << endl;
	auto edges = boost::edges(graph);
	for (auto it = edges.first; it != edges.second; ++it) {
		edge_t edge = *it;
		vertex_t source = boost::source(*it, graph);
		vertex_t target = boost::target(*it, graph);

		if (graph[edge].type == Edge::Routing) {
			cout << graph[source].label << " -> " << graph[target].label
					<< " weight: " << graph[edge].weight << endl;
		}
	}
}

//print all edges correspondents to resource constraints
void print_graph_resources(const graph_t &graph) {
	cout << "Resource Edges:" << endl;
	auto edges = boost::edges(graph);
	for (auto it = edges.first; it != edges.second; ++it) {
		edge_t edge = *it;
		vertex_t source = boost::source(*it, graph);
		vertex_t target = boost::target(*it, graph);

		if (graph[edge].type == Edge::Resource) {
			cout << graph[edge].idResource << " - "
					<< graph[source].label << " -> " << graph[target].label
					<< " weight: " << graph[edge].weight << endl;
		}
	}
}

//print resource edges correspondents to a specific resource
void print_graph_resource(const graph_t &graph, const int &resource) {
	cout << "Resource Edges of resource " << resource << endl;
	auto edges = boost::edges(graph);
	for (auto it = edges.first; it != edges.second; ++it) {
		edge_t edge = *it;
		vertex_t source = boost::source(*it, graph);
		vertex_t target = boost::target(*it, graph);

		if (graph[edge].type == Edge::Resource && graph[edge].idResource == resource) {
			cout << graph[source].label << " -> " << graph[target].label
					<< " weight: " << graph[edge].weight << endl;
		}
	}
}

//print graph using graphviz lib
void print_graph(const graph_t &graph) {
	boost::write_graphviz(cout, graph);
}

//try to perform topo sort and return true if the graph is a dag
bool topologicalSort(const graph_t &graph, deque<vertex_t> &topologicalSorted) {
	try {
		boost::topological_sort(graph, front_inserter(topologicalSorted));
	} catch (boost::not_a_dag &) {
		cout << "not a dag" << endl;
		return false;
	}
	return true;
}

void longestPath(graph_t &graph, const deque<vertex_t> &topologicalSorted) {
	cout << "Longest Path Procedure" << endl;
	for (auto i : topologicalSorted) {
		vertex_t vertexID = topologicalSorted[i];
		Vertex v = graph[vertexID];
		cout << "Calculating longest path to adjacencies of " << v.label
				<< endl;

		vector<vertex_t> visited;
		auto adjacency = boost::adjacent_vertices(topologicalSorted[i], graph);
		for (auto it = adjacency.first; it != adjacency.second; ++it) {
			vertex_t adjacentID = *it;
			Vertex adj = graph[adjacentID];

			if (visited.empty()
					|| find(visited.begin(), visited.end(), adjacentID)
							== visited.end()) {
				boost::graph_traits<graph_t>::out_edge_iterator ei, ei_end;
				boost::tie(ei, ei_end) = out_edges(vertexID, graph);

				for (; ei != ei_end; ++ei) {
					if (target(*ei, graph) == adjacentID) {
						edge_t edge = *ei;
						double path = graph[vertexID].starting
								+ graph[edge].weight;
						if (graph[adjacentID].starting < path) {
							graph[adjacentID].starting = path;
							cout << "New Longest Path to "
									<< graph[adjacentID].label << " from "
									<< graph[vertexID].label << " weight: "
									<< path << endl;
						}
					};
				};
				visited.push_back(adjacentID);
			}
		}
		cout << endl;
	}
}

void longestPathOnlyOneEdge(graph_t &graph,
		const deque<vertex_t> &topologicalSorted) {
	cout << "Longest Path Procedure" << endl;
	for (int i = 0; i < topologicalSorted.size(); i++) {
		vertex_t vertexID = topologicalSorted[i];
		Vertex v = graph[vertexID];

		//loop through adjacency of each vertex in the graph
		auto adjacency = boost::adjacent_vertices(topologicalSorted[i], graph);
		for (auto it = adjacency.first; it != adjacency.second; ++it) {
			vertex_t adjacentID = *it;
			Vertex adj = graph[adjacentID];

			pair<edge_t, bool> edge = boost::edge(vertexID, adjacentID, graph);
			double path = graph[vertexID].starting + graph[edge.first].weight;
			if (graph[adjacentID].starting < path) {
				graph[adjacentID].starting = path;
				cout << "New Longest Path to " << graph[adjacentID].label
						<< " from " << graph[vertexID].label << " weight: "
						<< path << endl;
			}
		}
		cout << endl;
	}
}

void print_adjacencyList(const graph_t &graph) {
	//loop through all vertices
	auto vertices = boost::vertices(graph);
	for (auto it = vertices.first; it != vertices.second; ++it) {
		vertex_t vertexID = *it;
		Vertex v = graph[vertexID];
		cout << "Vertex " << v.label << " adjacency: ";

		//loop through adjacency of each vertex in the graph
		auto adjacency = boost::adjacent_vertices(vertexID, graph);
		for (auto it2 = adjacency.first; it2 != adjacency.second; ++it2) {
			vertex_t adjacentID = *it2;
			Vertex adj = graph[adjacentID];
			cout << adj.label << " ";
		}
		cout << endl;
	}
}
