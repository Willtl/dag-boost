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
		Operation, Unavailability
	} type;
	string label;
	double starting = 0;
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

void print_graph_routing(const graph_t &graph);
void print_graph_resource(const graph_t &graph);
void print_graph(const graph_t &graph);
bool topologicalSort(const graph_t &graph, deque<vertex_t> &topologicalSorted);
void longestPath(graph_t &graph, const deque<vertex_t> &topologicalSorted);
void longestPathOnlyOneEdge(graph_t &graph, const deque<vertex_t> &topologicalSorted);
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
	edge_t e1121, e1131, e2141, e3141, e11212;
	bool b;
	boost::tie(e1121, b) = boost::add_edge(o11, o21, graph);
	boost::tie(e1131, b) = boost::add_edge(o11, o31, graph);
	boost::tie(e2141, b) = boost::add_edge(o21, o41, graph);
	boost::tie(e3141, b) = boost::add_edge(o31, o41, graph);

	boost::tie(e11212, b) = boost::add_edge(o11, o21, graph);

	//acess edges and change properties
	graph[e1121].weight = 1234 * 0.5;
	graph[e1121].type = Edge::Routing;
	graph[e1131].weight = 4512 * 0.5;
	graph[e1131].type = Edge::Routing;
	graph[e2141].weight = 3123 * 0.5;
	graph[e2141].type = Edge::Routing;
	graph[e3141].weight = 3125 * 0.5;
	graph[e3141].type = Edge::Routing;

	graph[e11212].weight = 5000 * 0.5;
	graph[e11212].type = Edge::Resource;

	//custom print graph with edges weight
	print_graph_routing(graph);
	print_graph_resource(graph);

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
void print_graph_resource(const graph_t &graph) {
	cout << "Resource Edges:" << endl;
	auto edges = boost::edges(graph);
	for (auto it = edges.first; it != edges.second; ++it) {
		edge_t edge = *it;
		vertex_t source = boost::source(*it, graph);
		vertex_t target = boost::target(*it, graph);

		if (graph[edge].type == Edge::Resource) {
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
	for (int i = 0; i < topologicalSorted.size(); i++) {
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
					|| std::find(visited.begin(), visited.end(), adjacentID)
							== visited.end()) {
				boost::graph_traits<graph_t>::out_edge_iterator ei, ei_end;
				boost::tie(ei, ei_end) = out_edges(vertexID, graph);
				int parallel_count = 0;

				for (; ei != ei_end; ++ei) {
					if (target(*ei, graph) == adjacentID) {
						edge_t edge = *ei;
						double path = graph[vertexID].starting + graph[edge].weight;
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
