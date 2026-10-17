#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

namespace graph
{
	using nodeid_t = uint32_t;
	
	struct edge_t
	{
    nodeid_t target;	// node index
    float cost;       // traversal cost (positive)
    float clearance;  // passage radius/clearance on this edge (used for visibility heuristics)
	};
	
	struct node_t
	{
    vector3 position;
    std::vector<edge_t> edges;
	};
	
	class graph_t
	{
		public:
    	graph_t(void) = default;
	
    	// add a node and return its nodeid_t
    	nodeid_t add_node(Vec3 pos) {
        	m_nodes.push_back(Node{pos, {}});
        	return m_nodes.size() - 1;
    	}
	
    	// add directed edge u -> v with cost and clearance
    	void add_edge(nodeid_t u, nodeid_t v, float cost, float clearance = 0.0f) {
        	m_nodes.at(u).edges.push_back(Edge{v, cost, clearance});
    	}
	
    	// convenience: add bidirectional edges
    	void add_bidirectional_edge(NodeId a, NodeId b, float cost, float clearance = 0.0f)
    	{
				add_edge(a,b,cost,clearance);
				add_edge(b,a,cost,clearance);
    	}
	
    	std::size_t size() const noexcept { return m_nodes.size(); }
    	const node_t& node(nodeid_t id) const { return m_nodes.at(id); }
    	const std::vector<node_t>& nodes() const noexcept { return m_nodes; }
	
	private:
    	std::vector<node_t> m_nodes;
	};
}
