#include <bits/stdc++.h>
using namespace std;

// Hàm tạo số ngẫu nhiên trong khoảng [a, b]
int getRandomNumber(int a, int b) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(a, b);
    return dist(gen);
}

// Lớp Way đại diện cho một hành trình
class Way {
public:
    vector<int> Permution;
    vector<vector<int>>& Expense;
    int cost;

    // Constructor
    Way(vector<int> permution, vector<vector<int>>& expense)
        : Permution(permution), Expense(expense), cost(0) {

        if (permution.empty()) {
            cout << "[DEBUG] Lỗi: Đường đi rỗng!" << endl;
            return;
        }

        for (size_t i = 1; i < Permution.size(); i++) {
            cost += Expense[Permution[i - 1]][Permution[i]];
        }
        cost += Expense[Permution.back()][Permution.front()];

        // In debug thông tin đường đi
        cout << "[DEBUG] Tạo đường đi: ";
        for (int city : Permution) cout << city << " ";
        cout << "| Chi phí: " << cost << endl;
    }
};

// Hàm tạo cá thể bằng phương pháp Random Shuffle
void insert_Random_shuffle(vector<Way>& Member, int base, int P, vector<vector<int>>& expense) {
    vector<int> vec(base);
    iota(vec.begin(), vec.end(), 0);

    set<vector<int>> S;
    random_device rd;
    mt19937 gen(rd());

    cout << "[DEBUG] insert_Random_shuffle bắt đầu, P = " << P << endl;

    while (S.size() < P) {
        vector<int> shuffled = vec;
        shuffle(shuffled.begin(), shuffled.end(), gen);
        S.insert(shuffled);
    }

    for (const auto& x : S) {
        Member.emplace_back(Way(x, expense));
    }

    cout << "[DEBUG] Số cá thể từ Random Shuffle: " << Member.size() << endl;
}

// Hàm tạo cá thể bằng phương pháp Nearest Neighbor
void insert_nearest_neighbor(vector<Way>& Member, int base, int P, vector<vector<int>>& expense) {
    set<vector<int>> T;

    cout << "[DEBUG] insert_nearest_neighbor bắt đầu, P = " << P << endl;

    while (T.size() < P) {
        int start = getRandomNumber(0, base - 1);
        cout << "[DEBUG] Chọn điểm xuất phát: " << start << endl;

        vector<int> vec;
        vec.push_back(start);
        set<int> visited;
        visited.insert(start);

        while (vec.size() < base) {
            int last = vec.back();
            int nearest_city = -1;
            int min_distance = INT_MAX;

            for (int i = 0; i < base; i++) {
                if (!visited.count(i) && expense[last][i] < min_distance) {
                    min_distance = expense[last][i];
                    nearest_city = i;
                }
            }

            if (nearest_city == -1) {
                cout << "[DEBUG] Không tìm thấy thành phố tiếp theo, thoát vòng lặp!" << endl;
                break;
            }

            vec.push_back(nearest_city);
            visited.insert(nearest_city);
        }

        if (vec.size() == base) {
            T.insert(vec);
            cout << "[DEBUG] Thêm đường đi mới, tổng số đường đi: " << T.size() << endl;
        }
    }

    for (const auto& x : T) {
        Member.emplace_back(Way(x, expense));
    }
    cout << "[DEBUG] Số cá thể từ Nearest Neighbor: " << Member.size() << endl;
}

// Lớp Population (Quần thể)
class Population {
public:
    int n;
    int P;
    vector<Way> Member;
    vector<vector<int>>& Expense;

    Population(int _n, vector<vector<int>>& expense)
        : n(_n), Expense(expense) {

        if (n <= 10) P = getRandomNumber(10, 50);
        else if (n <= 20) P = getRandomNumber(50, 100);
        else if (n <= 50) P = getRandomNumber(100, 300);
        else if (n <= 100) P = getRandomNumber(300, 1000);
        else P = getRandomNumber(1000, 5000);

        if (P == 0) P = 10; // Đảm bảo P > 0

        cout << "[DEBUG] Số lượng cá thể P được tạo: " << P << endl;


        int n_nearest_neighbor = min(P/2,n);
        int n_random_shuffle = P - n_nearest_neighbor;

        insert_nearest_neighbor(Member, n, n_nearest_neighbor, Expense);
        cout << "[DEBUG] Sau insert_nearest_neighbor, Member.size() = " << Member.size() << endl;

        insert_Random_shuffle(Member, n, n_random_shuffle, Expense);
        cout << "[DEBUG] Sau insert_Random_shuffle, Member.size() = " << Member.size() << endl;
    }
};

// Hàm main
int main() {
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

    Population pop(10, expense);
    cout << "[DEBUG] Số lượng cá thể trong quần thể: " << pop.Member.size() << endl;

    for (const auto& way : pop.Member) {
        cout << "Đường đi: ";
        for (int city : way.Permution) cout << city << " ";
        cout << "| Chi phí: " << way.cost << endl;
    }

    return 0;
}
