#pragma once
#include <array>

namespace augs  {
	template<class T, int const_count>
	class constant_size_vector {
		typedef std::array<T, const_count> arr_type;

		constant_size_vector(constant_size_vector&&) = delete;

		size_t count = 0;
	public:
		constant_size_vector(const constant_size_vector&) = default;
		constant_size_vector& operator=(constant_size_vector&&) = delete;
		
		constant_size_vector() = default;
		constant_size_vector& operator=(const constant_size_vector&) = default;

		constant_size_vector& operator=(const std::vector<T>& t) {
			count = 0;
			for (size_t i = 0; i < t.size(); ++i) {
				push_back(t[i]);
			}

			return *this;
		}

		arr_type raw;

		void push_back(const T& obj) {
			raw[count++] = obj;
		}

		T& operator[](size_t i) {
			return raw[i];
		}

		typename arr_type::iterator begin() {
			return raw.begin();
		}

		typename arr_type::iterator end() {
			return raw.begin() + size();
		}

		typename arr_type::const_iterator begin() const {
			return raw.begin();
		}

		typename arr_type::const_iterator end() const {
			return raw.begin() + size();
		}

		size_t size() const {
			return count;
		}

		size_t capacity() const {
			return const_count;
		}

		void clear() {
			for (auto& e : raw) {
				e = T();
			}

			count = 0;
		}
	};
}