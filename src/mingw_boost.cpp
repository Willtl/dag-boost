// More References
// http://marko-editor.com/articles/graph_connected_components/
// http://programmingexamples.net/wiki/Boost/BGL/CreateGraph
// https://stackoverflow.com/questions/5056520/algorithm-for-selecting-all-edges-and-vertices-connected-to-one-vertex

#include <iostream>
#include <string>
#include <deque>
#include <iterator>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>

using namespace std;

struct Vertex {
	enum {
		Operation, Unavailability
	} type;
	string label;
};

struct Edge {
	enum {
		Routing, Resource
	} type;
	double weight;
};

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
		Vertex, Edge> graph_t;
typedef typename boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
typedef typename boost::graph_traits<graph_t>::edge_descriptor edge_t;

void print_graph1(const graph_t &graph);
void print_graph2(const graph_t &graph);
bool topologicalSort(const graph_t &graph, deque<vertex_t> &topologicalSorted);
void print_adjacencyList(const graph_t &graph);

int main() {
	graph_t graph;
	//store all vertices
	std::vector<vertex_t> vertices;

	//create vertices and add
	vertex_t o11, o21, o31, o41;
	o11 = boost::add_vertex(graph);
	o21 = boost::add_vertex(graph);
	o31 = boost::add_vertex(graph);
	o41 = boost::add_vertex(graph);

	//acess vertices and change properties
	graph[o11].label = "11";
	graph[o11].type = Vertex::Operation;
	graph[o21].label = "21";
	graph[o11].type = Vertex::Operation;
	graph[o31].label = "31";
	graph[o11].type = Vertex::Operation;
	graph[o41].label = "41";
	graph[o11].type = Vertex::Operation;

	//create edges conecting vertices
	edge_t e1121, e1131, e2141, e3141;
	bool b;
	boost::tie(e1121, b) = boost::add_edge(o11, o21, graph);
	boost::tie(e1131, b) = boost::add_edge(o11, o31, graph);
	boost::tie(e2141, b) = boost::add_edge(o21, o41, graph);
	boost::tie(e3141, b) = boost::add_edge(o31, o41, graph);

	//acess edges and change properties
	graph[e1121].weight = 10;
	graph[e1121].type = Edge::Routing;

	//custom print graph with edges weight
	print_graph1(graph);

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
}

void print_graph1(const graph_t &graph) {
	cout << "Graph:" << endl;
	auto edges = boost::edges(graph);
	for (auto it = edges.first; it != edges.second; ++it) {
		edge_t edge = *it;
		vertex_t source = boost::source(*it, graph);
		vertex_t target = boost::target(*it, graph);

		cout << graph[source].label << " -> " << graph[target].label
				<< " weight: " << graph[edge].weight << endl;
	}
}

void print_graph2(const graph_t &graph) {
	boost::write_graphviz(cout, graph);
}

bool topologicalSort(const graph_t &graph, deque<vertex_t> &topologicalSorted) {
	try {
		boost::topological_sort(graph, front_inserter(topologicalSorted));
	} catch (boost::not_a_dag &) {
		cout << "not a dag" << endl;
		return false;
	}
	return true;
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
