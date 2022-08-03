#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

typedef std::pair<int64_t, int64_t> Coordinate;

struct PairHash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &pair) const {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

class Conway {
public:
  Conway(const std::vector<Coordinate> &coordinates)
      : alive_cells_(coordinates.begin(), coordinates.end()) {}

  void next_generation() {
    std::unordered_map<Coordinate, int, PairHash> neighbors;

    // Generate neighbor counts using alive cells
    for (const auto &[x, y] : alive_cells_) {
      for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
          if (check_overflow(x, i) || check_overflow(y, j))
            continue;
          if (i == 0 && j == 0)
            continue;
          Coordinate c = {x + i, y + j};
          neighbors[c]++;
        }
      }
    }

    // Check which dead cells have exactly 3 alive neighbors
    std::vector<Coordinate> new_alive_cells;
    for (const auto &[coordinate, count] : neighbors) {
      if (alive_cells_.find(coordinate) == alive_cells_.end() && count == 3) {
        new_alive_cells.push_back(coordinate);
      }
    }

    // Erase alive cells that have less than 2 or greater than 3 neighbors
    for (auto it = alive_cells_.begin(); it != alive_cells_.end();) {
      auto coordinate = *it;

      if (neighbors.find(coordinate) == neighbors.end() ||
          neighbors[coordinate] < 2 || neighbors[coordinate] > 3) {
        alive_cells_.erase(it++);
      } else {
        ++it;
      }
    }

    // Add dead cells that turned alive
    std::copy(new_alive_cells.begin(), new_alive_cells.end(),
              std::inserter(alive_cells_, alive_cells_.end()));
  }

  friend std::ostream &operator<<(std::ostream &stream, const Conway &conway) {
    for (const auto &[x, y] : conway.alive_cells_) {
      stream << x << " " << y << std::endl;
    }

    return stream;
  }

private:
  bool check_overflow(int64_t a, int64_t b) {
    if (a > 0 && b > std::numeric_limits<int64_t>::max() - a)
      return true; // `a + b` would overflow
    if (a < 0 && b < std::numeric_limits<int64_t>::min() - a)
      return true; // `a + b` would underflow:w
    return false;
  }

  std::unordered_set<Coordinate, PairHash> alive_cells_;
};

Coordinate parse_coordinate(std::string input) {
  std::regex re("[\\(\\),]");
  input.erase(std::remove_if(input.begin(), input.end(), ::isspace));
  std::sregex_token_iterator first(input.begin(), input.end(), re, -1), last;
  std::vector<std::string> tokens(first, last);
  return {std::stol(tokens[1]), std::stol(tokens[2])};
}

int main() {
  std::string input;
  std::vector<Coordinate> coordinates;
  while (std::getline(std::cin, input)) {
    if (input.empty())
      break;
    coordinates.push_back(parse_coordinate(input));
  }

  Conway c(coordinates);
  for (int i = 0; i < 10; ++i) {
    c.next_generation();
  }

  std::cout << c << std::endl;

  return 0;
}