#ifndef COMP6771_EUCLIDEAN_VECTOR_HPP
#define COMP6771_EUCLIDEAN_VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace comp6771 {
	class euclidean_vector_error : public std::runtime_error {
	public:
		explicit euclidean_vector_error(std::string const& what)
		: std::runtime_error(what) {}
	};

	class euclidean_vector {
	public:
		// Default Constructor
		euclidean_vector() noexcept {
			dimension_ = 1;
			reset_norm();
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitude_ = std::make_unique<double[]>(1);
			magnitude_[0] = 0.0;
		}

		// Single-argument Constructor
		explicit euclidean_vector(int dimension) noexcept {
			dimension_ = static_cast<size_t>(dimension);
			reset_norm();
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitude_ = std::make_unique<double[]>(static_cast<size_t>(dimension));
		}

		// Constructor
		euclidean_vector(int dimension, double default_mag) noexcept {
			dimension_ = static_cast<size_t>(dimension);
			reset_norm();
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitude_ = std::make_unique<double[]>(static_cast<size_t>(dimension));
			std::fill_n(magnitude_.get(), dimension, default_mag);
		}

		// iter Constructor
		euclidean_vector(std::vector<double>::const_iterator begin,
		                 std::vector<double>::const_iterator end) noexcept {
			auto length = std::distance(begin, end);
			reset_norm();
			if (begin == end) {
				dimension_ = 1;
				// NOLINTNEXTLINE(modernize-avoid-c-arrays)
				magnitude_ = std::make_unique<double[]>(1);
				magnitude_[0] = 0.0;
			}
			else {
				dimension_ = static_cast<size_t>(length);
				// NOLINTNEXTLINE(modernize-avoid-c-arrays)
				magnitude_ = std::make_unique<double[]>(dimension_);
				std::copy(begin, end, magnitude_.get());
			}
		}

		// initializer_list Constructor
		// if empty initializer list empty,
		// default constructor will be called
		euclidean_vector(std::initializer_list<double> d_list) noexcept {
			dimension_ = d_list.size();
			reset_norm();
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitude_ = std::make_unique<double[]>(d_list.size());
			std::copy(d_list.begin(), d_list.end(), magnitude_.get());
		}

		// copy constructor
		euclidean_vector(euclidean_vector const& orig) noexcept {
			this->dimension_ = orig.dimension_;
			norm_ = orig.norm_;
			// NOLINTNEXTLINE(modernize-avoid-c-arrays)
			magnitude_ = std::make_unique<double[]>(this->dimension_);
			std::copy(orig.magnitude_.get(), orig.magnitude_.get() + orig.dimension_, magnitude_.get());
		};

		// move constructor
		euclidean_vector(euclidean_vector&& orig) noexcept {
			this->dimension_ = 0;
			reset_norm();
			std::swap(this->dimension_, orig.dimension_);
			std::swap(this->norm_, orig.norm_);
			this->magnitude_ = std::move(orig.magnitude_);
		};

		// copy assignment
		auto operator=(euclidean_vector const&) noexcept -> euclidean_vector&;

		// move assignment
		auto operator=(euclidean_vector&&) noexcept -> euclidean_vector&;

		~euclidean_vector() = default;

		// subscript for non-const
		auto operator[](int) -> double&;

		// subscript for const
		auto operator[](int) const -> double;

		// Unary plus
		auto operator+() const noexcept -> euclidean_vector;

		// Negation
		auto operator-() const noexcept -> euclidean_vector;

		// Compound Addition
		auto operator+=(euclidean_vector const&) -> euclidean_vector&;
		// Compound Substraction
		auto operator-=(euclidean_vector const&) -> euclidean_vector&;
		// Compound Multiplication
		auto operator*=(double) noexcept -> euclidean_vector&;
		// Compound Devision
		auto operator/=(double) -> euclidean_vector&;

		// std::vector type conversion
		explicit operator std::vector<double>() const noexcept;

		// std::list type conversion
		explicit operator std::list<double>() const noexcept;

		// member function at with copy
		[[nodiscard]] auto at(int) const -> double;

		// member function at with reference
		[[nodiscard]] auto at(int) -> double&;

		[[nodiscard]] auto dimensions() const noexcept -> int {
			return static_cast<int>(dimension_);
		}

		// Friend function ==
		friend auto operator==(euclidean_vector const& lhs, euclidean_vector const& rhs) -> bool {
			if (lhs.dimensions() != rhs.dimensions()) {
				return false;
			}
			return std::equal(lhs.magnitude_.get(),
			                  lhs.magnitude_.get() + lhs.dimension_,
			                  rhs.magnitude_.get());
		}

		// Friend function !=
		friend auto operator!=(euclidean_vector const& lhs, euclidean_vector const& rhs) -> bool {
			return !(lhs == rhs);
		}

		// Friend function +
		friend auto operator+(euclidean_vector const& lhs, euclidean_vector const& rhs)
		   -> euclidean_vector {
			if (lhs.dimension_ != rhs.dimension_) {
				auto x = lhs.dimension_;
				auto y = rhs.dimension_;
				std::stringstream error_stream;
				error_stream << "Dimensions of LHS(";
				error_stream << x;
				error_stream << ") and RHS(";
				error_stream << y;
				error_stream << ") do not match";
				throw euclidean_vector_error(error_stream.str());
			}
			auto copy(lhs);
			copy += rhs;
			return copy;
		}

		// Friend function -
		friend auto operator-(euclidean_vector const& lhs, euclidean_vector const& rhs)
		   -> euclidean_vector {
			if (lhs.dimension_ != rhs.dimension_) {
				auto x = lhs.dimension_;
				auto y = rhs.dimension_;
				std::stringstream error_stream;
				error_stream << "Dimensions of LHS(";
				error_stream << x;
				error_stream << ") and RHS(";
				error_stream << y;
				error_stream << ") do not match";
			}
			auto copy(lhs);
			copy -= rhs;
			return copy;
		}

		// Friend function a*3
		friend auto operator*(euclidean_vector const& lhs, double scalar) -> euclidean_vector {
			auto copy(lhs);
			copy *= scalar;
			return copy;
		}

		// Friend function 3*a
		friend auto operator*(double scalar, euclidean_vector const& rhs) -> euclidean_vector {
			auto copy(rhs);
			copy *= scalar;
			return copy;
		}

		// Friend division
		friend auto operator/(euclidean_vector const& lhs, double dividend) -> euclidean_vector {
			if (dividend == 0.0) {
				throw euclidean_vector_error("Invalid vector division by 0");
			}
			auto copy(lhs);
			copy /= dividend;
			return copy;
		}

		// Friend <<
		friend auto operator<<(std::ostream& os, euclidean_vector const& orig) -> std::ostream& {
			std::stringstream error_stream;
			error_stream << '[';
			std::copy(orig.magnitude_.get(),
			          orig.magnitude_.get() + orig.dimension_,
			          std::ostream_iterator<double>(error_stream, " "));
			if (orig.dimension_ != 0) {
				// if dimension = 0, which means result is []
				// then I do not need to trim the last space beacuse there is no space in between
				// else, I need to trim the last space before ']'
				error_stream.seekp(-1, std::ios_base::end);
			}
			error_stream << "]";
			os << error_stream.str();
			return os;
		}

		// make norm and dot functions to be friend functions
		// sp they can access the maganitude_
		friend auto euclidean_norm(euclidean_vector const& v) -> double;

		friend auto dot(euclidean_vector const& x, euclidean_vector const& y) -> double;

	private:
		// ass2 spec requires we use double[]
		// NOLINTNEXTLINE(modernize-avoid-c-arrays)
		std::unique_ptr<double[]> magnitude_;
		size_t dimension_;

		// cached norm defined as a mutable private variable
		// because norm is always >= 0
		// if norm = -1, we need to calculate the norm
		mutable double norm_ = -1;

		auto reset_norm() -> void {
			norm_ = -1;
		}

		// helper function in copy and move assignment
		auto swap(euclidean_vector& other) -> void {
			std::swap(dimension_, other.dimension_);
			std::swap(magnitude_, other.magnitude_);
			std::swap(norm_, other.norm_);
		}
	};

	// Utility function norm
	auto euclidean_norm(euclidean_vector const& v) -> double;

	// Utility function unit
	auto unit(euclidean_vector const& v) -> euclidean_vector;

	// Utility function dot
	auto dot(euclidean_vector const& x, euclidean_vector const& y) -> double;

} // namespace comp6771
#endif // COMP6771_EUCLIDEAN_VECTOR_HPP
