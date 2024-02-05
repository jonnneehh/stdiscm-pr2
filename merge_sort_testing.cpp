#include <iostream>
#include <vector>
#include <utility> // For std::pair
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <chrono>  // For high-resolution clock
#include <thread>
#include <algorithm> // For std::reverse

using namespace std;

typedef pair<int, int> ii;

vector<ii> generate_intervals(int start, int end);
vector<ii> reverse_intervals(const vector<ii> &intervals);
vector<vector<ii>> organize_intervals_by_levels(const vector<ii> &intervals_old);
vector<vector<ii>> divide_intervals_equally(const vector<ii> &intervals, int n);

void merge(vector<int> &array, int s, int e);

int main()
{
    // Seed your randomizer
    srand(time(NULL));

    // Get array size from user
    int size = 5000000;
    int thread_count = 4;
    // cout << "Enter the size of the array: ";
    // cin >> size;
    // cout << "Enter the number of threads: ";
    // cin >> thread_count;

    // Generate a random array of given size
    vector<int> array(size);
    for (int &val : array)
    {
        val = rand() % size;
    }

    // Start timing the sorting process
    auto start = chrono::high_resolution_clock::now();

    // Call the generate_intervals method to generate the merge sequence
    vector<ii> intervals = generate_intervals(0, size - 1);
    vector<vector<ii>> levels = organize_intervals_by_levels(intervals);

    // SINGLE THREAD IMPLEMENTATION
    // for(ii interval : intervals){
    //     merge(array, interval.first, interval.second);
    // }

    // MULTITHREAD IMPLEMENTATION
    std::vector<std::thread> threads;
    for (int i = levels.size() - 1; i >= 0; --i)
    {
        const auto &level = levels[i];
        vector<vector<ii>> divided_intervals = divide_intervals_equally(level, thread_count);

        for (const auto &div_interval : divided_intervals)
        {
            threads.emplace_back([&array, div_interval](){
                for(ii interval : div_interval){
                    merge(array, interval.first, interval.second);
                }});
        }
        for (auto &t : threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

    // End timing the sorting process
    auto end = chrono::high_resolution_clock::now();
    // Calculate the duration
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    // Print the time taken for the sorting process
    cout << "Time taken for sorting: " << duration << " milliseconds." << endl;

    // DISPLAY THE ARRAY
    // for (int e : array)
    // {
    //     cout << e << " ";
    // }
    return 0;
}

vector<vector<ii>> divide_intervals_equally(const vector<ii> &intervals, int n)
{
    vector<vector<ii>> dividedIntervals(n); // Create n vectors to hold the divided intervals
    int totalIntervals = intervals.size();

    // Calculate the number of intervals per vector, and the remainder to distribute the extra intervals
    int intervalsPerVector = totalIntervals / n;
    int remainder = totalIntervals % n;

    int intervalIndex = 0; // Keeps track of the current index in the original intervals vector

    for (int i = 0; i < n; ++i)
    {
        // Determine how many intervals this vector should have
        int count = intervalsPerVector + (i < remainder ? 1 : 0);

        // Assign intervals to the current vector
        for (int j = 0; j < count; ++j)
        {
            dividedIntervals[i].push_back(intervals[intervalIndex++]);
        }
    }

    return dividedIntervals;
}

vector<ii> reverse_intervals(const vector<ii> &intervals)
{
    vector<ii> reversed = intervals;                // Make a copy of the intervals
    std::reverse(reversed.begin(), reversed.end()); // Reverse the copy
    return reversed;
}

vector<vector<ii>> organize_intervals_by_levels(const vector<ii> &intervals_old)
{
    vector<vector<ii>> levels;
    int level_size = 1; // Start with the whole interval
    int current_start = 0;
    int n = intervals_old.size();

    vector<ii> intervals = reverse_intervals(intervals_old);

    while (level_size < n)
    {
        vector<ii> level;
        for (int i = 0; i < level_size && current_start < intervals.size(); ++i)
        {
            level.push_back(intervals[current_start++]);
        }
        levels.push_back(level);
        level_size *= 2; // Double the intervals for the next level
    }

    // Handle the case where the last level might have fewer intervals than expected
    if (!levels.empty() && current_start < intervals.size())
    {
        vector<ii> last_level;
        while (current_start < intervals.size())
        {
            last_level.push_back(intervals[current_start++]);
        }
        levels.push_back(last_level);
    }

    for (auto &level : levels)
    {
        level = reverse_intervals(level);
    }

    return levels;
}

vector<ii> generate_intervals(int start, int end)
{
    vector<ii> frontier;
    frontier.push_back(ii(start, end));
    int i = 0;
    while (i < (int)frontier.size())
    {
        int s = frontier[i].first;
        int e = frontier[i].second;

        i++;

        if (s == e)
        {
            continue;
        }

        int m = s + (e - s) / 2;

        frontier.push_back(ii(m + 1, e));
        frontier.push_back(ii(s, m));
    }

    vector<ii> retval;
    for (int i = (int)frontier.size() - 1; i >= 0; i--)
    {
        retval.push_back(frontier[i]);
    }
    return retval;
}

void merge(vector<int> &array, int s, int e)
{
    int m = s + (e - s) / 2;
    vector<int> left, right;
    for (int i = s; i <= m; i++)
    {
        left.push_back(array[i]);
    }
    for (int i = m + 1; i <= e; i++)
    {
        right.push_back(array[i]);
    }

    int l_ptr = 0, r_ptr = 0, arr_idx = s;

    while (l_ptr < left.size() && r_ptr < right.size())
    {
        if (left[l_ptr] <= right[r_ptr])
        {
            array[arr_idx++] = left[l_ptr++];
        }
        else
        {
            array[arr_idx++] = right[r_ptr++];
        }
    }

    while (l_ptr < left.size())
    {
        array[arr_idx++] = left[l_ptr++];
    }

    while (r_ptr < right.size())
    {
        array[arr_idx++] = right[r_ptr++];
    }
}