#ifndef UFO_PLAN_GRAPH_HPP
#define UFO_PLAN_GRAPH_HPP

// UFO
#include <ufo/container/tree_map.hpp>
#include <ufo/map/occupancy/predicate.hpp>
#include <ufo/math/vec.hpp>

// STL
#include <cstdint>
#include <limits>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ufo
{
template <std::size_t Dim = 3>
class NavMap
{
 public:
	using Coord = Vec<Dim, float>;

	using node_t = std::uint32_t;
	using edge_t = std::uint32_t;

	static constexpr node_t const NULL_NODE     = std::numeric_limits<node_t>::max();
	static constexpr edge_t const NULL_EDGE     = std::numeric_limits<edge_t>::max();
	static constexpr float const  NULL_COST     = std::numeric_limits<float>::infinity();
	static constexpr float const  NULL_DISTANCE = std::numeric_limits<float>::infinity();

 private:
	struct Node : public Coord {
		float cost{};

		edge_t first_edge{};
		edge_t last_edge{};

		constexpr Node() = default;

		constexpr Node(Coord const& coord, float cost) : Coord(coord), cost(cost) {}
	};

	struct Edge {
		node_t node = NULL_NODE;

		float cost     = NULL_COST;
		float distance = NULL_DISTANCE;

		constexpr Edge() = default;

		constexpr Edge(node_t node, float cost, float distance)
		    : node(node), cost(cost), distance(distance)
		{
		}
	};

 public:
	struct Path {
		std::vector<node_t> nodes;
		float               cost{};
		float               distance{};
	};

	// Iterators
	using iterator       = typename TreeMap<Dim, node_t>::iterator;
	using const_iterator = typename TreeMap<Dim, node_t>::const_iterator;

	[[nodiscard]] Path plan(node_t start, node_t goal, float distance_weight = 1.0f,
	                        float edge_cost_weight = 1.0f, float node_cost_weight = 1.0f,
	                        float heuristic_weight = 1.0f) const
	{
		std::vector<node_t> starts{start};
		return plan(starts, goal, distance_weight, edge_cost_weight, node_cost_weight,
		            heuristic_weight);
	}

	[[nodiscard]] Path plan(std::vector<node_t> const& starts, node_t goal,
	                        float distance_weight = 1.0f, float edge_cost_weight = 1.0f,
	                        float node_cost_weight = 1.0f,
	                        float heuristic_weight = 1.0f) const
	{
		auto cost_f = [this, distance_weight, edge_cost_weight,
		               node_cost_weight](edge_t edge) {
			auto const& e = edges_[edge];
			auto const& n = nodes_[e.node];
			return distance_weight * e.distance + edge_cost_weight * e.cost +
			       node_cost_weight * n.cost;
		};

		return plan(starts, goal, cost_f, heuristic_weight);
	}

	template <class CostFun>
	[[nodiscard]] Path plan(node_t start, node_t goal, CostFun cost_f,
	                        float heuristic_weight = 1.0f) const
	{
		std::vector<node_t> starts{start};
		return plan(starts, goal, cost_f, heuristic_weight);
	}

	template <class CostFun>
	[[nodiscard]] Path plan(std::vector<node_t> const& starts, node_t goal, CostFun cost_f,
	                        float heuristic_weight = 1.0f) const
	{
		auto heuristic_f = [this, heuristic_weight](node_t cur, node_t goal) {
			return heuristic_weight * ufo::distance(coord(cur), coord(goal));
		};

		return plan(starts, goal, cost_f, heuristic_f);
	}

	template <class CostFun, class HeuristicFun>
	[[nodiscard]] Path plan(node_t start, node_t goal, CostFun cost_f,
	                        HeuristicFun heuristic_f) const
	{
		std::vector<node_t> starts{start};
		return plan(starts, goal, cost_f, heuristic_f);
	}

	template <class CostFun, class HeuristicFun>
	[[nodiscard]] Path plan(std::vector<node_t> const& starts, node_t const& goal,
	                        CostFun cost_f, HeuristicFun heuristic_f) const
	{
		assert(!starts.empty());
		assert(std::all_of(starts.begin(), starts.end(),
		                   [this](node_t n) { return NULL_NODE != n && nodes_.size() > n; }));
		assert(NULL_NODE != goal && nodes_.size() > goal);

		auto comp = [](auto const& a, auto const& b) { return a.second > b.second; };

		using OpenSet =
		    std::priority_queue<std::pair<node_t, float>,
		                        std::vector<std::pair<node_t, float>>, decltype(comp)>;
		using ClosedSet = std::unordered_set<node_t>;
		using CameFrom  = std::unordered_map<node_t, node_t>;
		using GScore    = std::unordered_map<node_t, float>;

		OpenSet   open(comp);
		ClosedSet closed;
		CameFrom  came_from;
		GScore    g_score;

		for (node_t start : starts) {
			g_score[start] = 0.0f;
			float f_score  = heuristic_f(start, goal);
			open.emplace(start, f_score);
		}

		node_t cur = NULL_NODE;
		while (!open.empty() && goal != (cur = open.top().first)) {
			open.pop();

			if (!closed.insert(cur).second) {
				// We have already processed this node
				continue;
			}

			Node const& n  = nodes_[cur];
			float       gs = g_score[cur];
			for (edge_t e = n.first_edge; n.last_edge > e; ++e) {
				node_t neighbor = edges_[e].node;

				if (NULL_DISTANCE == edges_[e].distance || 0 < closed.count(neighbor)) {
					continue;
				}

				float tentative_g_score = gs + cost_f(e);
				if (auto [it, inserted] = g_score.try_emplace(neighbor, tentative_g_score);
				    inserted || tentative_g_score < it->second) {
					came_from[neighbor] = cur;
					it->second          = tentative_g_score;
					float f_score       = tentative_g_score + heuristic_f(neighbor, goal);
					open.emplace(neighbor, f_score);
				}
			}
		}

		Path path{};

		if (goal != cur) {
			// Could not find a path
			path.distance = -1.0f;
			return path;
		}

		path.cost = g_score[goal];

		// Reconstruct paths
		path.nodes.push_back(goal);
		for (auto it = came_from.find(goal); came_from.end() != it;
		     it      = came_from.find(path.nodes.back())) {
			path.nodes.push_back(it->second);
		}

		std::reverse(path.nodes.begin(), path.nodes.end());

		for (std::size_t i = 1; path.nodes.size() > i; ++i) {
			Edge const& e = edges_[edge(path.nodes[i - 1], path.nodes[i])];
			path.distance += e.distance;
		}

		return path;
	}

	template <class Map>
	void update(Map const& map, float robot_radius)
	{
		unsigned min_depth{};
		for (; map.numDepthLevels() > min_depth; ++min_depth) {
			if (min(map.length(min_depth)) > robot_radius) {
				break;
			}
		}

		std::vector<TreeIndex> candidate_nodes;
		auto                   pred = pred::Free() && pred::Depth() >= min_depth;
		for (auto node : map.query(pred)) {
			candidate_nodes.push_back(node);
		}

		std::vector<TreeCoord<Dim>> nodes;
		nodes.reserve(candidate_nodes.size());
		for (auto node : candidate_nodes) {
			auto bb = map.boundingBox(node) + robot_radius;
			auto p  = !pred::Free() && pred::Intersects(bb);
			for (auto n : map.query(p)) {
				// TODO: Implement
			}
		}

		map_.clear(map.length(min_depth), map.resolution(min_depth));

		for (auto node : nodes) {
			Coord coord = map.center(node);
			auto  p     = !pred::Free() && pred::Intersects(Sphere(coord, robot_radius));
			for (auto n : map.query(p)) {
				continue;
			}

			// TODO: Implement
		}

		// TODO: Implement
	}

	template <class Map>
	void updateModified(Map const& map)
	{
		// TODO: Implement
	}

	void clear()
	{
		map_.clear();
		nodes_.clear();
		edges_.clear();
		free_nodes_.clear();
		free_edges_.clear();
		num_nodes_ = {};
		num_edges_ = {};
	}

	[[nodiscard]] std::size_t numNeighbors(node_t node) const
	{
		Node const& n = nodes_[node];
		std::size_t num{};
		for (edge_t e = n.first_edge; n.last_edge > e; ++e) {
			if (NULL_DISTANCE != edges_[e].distance) {
				++num;
			}
		}
		return num;
	}

	[[nodiscard]] std::vector<node_t> neighbors(node_t node) const
	{
		assert(NULL_NODE != node && nodes_.size() > node);
		auto const&         n = nodes_[node];
		std::vector<node_t> res;
		res.reserve(n.last_edge - n.first_edge);
		for (edge_t e = n.first_edge; n.last_edge > e; ++e) {
			if (NULL_DISTANCE != edges_[e].distance) {
				res.push_back(edges_[e].node);
			}
		}
		return res;
	}

	[[nodiscard]] Coord const& coord(node_t node) const
	{
		assert(NULL_NODE != node && nodes_.size() > node);
		return static_cast<Coord const&>(nodes_[node]);
	}

	[[nodiscard]] float& cost(node_t node)
	{
		assert(NULL_NODE != node && nodes_.size() > node);
		return nodes_[node].cost;
	}

	[[nodiscard]] float cost(node_t node) const
	{
		assert(NULL_NODE != node && nodes_.size() > node);
		return nodes_[node].cost;
	}

	[[nodiscard]] float& cost(node_t node, node_t neighbor)
	{
		auto e = edge(node, neighbor);
		assert(NULL_EDGE != e);
		return edges_[e].cost;
	}

	[[nodiscard]] float cost(node_t node, node_t neighbor) const
	{
		auto e = edge(node, neighbor);
		assert(NULL_EDGE != e);
		return edges_[e].cost;
	}

	[[nodiscard]] float& distance(node_t node, node_t neighbor)
	{
		auto e = edge(node, neighbor);
		assert(NULL_EDGE != e);
		return edges_[e].distance;
	}

	[[nodiscard]] float distance(node_t node, node_t neighbor) const
	{
		auto e = edge(node, neighbor);
		assert(NULL_EDGE != e);
		return edges_[e].distance;
	}

	node_t addNode(Coord const& position, float cost)
	{
		node_t node = nodes_.size();
		nodes_.emplace_back(position, cost);
		map_.emplace(position, node);
		++num_nodes_;
		return node;
	}

	bool eraseNode(node_t const& node)
	{
		assert(validNode(node));

		// Remove from map
		Coord coord(node);
		for (auto it = map_.beginNearest(coord), last = map_.endNearest(); last != it; ++it) {
			if (it->second == node) {
				map_.erase(it);
				break;
			} else if (it->first != coord) {
				return false;
			}
		}

		--num_nodes_;

		// Remove edges
		// TODO: Implement
		// auto const& n = nodes_[node];
		// if (n.first_edge != n.last_edge) {
		// 	for (edge_t edge = n.first_edge; n.last_edge > edge; ++edge) {
		// 		// TODO: Implement
		// 		eraseBiEdge(node, edges_[edge].node);
		// 	}

		// 	for (edge_t edge = n.first_edge; n.last_edge > edge; edge += edges_mul_) {
		// 		free_edges_.push_back(edge);
		// 	}
		// }

		// Remove node
		free_nodes_.push_back(node);

		return false;
	}

	bool addEdge(node_t start, node_t end, float cost = 0.0f)
	{
		assert(validNode(start));
		assert(validNode(end));
		return addEdge(start, end, cost, ufo::distance(coord(start), coord(end)));
	}

	bool addEdge(node_t start, node_t end, float cost, float distance)
	{
		assert(validNode(start));
		assert(validNode(end));

		edge_t e          = edge(end, start);
		float  cost_2     = NULL_EDGE == e ? NULL_COST : edges_[e].cost;
		float  distance_2 = NULL_EDGE == e ? NULL_DISTANCE : edges_[e].distance;

		return 0 != addBiEdge(start, end, cost, cost_2, distance, distance_2);
	}

	int addBiEdge(node_t node_1, node_t node_2, float cost = 0.0f)
	{
		assert(validNode(node_1));
		assert(validNode(node_2));
		return addBiEdge(node_1, node_2, cost, ufo::distance(coord(node_1), coord(node_2)));
	}

	int addBiEdge(node_t node_1, node_t node_2, float cost, float distance)
	{
		assert(validNode(node_1));
		assert(validNode(node_2));
		return addBiEdge(node_1, node_2, cost, cost, distance, distance);
	}

	int addBiEdge(node_t node_1, node_t node_2, float cost_1, float cost_2,
	              float distance_1, float distance_2)
	{
		assert(validNode(node_1));
		assert(validNode(node_2));

		int num_added{};
		num_added += NULL_DISTANCE == distance_1 ? 0 : 1;
		num_added += NULL_DISTANCE == distance_2 ? 0 : 1;

		if (auto e_1 = edge(node_1, node_2); NULL_EDGE != e_1) {
			// Edge already exists, update cost and distance
			edge_t e_2 = edge(node_2, node_1);
			assert(validEdge(e_2));

			num_added -= NULL_DISTANCE == edges_[e_1].distance ? 0 : 1;
			num_added -= NULL_DISTANCE == edges_[e_2].distance ? 0 : 1;

			edges_[e_1].cost     = cost_1;
			edges_[e_2].cost     = cost_2;
			edges_[e_1].distance = distance_1;
			edges_[e_2].distance = distance_2;

			return num_added;
		}

		Node& n_1 = nodes_[node_1];
		Node& n_2 = nodes_[node_2];

		std::size_t const s   = edges_.size();
		std::size_t const s_1 = numEdges(node_1);
		std::size_t const s_2 = numEdges(node_2);

		std::size_t const a_1 = numAllocatedEdges(node_1);
		std::size_t const a_2 = numAllocatedEdges(node_2);

		std::size_t n_a_1 = a_1;
		std::size_t n_a_2 = a_2;

		edge_t e_1_first = n_1.first_edge;
		edge_t e_2_first = n_2.first_edge;

		std::size_t num_new_edges{};

		if (s_1 == a_1) {
			n_a_1 += edges_mul_;

			if (auto it = free_edges_.find(n_a_1);
			    free_edges_.end() != it && !it->second.empty()) {
				e_1_first = it->second.back();
				it->second.pop_back();
			} else {
				e_1_first = s;
				num_new_edges += n_a_1;
			}
		}
		if (s_2 == a_2) {
			n_a_2 += edges_mul_;

			if (auto it = free_edges_.find(n_a_2);
			    free_edges_.end() != it && !it->second.empty()) {
				e_2_first = it->second.back();
				it->second.pop_back();
			} else {
				e_2_first = e_1_first == s ? s + n_a_1 : s;
				num_new_edges += n_a_2;
			}
		}

		if (0 != num_new_edges) {
			edges_.resize(edges_.size() + num_new_edges);
		}

		if (e_1_first != n_1.first_edge) {
			std::copy(edges_.begin() + n_1.first_edge, edges_.begin() + n_1.last_edge,
			          edges_.begin() + e_1_first);
			if (0 != a_1) {
				free_edges_[a_1].push_back(n_1.first_edge);
			}
			n_1.first_edge = e_1_first;
		}
		if (e_2_first != n_2.first_edge) {
			std::copy(edges_.begin() + n_2.first_edge, edges_.begin() + n_2.last_edge,
			          edges_.begin() + e_2_first);
			if (0 != a_2) {
				free_edges_[a_2].push_back(n_2.first_edge);
			}
			n_2.first_edge = e_2_first;
		}

		n_1.last_edge = e_1_first + s_1 + 1;
		n_2.last_edge = e_2_first + s_2 + 1;

		edges_[n_1.last_edge - 1] = Edge{node_2, cost_1, distance_1};
		edges_[n_2.last_edge - 1] = Edge{node_1, cost_2, distance_2};

		return num_added;
	}

	bool eraseEdge(node_t start, node_t end)
	{
		assert(validNode(start));
		assert(validNode(end));

		if (!isNeighbor(start, end)) {
			return false;
		}

		if (isNeighbor(end, start)) {
			// Bi-directional edge, so do not purge
			edge_t e  = edge(start, end);
			edges_[e] = Edge{end, NULL_COST, NULL_DISTANCE};
			return true;
		}

		// Directional edge, so purge

		// TODO: Implement

		return true;
	}

	int eraseBiEdge(node_t node_1, node_t node_2)
	{
		int num_erased{};
		num_erased += eraseEdge(node_1, node_2) ? 1 : 0;
		num_erased += eraseEdge(node_2, node_1) ? 1 : 0;
		return num_erased;
	}

	[[nodiscard]] node_t nearestNode(node_t node) const
	{
		assert(validNode(node));
		for (auto const& [_, n] : map_.nearest(coord(node))) {
			if (node == n) {
				continue;
			}
			return n;
		}
		return NULL_NODE;
	}

	template <class Geometry>
	[[nodiscard]] node_t nearestNode(Geometry const& geometry) const
	{
		for (auto const& [_, node] : map_.nearest(geometry)) {
			return node;
		}
		return NULL_NODE;
	}

	[[nodiscard]] std::vector<node_t> kNearestNodes(node_t node, unsigned k) const
	{
		assert(validNode(node));
		std::vector<node_t> res;
		res.reserve(k);
		for (auto const& [_, n] : map_.nearest(coord(node))) {
			if (node == n) {
				continue;
			}
			if (0 == k) {
				break;
			}
			--k;
			res.push_back(n);
		}
		return res;
	}

	template <class Geometry>
	[[nodiscard]] std::vector<node_t> kNearestNodes(Geometry const& geometry,
	                                                unsigned        k) const
	{
		std::vector<node_t> res;
		res.reserve(k);
		for (auto const& [_, node] : map_.nearest(geometry)) {
			if (0 == k) {
				break;
			}
			--k;
			res.push_back(node);
		}
		return res;
	}

	template <class Geometry>
	[[nodiscard]] std::vector<node_t> intersectingNodes(Geometry const& geometry) const
	{
		std::vector<node_t> res;
		for (auto const& [_, node] : map_.query(pred::Intersects(geometry))) {
			res.push_back(node);
		}
		return res;
	}

	iterator begin() { return map_.begin(); }

	iterator end() { return map_.end(); }

	const_iterator begin() const { return map_.cbegin(); }

	const_iterator end() const { return map_.cend(); }

	const_iterator cbegin() const { return begin(); }

	const_iterator cend() const { return end(); }

	[[nodiscard]] bool validNode(node_t node) const
	{
		return NULL_NODE != node && nodes_.size() > node;
	}

	[[nodiscard]] bool isNeighbor(node_t node, node_t neighbor) const
	{
		edge_t e = edge(node, neighbor);
		return NULL_EDGE != e && NULL_DISTANCE != edges_[e].distance;
	}

	[[nodiscard]] std::size_t numNodes() const { return num_nodes_; }

	[[nodiscard]] std::size_t numEdges() const { return num_edges_; }

 private:
	[[nodiscard]] edge_t edge(node_t node, node_t neighbor) const
	{
		assert(validNode(node));
		assert(validNode(neighbor));
		auto const& n = nodes_[node];
		for (edge_t e = n.first_edge; n.last_edge > e; ++e) {
			if (edges_[e].node == neighbor) {
				return e;
			}
		}
		return NULL_EDGE;
	}

	[[nodiscard]] bool validEdge(edge_t edge) const
	{
		return NULL_EDGE != edge && edges_.size() > edge;
	}

	[[nodiscard]] std::size_t numEdges(node_t node) const
	{
		assert(validNode(node));
		Node const& n = nodes_[node];
		return n.last_edge - n.first_edge;
	}

	[[nodiscard]] std::size_t numAllocatedEdges(node_t node) const
	{
		std::size_t const s = numEdges(node);
		return edges_mul_ * ((s + (edges_mul_ - 1)) / edges_mul_);
	}

 private:
	TreeMap<Dim, node_t> map_;

	std::vector<Node> nodes_;
	std::vector<Edge> edges_;

	std::size_t edges_mul_ = 10;

	std::vector<node_t>                        free_nodes_;
	std::map<std::size_t, std::vector<node_t>> free_edges_;

	std::size_t num_nodes_{};
	std::size_t num_edges_{};
};
}  // namespace ufo

#endif  // UFO_PLAN_GRAPH_HPP