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

		struct edge {
			std::shared_ptr<N> from;
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
				if (*a.from != *b.from) {
					return *a.from < *b.from;
				}
				if (*a.to != *b.to) {
					return *a.to < *b.to;
				}
				return *a.weight < *b.weight;
			}

			// find for {src,dst,weight}
			auto operator()(edge const& a, value_type const& b) const noexcept -> bool {
				if (*a.from != b.from) {
					return *a.from < b.from;
				}
				if (*a.to != b.to) {
					return *a.to < b.to;
				}
				return *a.weight < b.weight;
			}
			auto operator()(value_type const& a, edge const& b) const noexcept -> bool {
				if (a.from != *b.from) {
					return a.from < *b.from;
				}
				if (a.to != *b.to) {
					return a.to < *b.to;
				}
				return a.weight < *b.weight;
			}

			// find for {src, dst}
			auto operator()(edge const& a, std::pair<N, N> const& b) const noexcept -> bool {
				if (*a.from != b.first) {
					return *a.from < b.first;
				}
				return *a.to < b.second;
			}
			auto operator()(std::pair<N, N> const& a, edge const& b) const noexcept -> bool {
				if (a.first != *b.from) {
					return a.first < *b.from;
				}
				return a.second < *b.to;
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

			explicit iterator(
			   map_iterator map_iter_end,
			   std::map<std::shared_ptr<N>, std::set<edge, edge_cmp>, edge_map_cmp> const& pointee,
			   map_iterator map_iter,
			   set_iterator set_iter) noexcept
			: map_iter_end_(map_iter_end)
			, pointee_(&pointee)
			, map_iter_(map_iter)
			, set_iter_(set_iter){};

			// Iterator source
			auto operator*() const -> reference {
				return value_type{*set_iter_->from, *set_iter_->to, *set_iter_->weight};
			}

			// Iterator traversal
			auto operator++() -> iterator& {
				if (set_iter_ != map_iter_->second.end()) {
					++set_iter_;

					if (set_iter_ != map_iter_->second.end()) {
						return *this;
					}
				}

				++map_iter_;
				set_iter_ = map_iter_ == pointee_->end() ? set_iterator() : map_iter_->second.begin();
				return *this;
			}
			auto operator++(int) -> iterator {
				auto copy = *this;
				++*this;
				return copy;
			}
			auto operator--() -> iterator& {
				if (map_iter_ == pointee_->end()) {
					--map_iter_;
					set_iter_ = map_iter_->second.end();
					--set_iter_;
					return *this;
				}
				if (set_iter_ == map_iter_->second.begin()) {
					if (map_iter_ == pointee_->begin()) {
						set_iter_ = set_iterator();
					}
					else {
						--map_iter_;
						set_iter_ = map_iter_->second.end();
					}
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
			auto operator==(iterator const& other) const -> bool {
				if (other.map_iter_ == pointee_->end() && map_iter_ == pointee_->end()) {
					return true;
				}
				if (other.map_iter_ != map_iter_) {
					return false;
				}
				return other.set_iter_ == set_iter_;
			};

		private:
			map_iterator map_iter_end_;
			std::map<std::shared_ptr<N>, std::set<edge, edge_cmp>, edge_map_cmp> const* pointee_ =
			   nullptr;
			map_iterator map_iter_;
			set_iterator set_iter_;

			friend class graph<N, E>;
		};

		graph() = default;

		graph(std::initializer_list<N> il) {
			*this = graph(il.begin(), il.end());
		}

		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			std::set<edge, edge_cmp> empty_set;
			for (auto& it = first; it != last; ++it) {
				auto nodes_ptr = std::make_shared<N>(*it);
				nodes_.emplace(nodes_ptr);
			}
		}

		// std::exchange returns old value
		graph(graph&& other) noexcept {
			nodes_ = std::exchange(other.nodes_, std::set<std::shared_ptr<N>, node_cmp>{});
			edges_ =
			   std::exchange(other.edges_,
			                 std::map<std::shared_ptr<N>, std::set<edge, edge_cmp>, edge_map_cmp>{});

			weights_ = std::exchange(other.weights_, std::set<std::shared_ptr<E>, weight_cmp>{});
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
				std::for_each(v.second.begin(), v.second.end(), [this](auto const& e) {
					insert_edge(*(e.from), *(e.to), *(e.weight));
				});
			});
		}

		auto operator=(graph const& other) noexcept -> graph& {
			graph copy(other);
			copy.swap(*this);
			return *this;
		}

		auto insert_node(N const& value) -> bool {
			if (!is_node(value)) {
				auto new_node = std::make_shared<N>(value);
				nodes_.emplace(new_node);
				return true;
			}
			return false;
		}

		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
				                         "or dst node does not exist");
			}

			auto another_src = nodes_.find(src);
			auto another_dst = nodes_.find(dst);
			auto new_edge = edge{};
			new_edge.from = *another_src;
			new_edge.to = *another_dst;
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
			if (map_iter == edges_.end()) {
				auto node_iter = nodes_.find(src);
				std::set<edge, edge_cmp> empty_set = {new_edge};
				edges_.emplace(*node_iter, empty_set);
			}
			else {
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
			insert_node(new_data);
			merge_replace_node(old_data, new_data);
			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or "
				                         "new data if they don't exist in the graph");
			}
			auto found_iter = edges_.find(old_data);
			if (found_iter != edges_.end()) {
				for (auto map_iter = edges_.begin(); map_iter != edges_.end(); ++map_iter) {
					if (map_iter->second.empty()) {
						continue;
					}
					for (auto old_edge = map_iter->second.begin(); old_edge != map_iter->second.end();) {
						if (*old_edge->from == old_data) {
							insert_edge(new_data, *old_edge->to, *old_edge->weight);
							old_edge = map_iter->second.erase(old_edge);
						}
						else if (*old_edge->to == old_data) {
							insert_edge(*old_edge->from, new_data, *old_edge->weight);
							old_edge = map_iter->second.erase(old_edge);
						}
						else if (*old_edge->from == old_data && *old_edge->to == old_data) {
							insert_edge(new_data, new_data, *old_edge->weight);
							old_edge = map_iter->second.erase(old_edge);
						}
						else {
							++old_edge;
						}
					}
				}
				edges_.erase(found_iter);
			}
			auto old_data_it = nodes_.find(old_data);
			nodes_.erase(old_data_it);
		}

		auto erase_node(N const& value) -> bool {
			auto old_data_it = nodes_.find(value);
			if (old_data_it == nodes_.end()) {
				return false;
			}
			nodes_.erase(old_data_it);
			for (auto map_iter = edges_.begin(); map_iter != edges_.end();) {
				for (auto old_edge = map_iter->second.begin(); old_edge != map_iter->second.end();) {
					if (*old_edge->from == value || *old_edge->to == value) {
						old_edge = map_iter->second.erase(old_edge);
					}
					else {
						++old_edge;
					}
				}
				if (map_iter->second.empty()) {
					map_iter = edges_.erase(map_iter);
				}
				else {
					++map_iter;
				}
			}

			return true;
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
				                         "if "
				                         "they don't exist in the graph");
			}
			value_type edge_to_find = {src, dst, weight};
			auto map_iter = edges_.find(src);
			auto iter = map_iter->second.find(edge_to_find);
			if (iter == map_iter->second.end()) {
				return false;
			}
			map_iter->second.erase(iter);
			if (map_iter->second.empty()) {
				edges_.erase(map_iter);
			}
			return true;
		}

		auto erase_edge(iterator i) -> iterator {
			auto copy = i++;
			auto to_erase_map_iter = copy.map_iter_;
			auto to_erase_set_iter = copy.set_iter_;
			auto key = (*to_erase_set_iter).from;
			edges_[(*to_erase_set_iter).from].erase(to_erase_set_iter);
			if (edges_[key].empty()) {
				edges_.erase(to_erase_map_iter);
			}
			return i;
		}

		auto erase_edge(iterator i, iterator s) -> iterator {
			auto to_erase = i;
			while (to_erase != s) {
				to_erase = erase_edge(to_erase);
			}
			return to_erase;
		}

		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
			weights_.clear();
		}

		[[nodiscard]] auto is_node(N const& value) const -> bool {
			auto iter = nodes_.find(value);
			return (iter != nodes_.end());
		}

		[[nodiscard]] auto empty() const -> bool {
			return nodes_.empty();
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}
			auto src_dst_pair = std::make_pair(src, dst);
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return false;
			}
			if (map_iter->second.empty()) {
				return false;
			}
			auto find_edge_iter = map_iter->second.find(src_dst_pair);
			return find_edge_iter != map_iter->second.end();
		}

		[[nodiscard]] auto nodes() const noexcept -> std::vector<N> {
			auto vec_nodes = std::vector<N>();
			std::for_each(nodes_.begin(), nodes_.end(), [&vec_nodes](auto const& it) {
				vec_nodes.push_back(*it);
			});
			return vec_nodes;
		}

		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E> {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}
			auto vec_weights = std::vector<E>();
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return vec_weights;
			}
			std::for_each(map_iter->second.begin(),
			              map_iter->second.end(),
			              [&src, &dst, &vec_weights](auto const& it) {
				              if (*it.from == src && *it.to == dst) {
					              vec_weights.push_back(*it.weight);
				              }
				              return;
			              });

			return vec_weights;
		}

		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const -> iterator {
			value_type edge_to_find = {src, dst, weight};
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return end();
			}
			auto set_iter = map_iter->second.find(edge_to_find);
			if (set_iter == map_iter->second.end()) {
				return end();
			}
			return iterator(edges_.end(), edges_, map_iter, set_iter);
		}

		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in "
				                         "the graph");
			}
			/// Use find and is_connected
			auto vec_dsts = std::vector<N>();
			auto map_iter = edges_.find(src);
			if (map_iter == edges_.end()) {
				return vec_dsts;
			}
			if (map_iter->second.empty()) {
				return vec_dsts;
			}
			std::for_each(map_iter->second.begin(), map_iter->second.end(), [&vec_dsts](auto const& it) {
				auto f = std::find(vec_dsts.begin(), vec_dsts.end(), *it.to);
				if (f == vec_dsts.end()) {
					vec_dsts.push_back(*it.to);
				}
			});
			return vec_dsts;
		}

		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			auto flag = std::equal(nodes_.begin(),
			                       nodes_.end(),
			                       other.nodes_.begin(),
			                       other.nodes_.end(),
			                       [](auto const& lhs, auto const& rhs) { return *lhs == *rhs; });
			if (flag) {
				flag = std::equal(edges_.begin(),
				                  edges_.end(),
				                  other.edges_.begin(),
				                  other.edges_.end(),
				                  [](auto const& lhs, auto const& rhs) {
					                  return std::equal(lhs.second.begin(),
					                                    lhs.second.end(),
					                                    rhs.second.begin(),
					                                    rhs.second.end(),
					                                    [](auto const& lhs_e, auto const& rhs_e) {
						                                    return *lhs_e.from == *rhs_e.from
						                                           && *lhs_e.to == *rhs_e.to
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

		auto print_nodes() -> void {
			std::cout << "nodes:" << std::endl;
			for (auto const& it : nodes_) {
				std::cout << *it << " : " << it.use_count() << std::endl;
			}
			// return nodes_;
		}

		auto print_edges() -> void {
			std::cout << "edges:" << std::endl;
			for (auto const& it : edges_) {
				std::cout << *it.first << ": \n";
				auto set = it.second;
				for (auto const& e : set) {
					std::cout << *(e.from) << " -> " << *e.to << ":" << *e.weight << std::endl;
				}
			}
		}

		auto print_weights() -> void {
			std::cout << "weights:" << std::endl;
			for (auto const& it : weights_) {
				std::cout << *it << " : " << it.use_count() << std::endl;
			}
		}

		[[nodiscard]] auto begin() const -> iterator {
			auto inner = edges_.empty() ? decltype((edges_.begin())->second.begin()){}
			                            : (edges_.begin())->second.begin();
			return iterator(edges_.end(), edges_, edges_.begin(), inner);
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator(edges_.end(), edges_, edges_.end(), {});
		}

	private:
		std::set<std::shared_ptr<N>, node_cmp> nodes_;
		std::map<std::shared_ptr<N>, std::set<edge, edge_cmp>, edge_map_cmp> edges_;
		// std::set<edge, edge_cmp> edges_;
		std::set<std::shared_ptr<E>, weight_cmp> weights_;

		auto swap(graph& other) -> void {
			std::swap(nodes_, other.nodes_);
			std::swap(edges_, other.edges_);
			std::swap(weights_, other.weights_);
		}
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP
