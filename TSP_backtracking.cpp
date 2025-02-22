#include <iostream>
#include <vector>
#include <climits>

using namespace std;

const int N = 10; // Số lượng điểm
vector<vector<int>> expense = {
    {0, 29, 20, 21, 16, 31, 100, 12, 4, 31},
    {29, 0, 15, 17, 28, 40, 90, 25, 6, 50},
    {20, 15, 0, 28, 39, 60, 70, 21, 8, 45},
    {21, 17, 28, 0, 14, 25, 80, 30, 5, 35},
    {16, 28, 39, 14, 0, 22, 89, 17, 12, 41},
    {31, 40, 60, 25, 22, 0, 99, 26, 19, 38},
    {100, 90, 70, 80, 89, 99, 0, 29, 50, 20},
    {12, 25, 21, 30, 17, 26, 29, 0, 24, 32},
    {4, 6, 8, 5, 12, 19, 50, 24, 0, 18},
    {31, 50, 45, 35, 41, 38, 20, 32, 18, 0}
};

vector<bool> visited(N, false);
int minCost = INT_MAX;

void tsp(int currPos, int count, int cost, int start) {
    if (count == N && expense[currPos][start] > 0) {
        minCost = min(minCost, cost + expense[currPos][start]);
        return;
    }

    for (int i = 0; i < N; i++) {
        if (!visited[i] && expense[currPos][i] > 0) {
            visited[i] = true;
            tsp(i, count + 1, cost + expense[currPos][i], start);
            visited[i] = false;
        }
    }
}

int main() {
    visited[0] = true; // Bắt đầu từ đỉnh 0
    tsp(0, 1, 0, 0);
    cout << "Chi phí nhỏ nhất của chu trình Hamilton: " << minCost << endl;
    return 0;
}
