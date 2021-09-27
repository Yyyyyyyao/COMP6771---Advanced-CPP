#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <variant>
#include <regex>
#include <cmath>
#include <queue>

#include "q1/q1.hpp"

namespace q1 {
	using val = std::variant<int, double>;
	auto calculate(std::string const& cmd, val const& l, val const& r, std::vector<std::string>& output) -> val{
		if (cmd == "add"){
			if(std::holds_alternative<int>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a + b;
				std::stringstream os;
				os << a << " + " << b << " = " << (a+b);
				output.push_back(os.str());
				return res;
			}else if (std::holds_alternative<double>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<double>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a + b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " + " << b << " = " << (a+b);
				output.push_back(os.str());
				return res;
			} else if (std::holds_alternative<int>(l) and std::holds_alternative<double>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a + b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " + " << b << " = " << (a+b);
				output.push_back(os.str());
				return res;
			} else {
				auto a = std::get<double>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a + b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " + " << b << " = " << (a+b);
				output.push_back(os.str());
				return res;
			}
		}
		if (cmd == "sub"){
			if(std::holds_alternative<int>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a - b;
				std::stringstream os;
				os << a << " - " << b << " = " << (a-b);
				output.push_back(os.str());
				return res;
			}else if (std::holds_alternative<double>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<double>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a - b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " - " << b << " = " << (a-b);
				output.push_back(os.str());
				return res;
			} else if (std::holds_alternative<int>(l) and std::holds_alternative<double>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a - b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " - " << b << " = " << (a-b);
				output.push_back(os.str());
				return res;
			} else {
				auto a = std::get<double>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a - b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " - " << b << " = " << (a-b);
				output.push_back(os.str());
				return res;
			}
		}
		if (cmd == "mult"){
			if(std::holds_alternative<int>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a * b;
				std::stringstream os;
				os << a << " * " << b << " = " << (a*b);
				output.push_back(os.str());
				return res;
			}else if (std::holds_alternative<double>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<double>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a * b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " * " << b << " = " << (a*b);
				output.push_back(os.str());
				return res;
			} else if (std::holds_alternative<int>(l) and std::holds_alternative<double>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a * b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " * " << b << " = " << (a*b);
				output.push_back(os.str());
				return res;
			} else {
				auto a = std::get<double>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a * b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " * " << b << " = " << (a*b);
				output.push_back(os.str());
				return res;
			}
		}else{
			if(std::holds_alternative<int>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a / b;
				std::stringstream os;
				os << a << " / " << b << " = " << (a/b);
				output.push_back(os.str());
				return res;
			}else if (std::holds_alternative<double>(l) and std::holds_alternative<int>(r)){
				auto a = std::get<double>(l);
				auto b = std::get<int>(r);
				auto res = val();
				res = a / b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " / " << b << " = " << (a/b);
				output.push_back(os.str());
				return res;
			} else if (std::holds_alternative<int>(l) and std::holds_alternative<double>(r)){
				auto a = std::get<int>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a / b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " / " << b << " = " << (a/b);
				output.push_back(os.str());
				return res;
			} else {
				auto a = std::get<double>(l);
				auto b = std::get<double>(r);
				auto res = val();
				res = a / b;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a << " / " << b << " = " << (a/b);
				output.push_back(os.str());
				return res;
			}
		}
	}

	auto process(std::vector<std::string> const& input, std::size_t start, std::size_t end, std::vector<std::string>& output, std::stack<val>& stack) -> void{
		auto repeats = std::stack<std::size_t>();
		auto repeat_time = std::stack<int>();
		(void) output;
		for (auto j = start; j < end; ++j){
			auto cmd = input[j];
			std::vector<std::string> tokens;
			std::istringstream iss(cmd);
			for(std::string s; iss >> s;){
				tokens.push_back(s);
			}
			for(auto& i: tokens){
				if(std::regex_match(i, std::regex("^[0-9]+$"))){
					auto int_num = val();
					int_num = std::stoi(i);
					stack.push(int_num);
				}else if(std::regex_match(i, std::regex("^[0-9]+.[0-9]+$"))){
					auto double_num = val();
					double_num = std::stod(i);
					stack.push(double_num);
				}else if (i == "add" or i == "sub" or i == "mult" or i == "div"){
					auto l = stack.top();
					stack.pop();
					auto r = stack.top();
					stack.pop();
					auto result_cal = calculate(i, l, r, output);
					stack.push(result_cal);
				}else if (i == "sqrt"){
					auto a = stack.top();
					stack.pop();
					if (std::holds_alternative<int>(a)){
						auto v = std::get<int>(a);
						auto res = sqrt(v);
						std::stringstream os;
						os << "sqrt " << v << " = " << res;
						output.push_back(os.str());
						auto int_num = val();
						int_num = res;
						stack.push(int_num);
					}
					if (std::holds_alternative<double>(a)){
						auto v = std::get<double>(a);
						auto res = sqrt(v);
						std::stringstream os;
						os << std::fixed << std::setprecision(3) << "sqrt " << v << " = " << res;
						output.push_back(os.str());
						auto double_num = val();
						double_num = res;
						stack.push(double_num);
					}
				}else if (i == "pop"){
					stack.pop();
				}else if (i == "reverse"){
					auto reverse = std::get<int>(stack.top());
					stack.pop();
					auto q = std::queue<val>();
					for (auto k = 0; k < reverse; ++k){
						q.push(stack.top());
						stack.pop();
					}
					while(!q.empty()){
						stack.push(q.front());
						q.pop();
					}
				}else if (i == "repeat"){
					repeat_time.push(std::get<int>(stack.top())-1);
					stack.pop();
					repeats.push(j+1);
				}
				if (i == "endrepeat"){
					auto t = repeat_time.top();
					for(auto m = 0; m < t; ++m){
						process(input, repeats.top(), j, output, stack);
					}
					repeat_time.pop();
					repeats.pop();
				}
			}

		}

	}

	auto run_calculator(std::vector<std::string> const& input) -> std::vector<std::string> const {
		// (void)input; // Remove this line
		auto output = std::vector<std::string>{};
		auto stack = std::stack<val>();
		process(input, 0, input.size(), output, stack);
		return output;
	}
} // namespace q1
