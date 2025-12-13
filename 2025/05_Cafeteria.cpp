#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

using Range = pair<u_ll, u_ll>;

constexpr char dash = '-';

// Part 1
// so simplest would be to iterate through all ranges every time
// then it would be O(r x f) where r is num ranges and f is number fruits

// If we merge the ranges, then this could reduce time, unless the ranges
// dont really overlap. then we have O(rlogr + r x f) which is bigger, but
// only by the + rlogr, which is basically scalar change.
// but best case, we have O( f ) because the ranges completely collapse.
// so for unmerged ranges, O(r x f) always.
// merged ranges, best case O( f ), worst case O (r x f)

// what if you sorted _both_??
// and then you just have 2 pointers so your run time for scan is O( r )

// then, you have
//     O(flogf) for the sorting food
//     O(rlogr + r) for merge & sort ranges
//     O( r ) for scan
// Total = O( flogf + rlogr + r )
// this is the fastest!

/**
 * @brief Convert the input to vectors.
 */
void parse_input(vector<Range>& ranges, vector<u_ll>& ids, const vector<string>& input_lines) {
	const size_t n = input_lines.size();
	ranges.reserve(n);
	ids.reserve(n);

	bool at_ids = false;
	for (const auto& line : input_lines) {
		if (0 == line.size()) {
			at_ids = true;
			continue;
		}

		if (at_ids) {
			ids.emplace_back(stoull(line));
		} else {
			size_t dash_pos = line.find(dash);
			ranges.emplace_back(Range{stoull(line.substr(0, dash_pos)), stoull(line.substr(dash_pos + 1))});
		}
	}
}

/**
 * @brief take a list of ranges and merge them. O(nlogn)
 */
void merge_ranges(vector<Range>& ranges) {
	sort(ranges.begin(), ranges.end(), [](const Range& a, const Range& b) { return a.first < b.first; });

	const size_t n = ranges.size();
	size_t i_curr_range = 0;

	for (size_t i_forward_range = 1; i_forward_range < n; ++i_forward_range) {
		Range& current_range = ranges[i_curr_range];
		const Range& forward_range = ranges[i_forward_range];

		if (current_range.second < forward_range.first) {
			// this means we cannot merge the forward range into our new one.
			// thus, we must increment current range
			++i_curr_range;
			// copy the new range into the next position,
			// because the forward range could be further out
			ranges[i_curr_range] = forward_range;
		} else {
			// we can incorporate the forward range into the current range
			current_range.second = max(current_range.second, forward_range.second);
		}
	}

	// need to resize the vector to the i_current_range + 1
	ranges.resize(i_curr_range + 1);
}

u_int count_fresh(vector<Range>& ranges, vector<u_ll>& ids) {
	u_int result = 0;

	merge_ranges(ranges);
	sort(ids.begin(), ids.end());

	const size_t n_ranges = ranges.size();
	const size_t n_ids = ids.size();

	size_t i_ranges = 0;
	size_t i_ids = 0;

	while (i_ranges < n_ranges && i_ids < n_ids) {
		const Range& range = ranges[i_ranges];
		const u_ll id = ids[i_ids];
		if (id < range.first) {
			// too lo. increment ids
			++i_ids;
		} else if (id > range.second) {
			// to high, increment ranges
			++i_ranges;
		} else {
			// in range
			++result;
			++i_ids;
		}
	}

	return result;
}

// Part 2 seems even easier. We just can merge and count a the valid ids at the same time
u_ll count_total_possible_fresh(vector<Range>& ranges) {
	u_ll result = 0;
	merge_ranges(ranges);
	for (const Range& range : ranges) {
		result += (range.second - range.first + 1);
	}
	return result;
}

void time_wrap(
	std::function<void(vector<Range>&, vector<u_ll>&)> func,
	vector<Range>& ranges,
	vector<u_ll>& ids) {
	auto start = std::chrono::high_resolution_clock::now();
	func(ranges, ids);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

int solve(const vector<string>& input_lines) {
	// PART 1
	auto part_1 = [](vector<Range>& ranges, vector<u_ll>& ids) {
		u_int num_fresh = count_fresh(ranges, ids);
		cout << "Part 1 : num_fresh=" << num_fresh << "\n";
	};
	vector<Range> ranges;
	vector<u_ll> ids;
	parse_input(ranges, ids, input_lines);
	time_wrap(part_1, ranges, ids);

	// PART 2
	auto part_2 = [](vector<Range>& ranges, vector<u_ll>& ids) {
		u_ll total_possible_fresh = count_total_possible_fresh(ranges);
		cout << "Part 2 : count_total_possible_fresh=" << total_possible_fresh << "\n";
	};
	vector<Range> ranges2;
	vector<u_ll> ids2;
	parse_input(ranges2, ids2, input_lines);
	time_wrap(part_2, ranges2, ids2);

	return 0;
}

void get_input_as_vector(vector<string>& input_lines) {
	string line;
	while (getline(cin, line)) {
		input_lines.push_back(line);
	}
}

int main() {
	vector<string> input_lines;
	get_input_as_vector(input_lines);

	if (0 != solve(input_lines)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
