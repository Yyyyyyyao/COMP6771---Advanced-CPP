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
	auto calculate(std::string& s, val& a, val& b, std::vector<std::string>& output) -> val {
		if (s == "add") {
			if (std::holds_alternative<int>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 + b1;
				std::stringstream os;
				os << a1 << " + " << b1 << " = " << (a1 + b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<double>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 + b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " + " << b1 << " = " << (a1 + b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<int>(a) and std::holds_alternative<double>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 + b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " + " << b1 << " = " << (a1 + b1);
				output.push_back(os.str());
				return retval;
			}
			else {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 + b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " + " << b1 << " = " << (a1 + b1);
				output.push_back(os.str());
				return retval;
			}
		}
		if (s == "sub") {
			if (std::holds_alternative<int>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 - b1;
				std::stringstream os;
				os << a1 << " - " << b1 << " = " << (a1 - b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<double>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 - b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " - " << b1 << " = " << (a1 - b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<int>(a) and std::holds_alternative<double>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 - b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " - " << b1 << " = " << (a1 - b1);
				output.push_back(os.str());
				return retval;
			}
			else {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 - b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " - " << b1 << " = " << (a1 - b1);
				output.push_back(os.str());
				return retval;
			}
		}
		if (s == "div") {
			if (std::holds_alternative<int>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 / b1;
				std::stringstream os;
				os << a1 << " / " << b1 << " = " << (a1 / b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<double>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 / b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " / " << b1 << " = " << (a1 / b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<int>(a) and std::holds_alternative<double>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 / b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " / " << b1 << " = " << (a1 / b1);
				output.push_back(os.str());
				return retval;
			}
			else {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 / b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " / " << b1 << " = " << (a1 / b1);
				output.push_back(os.str());
				return retval;
			}
		}
		else {
			//*
			if (std::holds_alternative<int>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 * b1;
				std::stringstream os;
				os << a1 << " * " << b1 << " = " << (a1 * b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<double>(a) and std::holds_alternative<int>(b)) {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<int>(b);
				auto retval = val();
				retval = a1 * b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " * " << b1 << " = " << (a1 * b1);
				output.push_back(os.str());
				return retval;
			}
			else if (std::holds_alternative<int>(a) and std::holds_alternative<double>(b)) {
				auto a1 = std::get<int>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 * b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " * " << b1 << " = " << (a1 * b1);
				output.push_back(os.str());
				return retval;
			}
			else {
				auto a1 = std::get<double>(a);
				auto b1 = std::get<double>(b);
				auto retval = val();
				retval = a1 * b1;
				std::stringstream os;
				os << std::fixed << std::setprecision(3) << a1 << " * " << b1 << " = " << (a1 * b1);
				output.push_back(os.str());
				return retval;
			}
		}
	};
	auto process(std::vector<std::string> const& tokens,
	             std::size_t start,
	             std::size_t end,
	             std::vector<std::string>& output,
	             std::stack<val>& stack) -> void {
		auto repeats = std::stack<std::size_t>();
		auto repeat_time = std::stack<int>();
		for (auto j = start; j < end; j++) {
			auto s = tokens[j];
			std::vector<std::string> result;
			std::istringstream iss(s);
			for(std::string str; iss >> str; ) {
                result.push_back(str);
			}
			for (auto i : result) {
				if (std::regex_match(i, std::regex("^[0-9]+$"))) {
					auto item = val();
					item = std::stoi(i);
					stack.push(item);
					// os << "int " << s;
				}
				else if (std::regex_match(i, std::regex("^[0-9]+.[0-9]+$"))) {
					auto item = val();
					item = std::stod(i);
					stack.push(item);
					// os << "double " << s;
				}
				else if (i == "add" or i == "sub" or i == "mult" or i == "div") {
					auto a = stack.top();
					stack.pop();
					auto b = stack.top();
					stack.pop();
					auto result_calc = calculate(i, a, b, output);
					stack.push(result_calc);
				}
				else if (i == "sqrt") {
					auto a = stack.top();
					stack.pop();
					if (std::holds_alternative<int>(a)) {
						auto a1 = std::get<int>(a);
						auto re = sqrt(a1);
						std::stringstream os;
						os << "sqrt " << a1 << " = " << re;
						output.push_back(os.str());
						auto item = val();
						item = re;
						stack.push(item);
					}
					if (std::holds_alternative<double>(a)) {
						auto a1 = std::get<double>(a);
						auto re = sqrt(a1);
						std::stringstream os;
						os << std::fixed << std::setprecision(3) << "sqrt " << a1 << " = " << re;
						output.push_back(os.str());
						auto item = val();
						item = re;
						stack.push(item);
					}
				}
				else if (i == "pop") {
					stack.pop();
				}
				else if (i == "reverse") {
					auto reverse = std::get<int>(stack.top());
					stack.pop();
					auto q = std::queue<val>();
					for (auto k = 0; k < reverse; k++) {
						q.push(stack.top());
						stack.pop();
					}
					while (q.size() != 0) {
						stack.push(q.front());
						q.pop();
					}
				}
				else if (i == "repeat") {
					repeat_time.push(std::get<int>(stack.top()) - 1);
					stack.pop();
					repeats.push(j + 1);
				}
				if (i == "endrepeat") {
					auto t = repeat_time.top();
					for (auto m = 0; m < t; m++) {
						process(tokens, repeats.top(), j, output, stack);
					}
					repeat_time.pop();
					repeats.pop();
				}
			}
		}
	}

	auto run_calculator(std::vector<std::string> const& input) -> std::vector<std::string> const {
		auto stack = std::stack<val>();
		auto output = std::vector<std::string>{};
		process(input, 0, input.size(), output, stack);
		return output;
	}
} // namespace q1
