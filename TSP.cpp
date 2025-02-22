#include <bits/stdc++.h>
using namespace std;

// Khai báo các biến khởi tạo giá trị ngẫu nhiên toàn cục
random_device rd;
mt19937 gen(rd());

// Hàm tạo số ngẫu nhiên trong khoảng [a, b]
int getRandomNumber(int a, int b) {
    uniform_int_distribution<int> dist(a, b);
    return dist(gen);
}

double getRandomDouble(double a, double b) {
    uniform_real_distribution<double> dist(a, b);
    return dist(gen);
}

// Lớp Way đại diện cho một hành trình
class Way {
public:
    vector<int> Permution;
    vector<vector<int>>& Expense;
    int cost;
    double fitness;

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

        fitness = 1.0/cost;
    }

    // Copy constructor
    Way(const Way& other)
        : Permution(other.Permution), Expense(other.Expense), cost(other.cost), fitness(other.fitness) {}

    // Copy assignment operator
    Way& operator=(const Way& other) {
        if (this == &other) return *this;
        Permution = other.Permution;
        cost = other.cost;
        fitness = other.fitness;
        return *this;
    }

    void resetCost(){
        for (size_t i = 1; i < Permution.size(); i++) {
            cost += Expense[Permution[i - 1]][Permution[i]];
        }
        cost += Expense[Permution.back()][Permution.front()];
        fitness = 1.0/cost;
    }

    bool operator<(const Way& other) const {
        return fitness < other.fitness;
    }
};

// Hàm tạo cá thể bằng phương pháp Random Shuffle
void insert_Random_shuffle(vector<Way>& Member, int base, int P, vector<vector<int>>& expense) {
    vector<int> vec(base);
    iota(vec.begin(), vec.end(), 0);

    set<vector<int>> S;

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
    int n; // kích thước cá thể
    int P; // số lượng cá thể
    vector<Way> Member;
    vector<vector<int>>& Expense;
    double sumfitness = 0;

    Population(int _n, vector<vector<int>>& expense)
        : n(_n), Expense(expense) {

        if (n <= 10) P = getRandomNumber(10, 50);
        else if (n <= 20) P = getRandomNumber(50, 100);
        else if (n <= 50) P = getRandomNumber(100, 300);
        else if (n <= 100) P = getRandomNumber(300, 1000);
        else P = getRandomNumber(1000, 5000);

        if (P == 0) P = 10; // Đảm bảo P > 0

        cout << "[DEBUG] Số lượng cá thể P được tạo: " << P << endl;

        int n_nearest_neighbor = min(P / 2, n);
        int n_random_shuffle = P - n_nearest_neighbor;

        insert_nearest_neighbor(Member, n, n_nearest_neighbor, Expense);
        cout << "[DEBUG] Sau insert_nearest_neighbor, Member.size() = " << Member.size() << endl;

        insert_Random_shuffle(Member, n, n_random_shuffle, Expense);
        cout << "[DEBUG] Sau insert_Random_shuffle, Member.size() = " << Member.size() << endl;

        for (Way x : Member) sumfitness += x.fitness;

        sort(Member.begin(), Member.end());
    }

    Way RLselection() { // Roulette selection
        double select = getRandomDouble(0, 1);
        double fitness_search = select * sumfitness;

        auto it = lower_bound(Member.begin(), Member.end(), fitness_search,
            [](const Way& a, double val) { return a.fitness < val; });

        return (it != Member.end()) ? *it : Member.back();
    }

    Way TNselection(int k = 3) { // tournament selection mặc định k = 3
        Way best = Member[getRandomNumber(0, P - 1)]; // Chọn cá thể đầu tiên ngẫu nhiên

        for (int i = 1; i < k; i++) {
            Way candidate = Member[getRandomNumber(0, P - 1)];
            if (candidate.fitness > best.fitness) {
                best = candidate;
            }
        }

        return best;
    }

    Way RankSelection() {
        int N = Member.size();

        // Tính tổng rank: S = N * (N + 1) / 2
        int total_rank = N * (N + 1) / 2;

        // Chọn một giá trị ngẫu nhiên trong khoảng [1, total_rank]
        int random_value = getRandomNumber(1, total_rank);

        int cumulative_rank = 0;

        for (int i = 0; i < N; i++) { // Xét từ cá thể yếu nhất đến mạnh nhất
            cumulative_rank += (i + 1); // Rank của cá thể i là (i + 1)
            if (cumulative_rank >= random_value) {
                return Member[i]; // Chọn cá thể phù hợp
            }
        }

        return Member.back(); // Dự phòng, chọn cá thể mạnh nhất nếu có lỗi
    }

    Way RandomSelection() {
        int idx = getRandomNumber(0, P - 1); // Chọn ngẫu nhiên một chỉ số trong quần thể
        return Member[idx]; // Trả về cá thể được chọn
    }

    Way OXcrossover(Way& parent1, Way& parent2) {
        vector<int> child(n, -1);

        int start = getRandomNumber(0, n - 2);
        int end = getRandomNumber(start + 1, n - 1);

        unordered_set<int> used;
        for (int i = start; i <= end; i++) {
            child[i] = parent1.Permution[i];
            used.insert(child[i]);
        }

        int j = 0;
        for (int i = 0; i < n; i++) {
            if (child[i] == -1) {
                while (used.count(parent2.Permution[j])) j++;
                child[i] = parent2.Permution[j++];
            }
        }
        return Way(child, parent1.Expense);
    }

    Way PMXcrossover(Way& parent1, Way& parent2) {
        int n = parent1.Permution.size();
        int start = getRandomNumber(0, n - 2);
        int end = getRandomNumber(start + 1, n - 1);

        vector<int> child(n, -1);
        unordered_map<int, int> mapping;
        unordered_set<int> used;


        for (int i = start; i <= end; i++) {
            child[i] = parent1.Permution[i];
            mapping[parent1.Permution[i]] = parent2.Permution[i];
            used.insert(parent1.Permution[i]);
        }


        for (int i = 0; i < n; i++) {
            if (i >= start && i <= end) continue;

            int val = parent2.Permution[i];
            while (mapping.find(val) != mapping.end()) {
                val = mapping[val];
            }

            if (used.find(val) == used.end()) {
                child[i] = val;
                used.insert(val);
            }
        }

        return Way(child, parent1.Expense);
    }

        void mutate(Way& way, double mutation_rate = 0.4) {// đột biến với xác suất 0.4
        if (getRandomDouble(0, 1) < mutation_rate) {
            int i = getRandomNumber(0, n - 1);
            int j;
            do {
                j = getRandomNumber(0, n - 1);
            } while (i == j); // Đảm bảo i và j không trùng nhau

            swap(way.Permution[i], way.Permution[j]);

        }
    }

    Way CXcrossover(Way& parent1, Way& parent2) {

        vector<int> child(n, -1);
        vector<bool> visited(n, false);

        int index = 0;
        while (!visited[index]) {
            visited[index] = true;
            child[index] = parent1.Permution[index];
            index = find(parent1.Permution.begin(), parent1.Permution.end(), parent2.Permution[index]) - parent1.Permution.begin();
        }

        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                child[i] = parent2.Permution[i];
            }
        }

        return Way(child, parent1.Expense);
    }

    void evolveNextGeneration(string SelectionMethod, string CrossoverMethod) {
        vector<Way> newGeneration;
        while (newGeneration.size() < P) {
            Way parent1 = RLselection();
            Way parent2 = RLselection();
            if (SelectionMethod == "TNselection") {
                parent1 = TNselection();
                parent2 = TNselection();
            }
            if (SelectionMethod == "Rankselection") {
                parent1 = RankSelection();
                parent2 = RankSelection();
            }
            if (SelectionMethod == "Randomselection") {
                parent1 = RandomSelection();
                parent2 = RandomSelection();
            }

            Way child1 = OXcrossover(parent1, parent2);
            Way child2 = OXcrossover(parent2, parent1);

            if(CrossoverMethod=="PMXcrossover"){
                child1 = PMXcrossover(parent1,parent2);
                child2 = PMXcrossover(parent2,parent1);
            }

            if(CrossoverMethod=="CXcrossover"){
                child1 = CXcrossover(parent1,parent2);
                child2 = CXcrossover(parent2,parent1);
            }

            mutate(child1);
            mutate(child2);

            newGeneration.push_back(child1);
            if (newGeneration.size() < P) newGeneration.push_back(child2);
        }

        Member = newGeneration;
        sumfitness = 0;
        for (Way x : Member) sumfitness += x.fitness;
        sort(Member.begin(), Member.end());
    }

    Way getBestSolution() {
        return Member.back();
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
    int generations = 0, stagnation = 0, max_stagnation = 50; // sau 50 thế hệ không cải thiện thì dừng thuật toán
    int best_cost = pop.getBestSolution().cost;

    while (stagnation < max_stagnation) {
        pop.evolveNextGeneration("TNselection","PMXcrossover");
        generations++;
        if (pop.getBestSolution().cost < best_cost) {
            best_cost = pop.getBestSolution().cost;
            stagnation = 0;
        } else {
            stagnation++;
        }
    }

    cout << "Tìm thấy giải pháp tốt nhất sau " << generations << " thế hệ với chi phí: " << best_cost << endl;
    return 0;
}
