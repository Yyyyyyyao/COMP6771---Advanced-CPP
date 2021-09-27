#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "record.hpp"
#include "query.hpp"
#include <algorithm>
#include <ostream>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
namespace q2 {
	class database {
	public:
		database() = default;
		~database() = default;

		auto insert(record const& r) -> void {
			records_.push_back(r);
		}

		auto count() const -> std::size_t {
			return records_.size();
		}

		friend auto operator<<(std::ostream& os, database const& g) -> std::ostream& {
			for(auto it =g.records_.begin(); it != g.records_.end(); ++it){
				os << (*it);
			}
			return os;
		}
		friend auto operator>>(std::istream& is, database& sm) -> std::istream& {
			auto r = record();
			while(is >> r){
				sm.insert(r);
				r = record();
			}
			return is;
		}
		auto delete_matching(query const& q) -> std::size_t {
			return std::erase_if(records_, [&q](auto const& item){
				return q.matches(item);
			});
		}
		auto select(query const& q) const -> database {
			database n;
			std::for_each(records_.begin(), records_.end(), [&q, &n](auto const& item){
				if(q.matches(item)){
					n.insert(item);
				}
			});
			return n;
		}
	private:
		std::vector<record> records_;
	};

} // namespace q2

#endif // DATABASE_HPP
