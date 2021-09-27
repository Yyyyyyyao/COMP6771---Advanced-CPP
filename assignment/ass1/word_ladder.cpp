#include "comp6771/word_ladder.hpp"

// Write your implementation here
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// function to extract words with same length as "from" from lexicon
auto get_lexicon_word_bags(auto const& length, std::unordered_set<std::string> const& lexicon)
   -> std::unordered_map<std::string, std::unordered_set<std::string>> {
	// word_bags is a dictionary that
	// key: wor*
	// value: <word, work, worm ...>
	// * can represent any char
	auto word_bags = std::unordered_map<std::string, std::unordered_set<std::string>>{};
	for (auto const& word : lexicon) {
		auto len = word.size();
		// we first filter by the size
		// if from is length of 3, we only care about words of length 3
		if (len == length) {
			// The following for loop does a job that
			//  e.g.
			// given fly, we need to put it into word_bags
			// it belongs to word_class of *ly, f*y, fl*
			for (auto i = 0u; i < len; ++i) {
				auto word_class = word;
				word_class.at(i) = '*'; // find its word_class, e.g. fl*
				auto class_iter = word_bags.find(word_class);
				// if the class is already in word_bags, we insert word
				if (class_iter != word_bags.end()) {
					class_iter->second.insert(word);
				}
				else { // else we create this key and value and insert the word
					auto new_bag_class = std::unordered_set<std::string>{};
					new_bag_class.insert(word);
					word_bags.emplace(word_class, new_bag_class);
				}
			}
		}
	}
	return word_bags;
}

// get neighbours of a word
auto get_neighbours(auto const& target, auto const& total_lookup) -> std::unordered_set<std::string> {
	auto neighbours = std::unordered_set<std::string>{};
	auto const len = target.size();
	// with the target word
	// we simply look up that word_class it belongs to
	// the union of its classes is its neighbours
	// e.g. fly has a set of neighbours which is a union of
	// *ly, f*l, fl*
	for (auto i = 0u; i < len; ++i) {
		auto target_class = target;
		target_class.at(i) = '*';
		auto class_iter = total_lookup.find(target_class);
		if (class_iter != total_lookup.end()) {
			auto neighbour_set = class_iter->second;
			for (auto const n : neighbour_set) {
				neighbours.insert(n);
			}
		}
		else {
			std::cout << "fatal error\n";
		}
	}
	return neighbours;
}

// function of BFS
auto BFS(auto const& src, auto const& dest, auto& predecessor, auto& distance, auto const& lookup)
   -> bool {
	auto find_path_flag = false; // flag to show whether it find a path
	auto queue = std::queue<std::string>{}; // queue to process the search
	distance[src] = 0; // distance to source itself is 0
	predecessor[src].push_back(""); // the parent of the source is empty
	queue.push(src); // push source to the queue
	while (!queue.empty()) {
		auto const u = queue.front();
		queue.pop();
		auto const u_neighbours = get_neighbours(u, lookup); // get neighbours
		for (auto const& neighbour : u_neighbours) {
			auto it = distance.find(neighbour);
			if (it != distance.end()) {
				// we update the distance of a node
				// if we find a shorter path than what it is stored in previous
				// we also need to clean and update the predecessor of this node since it has a shorter
				// path
				if (distance[neighbour] > distance[u] + 1) {
					distance[neighbour] = distance[u] + 1;
					predecessor[neighbour].clear();
					predecessor[neighbour].push_back(u);
					queue.push(neighbour);
				}
				else if (distance[neighbour] == distance[u] + 1) {
					// if distance are equal,
					// we know that this node has one more predecessors, u,
					// which can make this node having the shorted distance to the source
					predecessor[neighbour].push_back(u);
				}
			}
			else {
				// if this neighbour does not have a distance to source
				// we simply add one
				distance[neighbour] = distance[u] + 1;
				queue.push(neighbour);
				predecessor[neighbour].push_back(u);
			}
			if (neighbour == dest) { // we find the path
				find_path_flag = true;
			}
		}
	}
	return find_path_flag;
}

// function to find all shortest paths
auto collect_paths(auto& paths, auto& path, auto const parent, auto const u) -> void {
	// base case
	// the parent of the source node is ""
	if (u == "") {
		paths.push_back(path);
		return;
	}

	auto it = parent.find(u);
	if (it != parent.end()) {
		auto const [key, value] = *it;
		for (auto const par : value) {
			path.push_back(u);
			collect_paths(paths, path, parent, par);
			path.pop_back();
		}
	}
}

namespace word_ladder {
	auto generate(std::string const& from,
	              std::string const& to,
	              std::unordered_set<std::string> const& lexicon)
	   -> std::vector<std::vector<std::string>> {
		auto paths = std::vector<std::vector<std::string>>{};

		// edge case 1: from and to are in different size
		if (from.size() != to.size()) {
			return paths;
		}

		// edge case 2: from does not exist and to does not exist
		if (!lexicon.contains(from) || !lexicon.contains(to)) {
			return paths;
		}

		// edge case 3: from and to are the same word
		if (from == to) {
			return paths;
		}

		auto src_word_len = from.size();
		auto len_lookup = get_lexicon_word_bags(src_word_len, lexicon);

		// predecessor stores
		// key: a word
		// value: a set of words which are the parent of this word when doing BFS to find shortest
		// path
		auto predecessor = std::unordered_map<std::string, std::vector<std::string>>{};

		// distance stores
		// key: a word
		// value: an integer which indicates the number of hops between "from" and the word
		auto distance = std::unordered_map<std::string, int>{};

		// it is a flag to show if there is a shortest path or not
		auto find_path_flag = BFS(from, to, predecessor, distance, len_lookup);

		// each individual path in paths
		auto path = std::vector<std::string>{};

		if (find_path_flag) {
			// the result of collect_paths
			// are unordered and reversed
			// since we start from "to" to find its parent
			collect_paths(paths, path, predecessor, to);

			// reverse each path
			for (auto& p : paths) {
				std::reverse(p.begin(), p.end());
			}

			// sort them at the end
			std::sort(paths.begin(), paths.end());
		}

		return paths;
	}
} // namespace word_ladder