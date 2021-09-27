#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
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
			using edges_iterator = typename std::set<edge, edge_cmp>::iterator;

		public:
			// iterator traits
			using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			// Iterator constructor
			iterator() = default;

			// Iterator source
			auto operator*() const -> reference {
				return value_type{*iter_->from, *iter_->to, *iter_->weight};
			}

			// Iterator traversal
			auto operator++() -> iterator& {
				++iter_;
				return *this;
			}
			auto operator++(int) -> iterator {
				auto copy = *this;
				++*this;
				return copy;
			}
			auto operator--() -> iterator& {
				--iter_;
				return *this;
			}
			auto operator--(int) -> iterator {
				auto copy = *this;
				--*this;
				return copy;
			}

			// Iterator comparison
			auto operator==(iterator const& other) const -> bool = default;

		private:
			edges_iterator iter_;
			explicit iterator(edges_iterator iter) {
				iter_ = iter;
			};
			friend class graph<N, E>;
		};

		// Your member functions go here
		// 2.2 Constructors
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
			nodes_ = std::exchange(other.nodes_, std::set<std::shared_ptr<N>, node_cmp>{});
			edges_ = std::exchange(other.edges_, std::set<edge, edge_cmp>{});
			weights_ = std::exchange(other.weights_, std::set<std::shared_ptr<E>, weight_cmp>{});
		}

		auto operator=(graph&& other) noexcept -> graph& {
			auto copy = graph(std::move(other));
			copy.swap(*this);
			return *this;
		}

		graph(graph const& other) noexcept {
			nodes_.clear();
			std::for_each(other.nodes_.begin(), other.nodes_.end(), [this](auto const& v) {
				nodes_.emplace(std::make_shared<N>(*v));
			});

			weights_.clear();
			std::for_each(other.weights_.begin(), other.weights_.end(), [this](auto const& v) {
				weights_.emplace(std::make_shared<E>(*v));
			});

			edges_.clear();
			std::for_each(other.edges_.begin(), other.edges_.end(), [this](auto const& v) {
				insert_edge(*(v.from), *(v.to), *(v.weight));
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

			value_type edge_to_find = {src, dst, weight};
			auto iter = edges_.find(edge_to_find);

			if (iter == edges_.end()) {
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

				edges_.emplace(new_edge);
				return true;
			}
			return false;
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
			for (auto old_edge = edges_.begin(); old_edge != edges_.end();) {
				if (*old_edge->from == old_data) {
					insert_edge(new_data, *old_edge->to, *old_edge->weight);
					old_edge = edges_.erase(old_edge);
				}
				else if (*old_edge->to == old_data) {
					insert_edge(*old_edge->from, new_data, *old_edge->weight);
					old_edge = edges_.erase(old_edge);
				}
				else if (*old_edge->from == old_data && *old_edge->to == old_data) {
					insert_edge(new_data, new_data, *old_edge->weight);
					old_edge = edges_.erase(old_edge);
				}
				else {
					++old_edge;
				}
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
			for (auto old_edge = edges_.begin(); old_edge != edges_.end();) {
				if (*old_edge->from == value || *old_edge->to == value) {
					old_edge = edges_.erase(old_edge);
				}
				else {
					++old_edge;
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
			auto iter = edges_.find(edge_to_find);
			if (iter == edges_.end()) {
				return false;
			}
			// auto erase_weight = *iter->weight;
			edges_.erase(iter);
			// auto w_iter = std::find_if(weights_.begin(),
			//                            weights_.end(),
			//                            [&erase_weight](auto const& it) { return *it == erase_weight;
			//                            });

			// if ((*w_iter).use_count() == 1) {
			// 	weights_.erase(w_iter);
			// }
			return true;
		}

		auto erase_edge(iterator i) -> iterator {
			auto to_erase = (i++).iter_;
			edges_.erase(to_erase);
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

		// own testing functions ------- start block
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
				std::cout << *it.from << " -> " << *it.to << ":" << *it.weight << std::endl;
			}
		}

		auto print_weights() -> void {
			std::cout << "weights:" << std::endl;
			for (auto const& it : weights_) {
				std::cout << *it << " : " << it.use_count() << std::endl;
			}
		}
		// own testing functions ------- end block

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
			auto find_edge_iter = edges_.find(src_dst_pair);
			return find_edge_iter != edges_.end();
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
			std::for_each(edges_.begin(), edges_.end(), [&src, &dst, &vec_weights](auto const& it) {
				if (*it.from == src && *it.to == dst) {
					vec_weights.push_back(*it.weight);
				}
				return;
			});
			return vec_weights;
		}

		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const -> iterator {
			value_type edge_to_find = {src, dst, weight};
			auto find_edge_iter = edges_.find(edge_to_find);
			return iterator(find_edge_iter);
		}

		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in "
				                         "the graph");
			}
			/// Use find and is_connected
			auto vec_dsts = std::vector<N>();
			std::for_each(edges_.begin(), edges_.end(), [&src, &vec_dsts](auto const& it) {
				auto f = std::find(vec_dsts.begin(), vec_dsts.end(), *it.to);
				if (*it.from == src && f == vec_dsts.end()) {
					vec_dsts.push_back(*it.to);
				}
				return;
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
					                  return *lhs.from == *rhs.from && *lhs.to == *rhs.to
					                         && *lhs.weight == *rhs.weight;
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

		[[nodiscard]] auto begin() const -> iterator {
			return iterator(edges_.begin());
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator(edges_.end());
		}

	private:
		std::set<std::shared_ptr<N>, node_cmp> nodes_;
		std::set<edge, edge_cmp> edges_;
		std::set<std::shared_ptr<E>, weight_cmp> weights_;

		auto swap(graph& other) -> void {
			std::swap(nodes_, other.nodes_);
			std::swap(edges_, other.edges_);
			std::swap(weights_, other.weights_);
		}
	};
} // namespace gdwg

#endif // GDWG_GRAPH_HPP
