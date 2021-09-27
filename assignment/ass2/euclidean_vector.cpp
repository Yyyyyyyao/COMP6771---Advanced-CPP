// Copyright (c) Christopher Di Bella.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
#include "comp6771/euclidean_vector.hpp"

namespace comp6771 {
	// Implement solution here
	// test line

	// copy assignment
	auto euclidean_vector::operator=(euclidean_vector const& orig) noexcept -> euclidean_vector& {
		auto copy(orig);
		copy.swap(*this);
		return *this;
	}

	// move assignment
	// I create a copy and swap this with copy
	// copy will be freed once exit
	auto euclidean_vector::operator=(euclidean_vector&& orig) noexcept -> euclidean_vector& {
		// this is move constructor
		auto copy = euclidean_vector(std::move(orig));
		copy.swap(*this);
		return *this;
	}

	// subscript [] non const
	auto euclidean_vector::operator[](int index) -> double& {
		assert(index >= 0 and index < static_cast<int>(dimension_));
		reset_norm();
		return magnitude_[static_cast<size_t>(index)];
	}

	// subscript [] const
	auto euclidean_vector::operator[](int index) const -> double {
		assert(index >= 0 and index < static_cast<int>(dimension_));
		return magnitude_[static_cast<size_t>(index)];
	}

	// Unary plus
	auto euclidean_vector::operator+() const noexcept -> euclidean_vector {
		auto copy = euclidean_vector(*this);
		return copy;
	}

	// Negation
	auto euclidean_vector::operator-() const noexcept -> euclidean_vector {
		auto copy = euclidean_vector(*this);
		std::transform(copy.magnitude_.get(),
		               copy.magnitude_.get() + copy.dimension_,
		               copy.magnitude_.get(),
		               [](double n) { return -n; });
		return copy;
	}

	// Compound Addition
	auto euclidean_vector::operator+=(euclidean_vector const& rhs) -> euclidean_vector& {
		if (rhs.dimension_ != this->dimension_) {
			auto x = this->dimension_;
			auto y = rhs.dimension_;
			std::stringstream error_stream;
			error_stream << "Dimensions of LHS(";
			error_stream << x;
			error_stream << ") and RHS(";
			error_stream << y;
			error_stream << ") do not match";
			throw euclidean_vector_error(error_stream.str());
		}
		reset_norm();
		std::transform(rhs.magnitude_.get(),
		               rhs.magnitude_.get() + rhs.dimension_,
		               magnitude_.get(),
		               magnitude_.get(),
		               std::plus<>());
		return *this;
	}
	// Compound Substraction
	auto euclidean_vector::operator-=(euclidean_vector const& rhs) -> euclidean_vector& {
		if (rhs.dimension_ != this->dimension_) {
			auto x = this->dimension_;
			auto y = rhs.dimension_;
			std::stringstream error_stream;
			error_stream << "Dimensions of LHS(";
			error_stream << x;
			error_stream << ") and RHS(";
			error_stream << y;
			error_stream << ") do not match";
			throw euclidean_vector_error(error_stream.str());
		}
		reset_norm();
		std::transform(magnitude_.get(),
		               magnitude_.get() + dimension_,
		               rhs.magnitude_.get(),
		               magnitude_.get(),
		               std::minus<>());
		return *this;
	}
	// Compound Multiplication
	auto euclidean_vector::operator*=(double scalar) noexcept -> euclidean_vector& {
		std::transform(magnitude_.get(),
		               magnitude_.get() + dimension_,
		               magnitude_.get(),
		               [scalar](double& n) { return n * scalar; });
		reset_norm();
		return *this;
	}
	// Compound Devision
	auto euclidean_vector::operator/=(double dividend) -> euclidean_vector& {
		if (dividend == 0.0) {
			throw euclidean_vector_error("Invalid vector division by 0");
		}
		reset_norm();
		std::transform(magnitude_.get(),
		               magnitude_.get() + dimension_,
		               magnitude_.get(),
		               [dividend](double& n) { return n / dividend; });
		return *this;
	}

	// std::vector type conversion
	euclidean_vector::operator std::vector<double>() const noexcept {
		std::vector<double> vec;
		std::copy(magnitude_.get(), magnitude_.get() + dimension_, back_inserter(vec));
		return vec;
	}

	// std::list type conversion
	euclidean_vector::operator std::list<double>() const noexcept {
		std::list<double> l;
		std::copy(magnitude_.get(), magnitude_.get() + dimension_, back_inserter(l));
		return l;
	}

	// member function at with copy
	[[nodiscard]] auto euclidean_vector::at(int index) const -> double {
		if (index < 0 or index >= static_cast<int>(dimension_)) {
			std::stringstream error_stream;
			error_stream << "Index ";
			error_stream << index;
			error_stream << " is not valid for this euclidean_vector object";
			throw euclidean_vector_error(error_stream.str());
		}
		return magnitude_[static_cast<size_t>(index)];
	}

	// member function at with reference
	[[nodiscard]] auto euclidean_vector::at(int index) -> double& {
		if (index < 0 or index >= static_cast<int>(dimension_)) {
			std::stringstream error_stream;
			error_stream << "Index ";
			error_stream << index;
			error_stream << " is not valid for this euclidean_vector object";
			throw euclidean_vector_error(error_stream.str());
		}
		reset_norm();
		return magnitude_[static_cast<size_t>(index)];
	}

	// Utility function norm
	// I apply friend to this function
	// therefore, I can change the mutable private data member norm_
	auto euclidean_norm(euclidean_vector const& v) -> double {
		if (v.dimensions() == 0) {
			return 0.0;
		}
		if (v.norm_ != -1) {
			return v.norm_;
		}
		auto sum = 0.0;
		std::for_each (v.magnitude_.get(), v.magnitude_.get() + v.dimension_, [&sum](auto const n) {
			sum += n * n;
		});
		v.norm_ = sqrt(sum);
		return v.norm_;
	}

	// Utility function unit
	auto unit(euclidean_vector const& v) -> euclidean_vector {
		if (v.dimensions() == 0) {
			throw euclidean_vector_error("euclidean_vector with no dimensions does not have a unit "
			                             "vector");
		}
		auto norm = euclidean_norm(v);
		if (norm == 0.0) {
			throw euclidean_vector_error("euclidean_vector with zero euclidean normal does not have a "
			                             "unit vector");
		}
		auto copy(v);
		copy /= norm;
		return copy;
	}

	// Utility function dot
	// convert euclidean vector into std::vector to avoid using for loop
	auto dot(euclidean_vector const& x, euclidean_vector const& y) -> double {
		if (x.dimensions() != y.dimensions()) {
			auto x_d = x.dimensions();
			auto y_d = y.dimensions();
			std::stringstream error_stream;
			error_stream << "Dimensions of LHS(";
			error_stream << x_d;
			error_stream << ") and RHS(";
			error_stream << y_d;
			error_stream << ") do not match";
			throw euclidean_vector_error(error_stream.str());
		}

		auto sum = std::inner_product(x.magnitude_.get(),
		                              x.magnitude_.get() + x.dimension_,
		                              y.magnitude_.get(),
		                              0.0);
		return sum;
	}
} // namespace comp6771
