#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

using ll = long long;

struct Coords {
	ll x;
	ll y;
};

constexpr char COMMA = ',';

// Part 1
// The most simple approach is to do an iteration of all pairs
// will implement this for the time being.
ll area(const Coords& a, const Coords& b) {
	auto [min_x, max_x] = minmax(a.x, b.x);
	auto [min_y, max_y] = minmax(a.y, b.y);
	return (max_x - min_x + 1) * (max_y - min_y + 1);
}

void parse_input(vector<Coords>& result, const vector<string>& input) {
	for (const string& line : input) {
		const size_t pos_comma = line.find(COMMA);
		result.emplace_back(Coords{stoll(line.substr(0, pos_comma)), stoll(line.substr(pos_comma + 1))});
	}
}

ll max_rectangle_quadratic(const vector<string>& input) {
	vector<Coords> coords;
	parse_input(coords, input);

	const size_t n = coords.size();
	ll max_size = 0;
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
 *
 * -------------------------------------
 * Okay, I've figured out how to compress the grid.
 * Now for the calculation.
 *
 * The problem states that you still have to use 2 red tiles as corners
 * This simplifies things.
 *
 * Brute force, we iterate through pairs, and then iterate
 * within the corner bounds and check if we cross a boundary.
 *
 * We could do better. We dynamically update the GridItems to
 * indicate the next available to the right and next available down & up.
 * Then the complexity just goes back to n^2,
 *
 * --------------------------------------
 * Overall Complexity:
 *
 * Time: O(nlogn + n + klogk + nlogk + k*j + j * n^2 ) = O(j * n^2)
 * Space: O(n + k*j)
 *
 *  - n : the number of coordinates
 *  - k : number unique y coords
 *  - j : number unique x coordinates
 */

enum Direction { LEFT, RIGHT, UP, DOWN };

struct Corner {
	Corner() : original_coords{}, mapped_coords{}, original_index{-1}, max_size{0LL} {}

	Coords original_coords;
	Coords mapped_coords;
	ll original_index;
	ll max_size;
};

std::ostream& operator<<(std::ostream& os, const Corner& corner) {
	os << "original=[x=" << corner.original_coords.x << ", y=" << corner.original_coords.y
	   << "] -> mapped=[x=" << corner.mapped_coords.x << ", y=" << corner.mapped_coords.y << "]"
	   << " idx=" << corner.original_index;
	return os;
}

constexpr char VERTICAL_BORDER = '|';
constexpr char HORIZONTAL_BORDER = '-';
constexpr char CORNER = 'X';
constexpr char DOT = '.';

struct GridItem {
	GridItem() : symbol{DOT}, relative_height{0LL} {}
	char symbol;
	ll relative_height;
};
using Grid = vector<vector<GridItem>>;

void print_grid(const Grid& grid) {
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();
	stringstream ss;
	for (size_t row = 0; row < rows; ++row) {
		for (size_t col = 0; col < cols; ++col) {
			ss << " " << grid[row][col].symbol << "(" << grid[row][col].relative_height << ") ";
		}
		ss << "\n";
	}
	cout << ss.str() << endl;
}

/**
 * @brief Assuming that our grid has stored the relative heights of each position,
 *        we only have to traverse horizontally, and check that the relative heights of
 *        each are greater than the vertical distance
 */
ll max_rectangle_for_corners(const Grid& grid, const Corner& a, const Corner& b) {
	ll potential_result = area(a.original_coords, b.original_coords);
	ll a_max = a.max_size;
	ll b_max = b.max_size;
	if (potential_result < a_max || potential_result < b_max) {
		// no point in checking this pair
		return 0LL;
	}

	auto [min_x, max_x] = minmax(a.mapped_coords.x, b.mapped_coords.x);
	auto [min_y, max_y] = minmax(a.mapped_coords.y, b.mapped_coords.y);
	const ll vertical_distance = max_y - min_y + 1;

	for (ll col = min_x; col <= max_x; ++col) {
		if (grid[max_y][col].relative_height < vertical_distance) {
			return 0LL;
		}
	}

	a.max_size = max(a_max, potential_result);
	b.max_size = max(b_max, potential_result);
	return potential_result;
}

/**
 * @brief Store the relative height of each position, representing the height
 *        from the border above, keeping in sync with the original coordinate
 *        representation of height being top down.
 *
 *        if you are in the polygon, you will increase the relative height as you go
 *        up by one. the first border hit will have a height of 1, since height is inclusive.
 */
void make_heights_dp(Grid& grid) {
	const size_t rows = grid.size();
	if (0 == rows)
		return;
	const size_t cols = grid[0].size();

	for (size_t col = 0; col < cols; ++col) {
		bool in_polygon = false;
		for (size_t row = 0; row < rows; ++row) {
			GridItem& item = grid[row][col];
			if (!in_polygon) {
				if (item.symbol == CORNER || item.symbol == HORIZONTAL_BORDER) {
					in_polygon = true;
					item.relative_height = 1;
				}
			} else {
				item.relative_height = grid[row - 1][col].relative_height + 1;
				if (item.symbol == CORNER || item.symbol == HORIZONTAL_BORDER) {
					in_polygon = false;
				}
			}
		}
	}
}

ll get_max_rectangle(Grid& grid, vector<Corner>& corners) {
	// pre-populate the relative heights so the
	// max_rectangle_for_corners only has to traverse in 1 dimension
	make_heights_dp(grid);

	ll result = 0LL;
	const size_t n = corners.size();
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = i + 1; j < n; ++j) {
			result = max(result, max_rectangle_for_corners(grid, corners[j], corners[i]));
		}
	}
	return result;
}

void add_vertical_border(Grid& grid, const ll x, const ll y_lo_exclusive, const ll y_hi_exclusive) {
	ll lo = y_lo_exclusive + 1;
	ll hi = y_hi_exclusive - 1;
	while (lo <= hi) {
		grid[lo][x].symbol = VERTICAL_BORDER;
		++lo;
	}
}

void add_horizontal_border(Grid& grid, const ll y, const ll x_lo_exclusive, const ll x_hi_exclusive) {
	ll lo = x_lo_exclusive + 1;
	ll hi = x_hi_exclusive - 1;
	while (lo <= hi) {
		grid[y][lo].symbol = HORIZONTAL_BORDER;
		++lo;
	}
}

void connect_corners(Grid& grid, const Corner& c1, const Corner& c2) {
	auto [min_x, max_x] = minmax(c1.mapped_coords.x, c2.mapped_coords.x);
	auto [min_y, max_y] = minmax(c1.mapped_coords.y, c2.mapped_coords.y);

	if (min_x == max_x) {
		// these are on the same column - need to add a vertical connection b/w rows
		add_vertical_border(grid, min_x, min_y, max_y);
	} else if (min_y == max_y) {
		// on the same row - need to add a horizontal connection b/w cols
		add_horizontal_border(grid, min_y, min_x, max_x);
	} else {
		throw runtime_error("x or y must be the same");
	}
}

void create_polygon_borders(Grid& grid, const vector<Corner>& corners) {
	const size_t n = corners.size();
	for (size_t i = 0; i < n - 1; ++i) {
		connect_corners(grid, corners[i], corners[i + 1]);
	}
	connect_corners(grid, corners[n - 1], corners[0]);
}

// run a binary search on a sorted list
template <typename TIdx, typename TItem>
TIdx search_sorted_list(const vector<TItem>& list, TItem x) {
	TIdx lo = 0;
	TIdx hi = list.size();
	while (lo < hi) {
		TIdx mid = (lo + hi) / 2;

		if (list[mid] < x) {
			lo = mid + 1;
		} else if (list[mid] > x) {
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
	ll last_x = result[0].original_coords.x;
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

void resolve_unique_y_values(vector<ll>& result, const vector<Corner>& input) {
	set<ll> seen;
	for (const Corner& c : input) {
		if (seen.insert(c.original_coords.y).second) {
			// The value was new
			result.emplace_back(c.original_coords.y);
		}
	}
}

ll get_num_unique_x(const vector<Corner>& input) {
	set<ll> seen;
	for (const Corner& c : input) {
		seen.emplace(c.original_coords.x);
	}
	return seen.size();
}

/**
 * @brief will create a compressed grid given the corner values.
 *        will only insert the corners in the grid & not fill.
 *
 * @param corners_in original list of sorted corners. New coordinates will be added here
 * @return resulting compressed grid that contains the indices of corners
 *         empty positions will contain -1
 */
Grid create_compressed_grid(vector<Corner>& corners_in) {
	// sort the corners for grid construction
	vector<Corner> corners_sorted(corners_in);
	sort_by_x(corners_sorted);
	sort_by_y_within_x_chunk(corners_sorted);

	// get unique y values.
	// this will help later to emplace the values in the correct y index
	vector<ll> unique_y_values_sorted;
	resolve_unique_y_values(unique_y_values_sorted, corners_sorted);
	sort(unique_y_values_sorted.begin(), unique_y_values_sorted.end());

	// grid only needs to be size of num unique values
	ll num_unique_x = get_num_unique_x(corners_in);
	Grid result(unique_y_values_sorted.size(), vector<GridItem>(num_unique_x, GridItem{}));

	ll i_corner = 0;			  // current index in the corners list
	ll compressed_x_idx_curr = 0; // current index in the compressed grid
	const ll n_total_corners = corners_in.size();
	while (i_corner < n_total_corners) {

		// for every corner that has the same x value, find the appropriate y
		const ll this_iteration_x = corners_sorted[i_corner].original_coords.x;
		ll compressed_y_idx_curr = 0;
		while (i_corner < n_total_corners && this_iteration_x == corners_sorted[i_corner].original_coords.x) {
			const Corner& corner_in_sorted = corners_sorted[i_corner];

			// look through corners for the number of unique values higher than the last
			// y idx and lower than this y index
			const ll original_y = corner_in_sorted.original_coords.y;
			compressed_y_idx_curr = search_sorted_list<ll, ll>(unique_y_values_sorted, original_y);

			// update the original value with the mapped coordinates
			ll original_idx = corner_in_sorted.original_index;
			Corner& original_corner = corners_in[original_idx];
			original_corner.mapped_coords.x = compressed_x_idx_curr;
			original_corner.mapped_coords.y = compressed_y_idx_curr;

			// set the grid item as a corner
			GridItem& this_item = result[compressed_y_idx_curr][compressed_x_idx_curr];
			this_item.symbol = CORNER;

			// increment the corner counter
			++i_corner;
		}

		// we have found all the corners with the same x value.
		// increment the compressed x idx
		++compressed_x_idx_curr;
	}

	return move(result);
}

// expects result to be initialized to correct size
void parse_input_to_corners(vector<Corner>& result, const vector<string>& input) {
	const ll n = input.size();
	if (result.size() != n) {
		throw runtime_error("Invalid size");
	}
	for (ll i = 0; i < n; ++i) {
		Corner& corner = result[i];
		corner.original_index = i;

		const string& line = input[i];
		const size_t pos_comma = line.find(COMMA);
		corner.original_coords.x = stoull(line.substr(0, pos_comma));
		corner.original_coords.y = stoull(line.substr(pos_comma + 1));
	}
}

ll max_rectangle_part_2(const vector<string>& input) {
	const ll n = input.size();

	// parse to corners, containing the original coordinates
	Corner base;
	vector<Corner> corners(n, Corner{});
	parse_input_to_corners(corners, input);

	// map the corners to a compressed grid
	Grid grid = create_compressed_grid(corners);
	print_grid(grid);

	// create the borders
	create_polygon_borders(grid, corners);
	print_grid(grid);

	// solve using our compressed grid & corners
	return get_max_rectangle(grid, corners);
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
		ll result = max_rectangle_quadratic(input);
		cout << "Part 1 : max_rectangle_quadratic=" << result << "\n";
	};
	// time_wrap(solve_1, input_lines);

	auto solve_2 = [](const vector<string>& input) {
		ll result = max_rectangle_part_2(input);
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
