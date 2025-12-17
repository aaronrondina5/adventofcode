#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

struct Coords {
	u_ll x;
	u_ll y;
};

constexpr char COMMA = ',';

// Part 1
// The most simple approach is to simply do an iteration of all pairs
// will implement this for the time being.
u_ll distance_between_inclusive(const u_ll p1, const u_ll p2) {
	return (p1 > p2 ? p1 - p2 : p2 - p1) + 1;
}

u_ll area(const Coords& coord_1, const Coords& coord_2) {
	return distance_between_inclusive(coord_1.x, coord_2.x) *
		   distance_between_inclusive(coord_1.y, coord_2.y);
}

void parse_input(vector<Coords>& result, const vector<string>& input) {
	for (const string& line : input) {
		const size_t pos_comma = line.find(COMMA);
		result.emplace_back(Coords{stoull(line.substr(0, pos_comma)), stoull(line.substr(pos_comma + 1))});
	}
}

u_ll max_rectangle_quadratic(const vector<string>& input) {
	vector<Coords> coords;
	parse_input(coords, input);

	const size_t n = coords.size();
	u_ll max_size = 0;
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = i + 1; j < n; ++j) {
			max_size = max(max_size, area(coords[i], coords[j]));
		}
	}
	return max_size;
}

/*
 * Part 2
 * This is a big problem, and there may be many iterations to make more efficient
 *
 * After a long time of thinking about this, I am going to begin by simply trying
 * to make this grid. Looking at the size of the inputs, the grid is going to be
 * way to large as is. Thus, I am going to attempt to try to compress all the
 * unnecessary space.
 * Things that are in the same row and column should remain that way.
 * the space between corners should be compressed.
 *
 * I think first I can sort by x, then by y within each x chunk
 *
 * For example:
 *    [ (1, 2), (1, 4), (4, 1), (4, 5), (4, 5), (5, 7), ... ]
 *
 * Then we can begin to insert one x chunk at a time. The tricky thing
 * becomes - how do we know the right y index? Well, we need to know how
 * many unique y values are below our x values. So, we can create a vector
 * of unique y values an binary search.
 */

enum Direction { LEFT, RIGHT, UP, DOWN };

struct Corner {
	Coords original_coords;
	Coords mapped_coords;
	u_int original_index;
};

constexpr char SPACE = ' ';
constexpr char CORNER = 'X';

void fill_polygon(
	vector<vector<char>>& grid,
	const vector<Corner>& original_corners,
	const size_t idx_lowest_x) {
	// TODO
}

void create_polygon_borders(vector<vector<char>>& grid, const vector<Corner>& corners) {
	// TODO
}

template <typename TIdx, typename TItem>
TIdx search_sorted_list(const vector<TItem>& list, TItem x) {
	TIdx lo = 0;
	TIdx hi = list.size();
	while (lo < hi) {
		TIdx mid = (lo + hi) / 2;

		if (list[mid] < y) {
			lo = mid + 1;
		} else if (list[mid] > y) {
			hi = mid;
		} else {
			return mid;
		}
	}
	return lo;
}

// will sort first by x, then by y within the ranges of same x
// ie - [ (1, 2), (1, 4), (4, 1), (4, 5), (4, 5), (5, 7), ... ]
void sort_by_y_within_x_chunk(vector<Corner>& result) {
	const size_t n_corners = result.size();
	vector<size_t> breaks;
	breaks.emplace_back(0); // first inclusive break
	u_int last_x = result[0].original_coords.x;
	for (size_t i = 1; i < n_corners; ++i) {
		if (result[i].original_coords.x != last_x) {
			breaks.emplace_back(i);
			last_x = result[i].original_coords.x;
		}
	}
	breaks.emplace_back(n_corners); // last exclusive break

	auto compare_least_y = [](const Corner& a, const Corner& b) {
		return a.original_coords.y < b.original_coords.y;
	};
	for (size_t i = 1; i < breaks.size(); ++i) {
		sort(result.begin() + breaks[i - 1], result.begin() + breaks[i], compare_least_y);
	}
}

void sort_by_x(vector<Corner>& input) {
	const auto cmpr_x_less = [](const Corner& a, const Corner& b) {
		return a.original_coords.x < b.original_coords.x;
	};
	sort(input.begin(), input.end(), cmpr_x_less);
}

void resolve_unique_y_values(vector<u_ll>& result, const vector<Corner>& input) {
	set<u_ll> seen;
	for (const Corner& c : input) {
		if (seen.insert(c.original_coords.y).second) {
			// The value was new
			result.emplace_back(c.original_coords.y);
		}
	}
}

/**
 * @brief will create a compressed grid given the corner values.
 *        will only insert the corners in the grid & not fill.
 *
 * @param corners_original original list of sorted corners. New coordinates will be added here
 * @param corners_sorted_out this function will sort by x, then y within x.
 * @param grid_out resulting compressed grid
 */
// will create a compressed grid given the corner values.
// will only insert the corners in the grid & not fill
// @param corners_original
void create_compressed_grid(
	vector<Corner>& corners_original,
	vector<Corner>& corners_sorted_out,
	vector<vector<char>>& grid_out) {

	const u_ll n = corners_original.size();

	// sort the corners for grid construction
	sort_by_x(corners_sorted_out);
	sort_by_y_within_x_chunk(corners_sorted_out);

	// get unique y values.
	// this will help later to emplace the values in the correct y index
	vector<u_ll> unique_y_values_sorted;
	resolve_unique_y_values(unique_y_values_sorted, corners_sorted_out);
	sort(unique_y_values_sorted.begin(), unique_y_values_sorted.end());

	u_ll i_corner = 0;				// current index in the corners list
	u_ll compressed_x_idx_curr = 0; // current index in the compressed grid
	while (i_corner < n) {

		// for every coordinate of the same x value,
		// find the right y position and place in the grid.
		const u_ll original_x = corners_sorted_out[i_corner].original_coords.x;
		u_ll compressed_y_idx_curr = 0;
		while (corners_sorted_out[i_corner].original_coords.x == original_x) {

			// look through corners for the number of unique values higher than the last
			// y idx and lower than this y index
			const u_ll original_y = corners_sorted_out[i_corner].original_coords.y;
			compressed_y_idx_curr = search_sorted_list<u_ll, u_ll>(unique_y_values_sorted, original_y);
			grid_out[compressed_x_idx_curr][compressed_y_idx_curr] = CORNER;

			// update the original value with the mapped coordinates
			u_ll original_idx = corners_sorted_out[i_corner].original_index;
			Corner& original_corner = corners_original[original_idx];
			original_corner.mapped_coords.x = compressed_x_idx_curr;
			original_corner.mapped_coords.y = compressed_y_idx_curr;

			// increment the corner counter
			++i_corner;
		}

		// we have found all the corners with the same x value.
		// increment the compressed x idx
		++compressed_x_idx_curr;
	}
}

// expects result to be initialized to correct size
void parse_input_to_corners(vector<Corner>& result, const vector<string>& input) {
	const u_ll n = input.size();
	if (result.size() != n) {
		throw runtime_error("Invalid size");
	}
	for (u_ll i = 0; i < n; ++i) {
		Corner& corner = result[i];
		corner.original_index = i;

		const string& line = input[i];
		const size_t pos_comma = line.find(COMMA);
		corner.original_coords.x = stoull(line.substr(0, pos_comma));
		corner.original_coords.y = stoull(line.substr(pos_comma + 1));
	}
}

u_ll max_rectangle_part_2(const vector<string>& input) {
	const u_ll n = input.size();

	// parse to corners, containing the original coordinates
	Corner base;
	vector<Corner> corners_original(n, Corner{});
	parse_input_to_corners(corners_original, input);

	// map the corners to a compressed grid
	vector<Corner> corners_sorted(n, Corner{});
	vector<vector<char>> grid(n, vector<char>(n, SPACE));
	create_compressed_grid(corners_original, corners_sorted, grid);

	return 1ULL;
}

void time_wrap(function<void(const vector<string>&)> func, const vector<string>& input) {
	auto start = chrono::system_clock::now();
	func(input);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << endl;
}

int solve(vector<string>& input_lines) {
	auto solve_1 = [](const vector<string>& input) {
		u_ll result = max_rectangle_quadratic(input);
		cout << "Part 1 : max_rectangle_quadratic=" << result << "\n";
	};
	// time_wrap(solve_1, input_lines);

	auto solve_2 = [](const vector<string>& input) {
		u_ll result = max_rectangle_part_2(input);
		cout << "Part 2 : max_rectangle_part_2=" << result << "\n";
	};
	time_wrap(solve_2, input_lines);

	return 0;
}

void get_input_as_vector(vector<string>& input) {
	string line;
	while (getline(cin, line)) {
		input.push_back(line);
	}
}

int main() {
	vector<string> input;
	get_input_as_vector(input);

	if (0 != solve(input)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
