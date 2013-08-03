#pragma once
#include <vector>

namespace augmentations {
	namespace util {
		template<typename value>
		class sorted_vector {
			std::vector<value> raw;
		public:
			void add(const value& val) {
				raw.push_back(val);
				std::sort(raw.begin(), raw.end());
			}

			void remove(const value& val) {
				auto it = std::lower_bound(raw.begin(), raw.end(), val);

				if (it != raw.end() && *it == val)
					raw.erase(it);
			}

			bool find(const value& val) const {
				return std::binary_search(raw.begin(), raw.end(), val);
			}

			const std::vector<value>& get_vector() const {
				return raw;
			}
		};
	}
}