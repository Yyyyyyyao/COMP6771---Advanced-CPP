#ifndef RECORD_HPP
#define RECORD_HPP

#include <algorithm>
#include <ostream>
#include <map>
#include <iostream>
#include <string>
#include <sstream>

namespace q2 {
	class record {
	public:
		record() {};
		~record() {};
		auto get_value(std::string const& s) const -> std::string {
			auto iter = values_.find(s);
			if(iter != values_.end()){
				return iter->second;
			}
			return "";
		}

		auto has_attribute(std::string const& key) const -> bool {
			auto iter = values_.find(key);
			if(iter != values_.end()){
				return true;
			}
			return false;
		}
		auto count() const -> std::size_t {
			return values_.size();
		}

		auto delete_attribute(std::string const& key) -> bool {
			if(has_attribute(key)){
				values_.erase(key);
				return true;
			}
			return false;
		}
		auto set_value(std::string const& key, std::string const& val) -> void {
			values_[key] = val;
		}
		friend auto operator<<(std::ostream& os, record const& g) -> std::ostream& {
			os << "{\n";
			std::for_each(g.values_.begin(), g.values_.end(), [&os](auto const& iter){
				os << string_replace(string_replace(iter.first, "!", "!!"), "=", "!=") << "=" << string_replace(string_replace(iter.second, "!", "!!"), "=", "!=") << std::endl;
			});
			os << "}" << std::endl;
			return os;
		}
		friend auto operator>>(std::istream& is, record& sm) -> std::istream& {
			(void) sm;
			for(std::string s; std::getline(is, s);){
				if (s == "{"){
					continue;
				}
				if (s == "}"){
					break;
				}
				s = string_replace(s, "!!", "!");
				auto p = s.find("=");
				auto pp = s.find("!=");
				while(pp == p-1){
					p = s.find("=", p+1);
					pp = s.find("!=", p+1);
				}

				sm.set_value(string_replace(s.substr(0, p), "!=", "="), string_replace(s.substr(p+1, s.size()), "!=", "="));
			}

			return is;
		}
	private:
		std::map<std::string, std::string> values_;
		static auto string_replace(std::string const& s, std::string const& target, std::string const& repl) -> std::string{
			auto ret = s;
			auto pos = ret.find(target);
			while(pos != std::string::npos){
				ret.replace(pos, target.size(), repl);
				pos = ret.find(target, pos+repl.size());
			}
			return ret;
		}
	};
} // namespace q2

#endif // RECORD_HPP
