#include <iostream>
#include <vector>
#include <utility> // For std::pair
#include <algorithm> // For std::reverse

using namespace std;

typedef pair<int, int> ii;

vector<ii> generate_intervals(int start, int end) {
    vector<ii> frontier;
    frontier.push_back(ii(start, end));
    int i = 0;
    while (i < (int)frontier.size()) {
        int s = frontier[i].first;
        int e = frontier[i].second;

        i++;

        if (s == e) {
            continue;
        }

        int m = s + (e - s) / 2;

        frontier.push_back(ii(m + 1, e));
        frontier.push_back(ii(s, m));
    }

    vector<ii> retval;
    for (int i = (int)frontier.size() - 1; i >= 0; i--) {
        retval.push_back(frontier[i]);
    }
    return retval;
}

vector<ii> reverse_intervals(const vector<ii>& intervals) {
    vector<ii> reversed = intervals; // Make a copy of the intervals
    std::reverse(reversed.begin(), reversed.end()); // Reverse the copy
    return reversed;
}

vector<vector<ii>> organize_intervals_by_levels(const vector<ii>& intervals_old, int n) {
    vector<vector<ii>> levels;
    int level_size = 1; // Start with the whole interval
    int current_start = 0;

    vector<ii> intervals = reverse_intervals(intervals_old);

    while (level_size < n) {
        vector<ii> level;
        for (int i = 0; i < level_size && current_start < intervals.size(); ++i) {
            level.push_back(intervals[current_start++]);
        }
        levels.push_back(level);
        level_size *= 2; // Double the intervals for the next level
    }

    // Handle the case where the last level might have fewer intervals than expected
    if (!levels.empty() && current_start < intervals.size()) {
        vector<ii> last_level;
        while (current_start < intervals.size()) {
            last_level.push_back(intervals[current_start++]);
        }
        levels.push_back(last_level);
    }

    for (auto& level : levels){
        level = reverse_intervals(level);
    }

    return levels;
}

vector<vector<ii>> divide_intervals_equally(const vector<ii>& intervals, int n) {
    vector<vector<ii>> dividedIntervals(n); // Create n vectors to hold the divided intervals
    int totalIntervals = intervals.size();
    
    // Calculate the number of intervals per vector, and the remainder to distribute the extra intervals
    int intervalsPerVector = totalIntervals / n;
    int remainder = totalIntervals % n;

    int intervalIndex = 0; // Keeps track of the current index in the original intervals vector

    for (int i = 0; i < n; ++i) {
        // Determine how many intervals this vector should have
        int count = intervalsPerVector + (i < remainder ? 1 : 0);
        
        // Assign intervals to the current vector
        for (int j = 0; j < count; ++j) {
            dividedIntervals[i].push_back(intervals[intervalIndex++]);
        }
    }

    return dividedIntervals;
}

int main() {
    int n = 16; // Example size, adjust as needed
    auto intervals = generate_intervals(0, n - 1);

    vector<vector<ii>> divided = divide_intervals_equally(intervals, 4);

    // Print the divided intervals
    for (int i = 0; i < divided.size(); ++i) {
        cout << "Vector " << i << ":" << endl;
        for (const auto& interval : divided[i]) {
            cout << "(" << interval.first << ", " << interval.second << ") ";
        }
        cout << endl;
    }
    return 0;
}
