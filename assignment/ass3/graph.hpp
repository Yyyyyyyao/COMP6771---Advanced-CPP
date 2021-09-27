#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace gdwg {
	template<typename N, typename E>
	class graph {
	public:
		struct value_type {
			N from;
			N to;
			E weight;
		};

		graph() = default;

		graph(std::initializer_list<N> il) {
			*this = graph(il.begin(), il.end());
		}

		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto& it = first; it != last; ++it) {
				nodes_.emplace(std::make_shared<N>(*it));
			}
		}

		// std::exchange returns old value
		graph(graph&& other) noexcept {
			nodes_ = std::move(other.nodes_);
			edges_ = std::move(other.edges_);
			weights_ = std::move(other.weights_);
		}

		auto operator=(graph&& other) noexcept -> graph& {
			auto copy = graph(std::move(other));
			copy.swap(*this);
			return *this;
		}

		graph(graph const& other) noexcept {
			nodes_.clear();
			edges_.clear();
			std::for_each(other.nodes_.begin(), other.nodes_.end(), [this](auto const& v) {
				insert_node(*v);
			});

			weights_.clear();
			std::for_each(other.weights_.begin(), other.weights_.end(), [this](auto const& v) {
				weights_.emplace(std::make_shared<E>(*v));
			});

			std::for_each(other.edges_.begin(), other.edges_.end(), [this](auto const& v) {
				std::for_each(v.second.begin(), v.second.end(), [this, v](auto const& e) {
					insert_edge(*(v.first), *(e.to), *(e.weight));
				});
			});
		}

		auto operator=(graph const& other) noexcept -> graph& {
			graph copy(other);
			copy.swap(*this);
			return *this;
		}

		auto insert_node(N const& value) noexcept -> bool {
			if (!is_node(value)) {
				nodes_.emplace(std::make_shared<N>(value));
				return true;
			}
			return false;
		}

		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
				                         "or dst node does not exist");
			}

			// Create new_edge for inserting
			auto another_dst = nodes_.find(dst);
			auto new_edge = edge{};
			new_edge.to = *another_dst;

			// If the weight is new, create a base resource for it
			auto another_weight = weights_.find(weight);
			if (another_weight != weights_.end()) {
				new_edge.weight = *another_weight;
			}
			else {
				auto new_weight = std::make_shared<E>(weight);
				weights_.emplace(new_weight);
				new_edge.weight = new_weight;
			}

			auto map_iter = edges_.find(src);
			// if in edges_ map, the incoming src node is first time added
			// I need to create the key and the set
			if (map_iter == edges_.end()) {
				auto node_iter = nodes_.find(src);
				std::set<edge, edge_cmp> empty_set = {new_edge};
				edges_.emplace(*node_iter, empty_set);
			}
			else { // else I insert into the set of edges under the src node
				auto src_out_edges = map_iter->second;
				value_type edge_to_find = {src, dst, weight};
				auto iter = src_out_edges.find(edge_to_find);
				if (iter != src_out_edges.end()) {
					return false;
				}
				map_iter->second.emplace(new_edge);
			}
			return true;
		}

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
				                         "doesn't exist");
			}
			if (is_node(new_data)) {
				return false;
			}
			// in order for the edges map to be in the right order
			// I create new_data node first and merge old with the new
			insert_node(new_data);
			merge_replace_node(old_data, new_data);
			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or "
				                         "new data if they don't exist in the graph");
			}

			for (auto map_iter = edges_.begin(); map_iter != edges_.end();) {
				// three cases
				// 1. for an edge, both src and dst are old data
				// 2. for an edge, src is the old data
				// 3. for an edge, dst is the old data
				// In order to keep the right order,
				// I insert new edge and delete the old one
				for (auto old_edge = map_iter->second.begin(); old_edge != map_iter->second.end();) {
					if (*map_iter->first == old_data && *old_edge->to == old_data) {
						insert_edge(new_data, new_data, *old_edge->weight);
						old_edge = map_iter->second.erase(old_edge);
					}
					else if (*map_iter->first == old_data) {
						insert_edge(new_data, *old_edge->to, *old_edge->weight);
						old_edge = map_iter->second.erase(old_edge);
					}
					else if (*old_edge->to == old_data) {
						insert_edge(*map_iter->first, new_data, *old_edge->weight);
						old_edge = map_iter->second.erase(old_edge);
					}
					else {
						++old_edge;
					}
				}
				// if this src does not have edges,
				// erase it from map
				if (map_iter->second.empty()) {
					map_iter = edges_.erase(map_iter);
				}
				else {
					++map_iter;
				}
			}
			// after done with the edges map
			// I need to delete the old node as well
			auto old_data_it = nodes_.find(old_data);
			nodes_.erase(old_data_it);
		}

		auto erase_node(N const& value) noexcept -> bool {
			auto old_data_it = nodes_.find(value);
			if (old_data_it == nodes_.end()) {
				return false;
			}
			nodes_.erase(old_data_it);
			// I need to loop through the edges map
			// to delete any edges with node value involved
			for (auto map_iter = edges_.begin(); map_iter != edges_.end();) {
				for (auto old_edge = map_iter->second.begin(); old_edge != map_iter->second.end();) {
					if (*map_iter->first == value || *old_edge->to == value) {
						old_edge = map_iter->second.erase(old_edge);
					}
					else {
						++old_edge;
					}
				}
				// if this src key have no edges in its edge set
				// I need to delete the key from edges map as well because it does not have edges
				if (map_iter->second.empty()) {
					map_iter = edges_.erase(map_iter);
				}
				else {
					++map_iter;
				}
			}
			return true;
		}

		// Complexity: log(n)+log(e)
		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
				                         "if "
				                         "they don't exist in the graph");
			}
			// create a value type for searching in edge set of a src key
			value_type edge_to_find = {src, dst, weight};
			// Complexity: log(n) - n is the number of stored nodes
			auto map_iter = edges_.find(src);
			// if the src map does not have edges
			// return false
			if (map_iter == edges_.end()) {
				return false;
			}
			// Complexity: log(e) - e is the number of edges for this src node
			auto iter = map_iter->second.find(edge_to_find);
			if (iter == map_iter->second.end()) {
				return false;
			}
			map_iter->second.erase(iter);
			// if the last edge of this src is deleted
			// delete the src from the edges map as well
			if (map_iter->second.empty()) {
				edges_.erase(map_iter);
			}
			return true;
		}

		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
			weights_.clear();
		}

		// Complexity: O(log(n))
		[[nodiscard]] auto is_node(N const& value) const noexcept -> bool {
			// set find has log complexity based on set size
			auto iter = nodes_.find(value);
			return (iter != nodes_.end());
		}

		[[nodiscard]] auto empty() const noexcept -> bool {
			return nodes_.empty();
		}

		// Complexity: O(log(n)+log(e))
		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}

			// Complexity: log(n)
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return false;
			}
			if (map_iter->second.empty()) {
				return false;
			}
			// Complexity: log(e)
			auto find_edge_iter = map_iter->second.find(dst);
			return find_edge_iter != map_iter->second.end();
		}

		// Complexity: O(n)
		[[nodiscard]] auto nodes() const noexcept -> std::vector<N> {
			auto vec_nodes = std::vector<N>();
			std::for_each(nodes_.begin(), nodes_.end(), [&vec_nodes](auto const& it) {
				vec_nodes.push_back(*it);
			});
			return vec_nodes;
		}

		// Complexity: O(log(n) + e)
		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E> {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}
			auto vec_weights = std::vector<E>();
			// Complexity: log(n)
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return vec_weights;
			}
			if (map_iter->second.empty()) {
				return vec_weights;
			}
			// Complexity: O(e) - e is the numebr of edges from src
			std::for_each(map_iter->second.begin(),
			              map_iter->second.end(),
			              [&dst, &vec_weights](auto const& it) {
				              if (*it.to == dst) {
					              vec_weights.push_back(*it.weight);
				              }
				              return;
			              });

			return vec_weights;
		}

		// Complexity: O(log(n)+e)
		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in "
				                         "the graph");
			}
			auto vec_dsts = std::vector<N>();
			// Complexity: log(n)
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return vec_dsts;
			}
			if (map_iter->second.empty()) {
				return vec_dsts;
			}
			// Complexity: log(e)
			std::for_each(map_iter->second.begin(), map_iter->second.end(), [&vec_dsts](auto const& it) {
				// this is used for find duplicate dst
				// because they may have different weights
				auto f = std::find(vec_dsts.begin(), vec_dsts.end(), *it.to);
				if (f == vec_dsts.end()) {
					vec_dsts.push_back(*it.to);
				}
			});
			return vec_dsts;
		}

		// Complexity: O(n+e)
		[[nodiscard]] auto operator==(graph const& other) const noexcept -> bool {
			// Compleixty: O(n)
			auto flag = std::equal(nodes_.begin(),
			                       nodes_.end(),
			                       other.nodes_.begin(),
			                       other.nodes_.end(),
			                       [](auto const& lhs, auto const& rhs) { return *lhs == *rhs; });
			// if nodes are different
			// there is no need to check edges
			if (flag) {
				// Compleixty: O(e)
				flag = std::equal(edges_.begin(),
				                  edges_.end(),
				                  other.edges_.begin(),
				                  other.edges_.end(),
				                  [](auto const& lhs, auto const& rhs) {
					                  return (*(lhs.first) == *(rhs.first))
					                         && std::equal(lhs.second.begin(),
					                                       lhs.second.end(),
					                                       rhs.second.begin(),
					                                       rhs.second.end(),
					                                       [](auto const& lhs_e, auto const& rhs_e) {
						                                       return *lhs_e.to == *rhs_e.to
						                                              && *lhs_e.weight == *rhs_e.weight;
					                                       });
				                  });
			}
			return flag;
		}

		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			for (auto const& src : g.nodes_) {
				os << *src << " (\n";
				auto vec_dsts = g.connections(*src);
				for (auto const& dst : vec_dsts) {
					auto vec_weights = g.weights(*src, dst);
					for (auto const& weight : vec_weights) {
						os << "  " << dst << " | " << weight << "\n";
					}
				}
				os << ")\n";
			}
			return os;
		}

	private:
		struct edge {
			std::shared_ptr<N> to;
			std::shared_ptr<E> weight;
		};

		struct node_cmp {
			using is_transparent = void;
			auto operator()(std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) const noexcept
			   -> bool {
				return *a < *b;
			}
			auto operator()(std::shared_ptr<N> const& a, N const& b) const noexcept -> bool {
				return *a < b;
			}
			auto operator()(N const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
				return a < *b;
			}
		};

		struct edge_map_cmp {
			using is_transparent = void;
			auto operator()(std::shared_ptr<N> const& a, std::shared_ptr<N> const& b) const noexcept
			   -> bool {
				return *a < *b;
			}

			auto operator()(std::shared_ptr<N> const& a, N const& b) const noexcept -> bool {
				return *a < b;
			}
			auto operator()(N const& a, std::shared_ptr<N> const& b) const noexcept -> bool {
				return a < *b;
			}
		};

		struct edge_cmp {
			using is_transparent = void;
			auto operator()(edge const& a, edge const& b) const noexcept -> bool {
				if (*a.to != *b.to) {
					return *a.to < *b.to;
				}
				return *a.weight < *b.weight;
			}

			// find for {src, dst,weight}
			auto operator()(edge const& a, value_type const& b) const noexcept -> bool {
				if (*a.to != b.to) {
					return *a.to < b.to;
				}
				return *a.weight < b.weight;
			}
			auto operator()(value_type const& a, edge const& b) const noexcept -> bool {
				if (a.to != *b.to) {
					return a.to < *b.to;
				}
				return a.weight < *b.weight;
			}

			// find for {dst}
			auto operator()(edge const& a, N const& b) const noexcept -> bool {
				return *a.to < b;
			}
			auto operator()(N const& a, edge const& b) const noexcept -> bool {
				return a < *b.to;
			}
		};

		struct weight_cmp {
			using is_transparent = void;
			auto operator()(std::shared_ptr<E> const& a, std::shared_ptr<E> const& b) const noexcept
			   -> bool {
				return *a < *b;
			}
			auto operator()(std::shared_ptr<E> const& a, E const& b) const noexcept -> bool {
				return *a < b;
			}
			auto operator()(E const& a, std::shared_ptr<E> const& b) const noexcept -> bool {
				return a < *b;
			}
		};
		std::set<std::shared_ptr<N>, node_cmp> nodes_;
		std::map<std::shared_ptr<N>, std::set<edge, edge_cmp>, edge_map_cmp> edges_;
		std::set<std::shared_ptr<E>, weight_cmp> weights_;

		auto swap(graph& other) -> void {
			std::swap(nodes_, other.nodes_);
			std::swap(edges_, other.edges_);
			std::swap(weights_, other.weights_);
		}

	public:
		class iterator {
			using map_iterator =
			   typename std::map<std::shared_ptr<N>, std::set<edge, edge_cmp>, edge_map_cmp>::const_iterator;

			using set_iterator = typename std::set<edge, edge_cmp>::const_iterator;

		public:
			// iterator traits
			using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			// Iterator constructor
			iterator() = default;

			// copy/move constructor/assignment
			iterator(const iterator&) = default;
			iterator(iterator&&) noexcept = default;
			auto operator=(const iterator&) -> iterator& = default;
			auto operator=(iterator&&) noexcept -> iterator& = default;

			// Iterator source
			auto operator*() const -> reference {
				return value_type{*map_iter_->first, *set_iter_->to, *set_iter_->weight};
			}

			// Iterator traversal
			auto operator++() -> iterator& {
				++set_iter_;
				while (map_iter_ != last_ and set_iter_ == map_iter_->second.end()) {
					++map_iter_;
					if (map_iter_ != last_) {
						set_iter_ = map_iter_->second.begin();
					}
				}
				return *this;
			}

			auto operator++(int) -> iterator {
				auto copy = *this;
				++*this;
				return copy;
			}

			auto operator--() -> iterator& {
				while (map_iter_ == last_ or set_iter_ == map_iter_->second.begin()) {
					--map_iter_;
					set_iter_ = map_iter_->second.end();
				}
				--set_iter_;
				return *this;
			}
			auto operator--(int) -> iterator {
				auto copy = *this;
				--*this;
				return copy;
			}

			// Iterator comparison
			friend auto operator==(const iterator& lhs, const iterator& rhs) -> bool {
				return lhs.map_iter_ == rhs.map_iter_
				       and (lhs.map_iter_ == lhs.last_ or lhs.set_iter_ == rhs.set_iter_);
			}

		private:
			map_iterator last_;
			map_iterator map_iter_;
			set_iterator set_iter_;
			explicit iterator(map_iterator last, map_iterator map_iter, set_iterator set_iter) noexcept
			: last_(last)
			, map_iter_(map_iter)
			, set_iter_(set_iter){};

			friend class graph<N, E>;
		};

		// Complexity: O(1)
		auto erase_edge(iterator i) noexcept -> iterator {
			auto copy = i++;
			auto to_erase_map_iter = copy.map_iter_;
			auto to_erase_set_iter = copy.set_iter_;
			auto key = (*to_erase_map_iter).first;
			edges_[key].erase(to_erase_set_iter);
			// If it is the last edge I erased for src
			// Delete this src key from edges map as well
			if (edges_[key].empty()) {
				edges_.erase(to_erase_map_iter);
			}
			return i;
		}

		// Complexity: O(d) - d is the number of edges between i and s
		auto erase_edge(iterator i, iterator s) noexcept -> iterator {
			auto to_erase = i;
			while (to_erase != s) {
				to_erase = erase_edge(to_erase);
			}
			return to_erase;
		}

		[[nodiscard]] auto begin() const -> iterator {
			// the inner iter is the iterator for set
			// define as empty if edges_ map is empty because there are no edges
			auto inner = edges_.empty() ? decltype((edges_.begin())->second.begin()){}
			                            : (edges_.begin())->second.begin();
			return iterator(edges_.end(), edges_.begin(), inner);
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator(edges_.end(), edges_.end(), {});
		}

		// Complexity: O(log(n)+log(e))
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const noexcept
		   -> iterator {
			value_type edge_to_find = {src, dst, weight};
			// Complexity: log(n)
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return end();
			}
			// Complexity: log(e)
			auto set_iter = map_iter->second.find(edge_to_find);
			if (set_iter == map_iter->second.end()) {
				return end();
			}
			return iterator(edges_.end(), map_iter, set_iter);
		}
	};
} // namespace gdwg
#endif // GDWG_GRAPH_HPP
