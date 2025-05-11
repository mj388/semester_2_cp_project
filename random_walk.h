#include <vector>

using namespace std;

struct RandomWalkStats {
    int totalMoves;
    long long elapsedMillis;
};

extern RandomWalkStats random_stats;

vector<pair<vector<vector<int>>, pair<int, int>>> random_walk_algorithm(vector<vector<int>> maze);