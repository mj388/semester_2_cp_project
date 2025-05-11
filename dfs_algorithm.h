#include <vector>

using namespace std;

struct DFSStats {
    int totalMoves;
    long long elapsedMillis;
};

extern DFSStats dfs_stats;

vector<pair<vector<vector<int>>, pair<int, int>>> dfs_algorithm(vector<vector<int>> maze);