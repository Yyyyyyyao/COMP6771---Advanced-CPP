#ifndef QUERY_HPP
#define QUERY_HPP

#include <algorithm>
#include <ostream>
#include <map>
#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include "record.hpp"

namespace q2 {
	class record;

	class query {
	public:
		virtual ~query() = default;
		virtual auto clone() const -> query* {
			return new query(*this);
		}
		auto virtual matches(record const&) const -> bool{
			return true;
		};
	};

	class query_equals : public query {
	public:
		query_equals(std::string attr, std::string value):
		attr_(attr),
		value_(value){};

		auto matches(const record& r) const -> bool override {
			if (r.has_attribute(attr_)){
				return r.get_value(attr_) == value_;
			}
			return false;
		}
		auto clone() const -> query_equals* override {
			return new query_equals(attr_, value_);
		}
	private:
		std::string attr_;
		std::string value_;
	};

	// class query_less_than
	class query_less_than : public query {
	public:
		query_less_than(std::string attr, std::string value):
		attr_(attr),
		value_(value){};

		auto matches(const record& r) const -> bool override {
			if (r.has_attribute(attr_)){
				return r.get_value(attr_) < value_;
			}
			return false;
		}
		auto clone() const -> query_less_than* override {
			return new query_less_than(attr_, value_);
		}
	private:
		std::string attr_;
		std::string value_;
	};


	// class query_greater_than
	class query_greater_than : public query {
	public:
		query_greater_than(std::string attr, std::string value):
		attr_(attr),
		value_(value){};

		auto matches(const record& r) const -> bool override {
			if (r.has_attribute(attr_)){
				return r.get_value(attr_) > value_;
			}
			return false;
		}
		auto clone() const -> query_greater_than* override {
			return new query_greater_than(attr_, value_);
		}
	private:
		std::string attr_;
		std::string value_;
	};

	// class query_starts_with
	class query_starts_with : public query {
	public:
		query_starts_with(std::string attr, std::string value):
		attr_(attr),
		value_(value){};

		auto matches(const record& r) const -> bool override {
			if (r.has_attribute(attr_)){
				auto s = r.get_value(attr_);
				auto reg = std::regex("^"+value_);
				if(std::regex_search(s, reg)){
					return true;
				}
			}
			return false;
		}
		auto clone() const -> query_starts_with* override {
			return new query_starts_with(attr_, value_);
		}
	private:
		std::string attr_;
		std::string value_;
	};

	// class query_and
	class query_and : public query{
	public:
		query_and(query const& q1, query const& q2):
		q1_(q1.clone()), q2_(q2.clone()){};
		auto matches(const record& r) const -> bool override{
			return q1_->matches(r) and q2_->matches(r);
		}

		auto clone() const -> query_and* override{
			return new query_and(*q1_, *q2_);
		}

		~query_and(){
			delete q1_;
			delete q2_;
		}

	private:
		query* q1_;
		query* q2_;
	};

	// class query_or
	class query_or : public query{
	public:
		query_or(query const& q1, query const& q2):
		q1_(q1.clone()), q2_(q2.clone()){};
		auto matches(const record& r) const -> bool override{
			return q1_->matches(r) or q2_->matches(r);
		}

		auto clone() const -> query_or* override{
			return new query_or(*q1_, *q2_);
		}

		~query_or(){
			delete q1_;
			delete q2_;
		}

	private:
		query* q1_;
		query* q2_;
	};

	// class query_not
	class query_not : public query{
	public:
		query_not(query const& q1):
		q1_(q1.clone()){};
		auto matches(const record& r) const -> bool override{
			return !q1_->matches(r);
		}

		auto clone() const -> query_not* override{
			return new query_not(*q1_);
		}

		~query_not(){
			delete q1_;
		}

	private:
		query* q1_;
	};
} // namespace q2

#endif // QUERY_HPP
