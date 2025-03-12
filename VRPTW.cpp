#include <bits/stdc++.h>
using namespace std;

random_device rd;
mt19937 gen(rd());

int getRandomNumber(int a, int b)
{
    uniform_int_distribution<int> dist(a, b);
    return dist(gen);
}

double getRandomDouble(double a, double b)
{
    uniform_real_distribution<double> dist(a, b);
    return dist(gen);
}

bool checkValid(vector<int> x, int v, vector<vector<double>> &Expensive, vector<pair<double, double>> &Timewindow, vector<double> &Weight, vector<double> &TimeServe, double Q)
{
    if (v == 0)
        return true;
    x.push_back(v);
    double b = 0; // thời gian bắt đầu hành trình
    for (int i = 0; i < x.size() - 1; i++)
    { // Sửa lỗi truy cập ngoài phạm vi
        int from = x[i], to = x[i + 1];
        double travel_time = Expensive[from][to];
        if (i == 0)
        {
            // Xe đến điểm đầu tiên ít nhất là tại cửa sổ thời gian mở hoặc thời gian di chuyển
            b = max(Timewindow[to].first, travel_time);
        }
        else
        {
            // Xe đến sớm -> phải đợi, nếu trễ thì đi thẳng luôn
            b = max(b + travel_time, Timewindow[to].first);
        }
        // Kiểm tra nếu xe đến sau cửa sổ thời gian đóng thì tuyến không hợp lệ
        if (b > Timewindow[to].second)
            return false;
        b += TimeServe[to];
    }
    double w = 0;
    for (int i = 1; i < x.size(); i++)
    {
        w += Weight[x[i]];
    }
    if (w > Q)
        return false;
    return true;
}

// Hàm trích xuất danh sách các cung từ một lời giải VRPTW
set<pair<int, int>> extractArcs(const vector<vector<int>> &solution)
{
    set<pair<int, int>> arcs;
    for (const auto &route : solution)
    {
        for (size_t i = 0; i < route.size() - 1; ++i)
        {
            arcs.insert({route[i], route[i + 1]});
        }
    }
    return arcs;
}

// Hàm tính hệ số Jaccard giữa hai lời giải
double jaccardSimilarity(const vector<vector<int>> &solutionA, const vector<vector<int>> &solutionB)
{
    set<pair<int, int>> arcsA = extractArcs(solutionA);
    set<pair<int, int>> arcsB = extractArcs(solutionB);

    // Tính giao và hợp của hai tập cung
    set<pair<int, int>> intersectionSet, unionSet;

    for (const auto &arc : arcsA)
    {
        if (arcsB.find(arc) != arcsB.end())
        {
            intersectionSet.insert(arc);
        }
        unionSet.insert(arc);
    }

    for (const auto &arc : arcsB)
    {
        unionSet.insert(arc);
    }

    // Tính hệ số Jaccard
    double intersectionSize = intersectionSet.size();
    double unionSize = unionSet.size();

    return (unionSize > 0) ? (intersectionSize / unionSize) : 0.0;
}
class Route
{
public:
    vector<vector<int>> u;                    // lưu trữ tuyến đường
    int n;                                    // số điểm cần phục vụ
    double Q;                                 // tải trọng tối đa của xe
    int f1;                                   // số xe
    vector<vector<double>> &Expensive;        // ma trận khoảng cách trong bài này ta coi chi phí chính là thời gian di chuyển giữa các điểm
    vector<double> &Weight;                   // khối lượng hàng phục vụ tại vị trí i
    vector<double> &TimeServe;                // thời gian phục vụ
    vector<pair<double, double>> &Timewindow; // cửa số thời gian tường khách hàng
    double f2;                                // tổng khoảng cách di chuyển
    double f3;                                // tổng thơi gian di chuyển giả thiết khách hàng đầu tiên được phục vụ ngay tại thời điểm bắt đầu cửa sổ thời gian
    bool isValid;                             // Kiểm tra tuyến có hợp lệ không

    Route(vector<vector<int>> U, int N, vector<vector<double>> &expensive, vector<pair<double, double>> &timewindow, vector<double> &weight, vector<double> &timeServe, double q) : u(U), n(N), Q(q), Expensive(expensive), Weight(weight), TimeServe(timeServe), Timewindow(timewindow), isValid(true)
    {
        f1 = u.size();
        f2 = 0;
        for (vector<int> x : u)
        {
            for (int i = 0; i < x.size() - 1; i++)
            {
                f2 += Expensive[x[i]][x[i + 1]];
            }
        }
        f3 = 0;
        double b = 0; // thời gian bắt đầu hành trình
        for (vector<int> x : u)
        {
            if (x.empty())
                continue;

            for (size_t i = 0; i < x.size() - 1; i++)
            {
                int from = x[i], to = x[i + 1];
                double travel_time = Expensive[from][to];

                if (i == 0)
                {
                    // Xe đến điểm đầu tiên ít nhất là tại cửa sổ thời gian mở hoặc thời gian di chuyển
                    b = max(Timewindow[to].first, travel_time);
                }
                else
                {
                    // Xe đến sớm -> phải đợi, nếu trễ thì đi thẳng luôn
                    b = max(b + travel_time, Timewindow[to].first);
                }
                // Cập nhật tổng thời gian di chuyển
                f3 += travel_time;
                b += TimeServe[to];
            }
        }

        for (vector<int> x : u)
        {
            if (!checkValid(x, 0, Expensive, Timewindow, Weight, TimeServe, Q))
            {
                f1 = f2 = f3 = -1;
                isValid = false;
            }
        }
    }

    Route(const Route &other) : u(other.u), f1(other.f1), Expensive(other.Expensive), Weight(other.Weight), TimeServe(other.TimeServe), Timewindow(other.Timewindow), f2(other.f2), f3(other.f3), isValid(other.isValid) {}

    Route &operator=(const Route &other)
    {
        if (this == &other)
            return *this;
        u = other.u;
        f1 = other.f1;
        f2 = other.f2;
        f3 = other.f3;
        isValid = other.isValid;
        return *this;
    }

    bool operator<(const Route &other) const
    {
        return ((f1 <= other.f1) && (f2 <= other.f2) && (f3 <= other.f3) && (f1 < other.f1 || f2 < other.f2 || f3 < other.f3)); // x<y tức là x thống trị y
    }

    void print()
    {
        if (!isValid)
        {
            cout << "Tuyến đường không hợp lệ!\n";
            return;
        }
        cout << "Số xe: " << f1 << "\n";
        cout << "Tổng quãng đường: " << f2 << "\n";
        cout << "Tổng thời gian di chuyển: " << f3 << "\n";
        cout << "Các tuyến: " << endl;
        for (vector<int> x : u)
        {
            for (int v : x)
            {
                cout << v << " ";
            }
            cout << endl;
        }
    }
};

template <typename T1, typename T2>
class NSGA_II
{
public:
    T1 &population;

    NSGA_II(T1 &_population) : population(_population) {}

    void NSGA_II_Genetic(double mutation_rate, int stagnation)
    {

        cout << " start genetic";
        for (int i = 0; i < stagnation; i++)
        {
            cout << i << " ";
            vector<T2> Child;

            while (Child.size() < population.size)
            {
                T2 parent1 = population.TNselection();
                T2 parent2 = population.TNselection();

                T2 child1 = population.crossover(parent1, parent2);
                T2 child2 = population.crossover(parent2, parent1);

                child1 = population.mutate(child1, mutation_rate);
                child2 = population.mutate(child2, mutation_rate);

                Child.push_back(child1);
                Child.push_back(child2);
            }

            for (auto &x : Child)
            {
                population.Member.push_back(x);
            }

            population.sort_by_domination_crowdingdistance();
            population.create_next_member();
        }
    }
};

template <typename T1, typename T2>
class MOEA
{
public:
    T1 population;
    MOEA(T1 _population) : population(_population) {}
    void MOEA_Genetic(double mutation_rate, int stagnation)
    {
        for (int i = 0; i < stagnation; i++)
        {
            vector<T2> Child;

            while (Child.size() < population.size)
            {
                T2 parent1 = population.TNselection();
                T2 parent2 = population.SelectByParent1(parent1);

                T2 child1 = population.crossover(parent1, parent2);
                T2 child2 = population.crossover(parent2, parent1);

                child1 = population.mutate(child1, mutation_rate);
                child2 = population.mutate(child2, mutation_rate);

                Child.push_back(child1);
                Child.push_back(child2);
            }

            for (auto &x : Child)
            {
                population.Member.push_back(x);
            }

            population.sort_by_domination_crowdingdistance();
            population.create_next_member();
        }
    }
};

class Population
{
public:
    int size;                                            // kích thước quần thể
    int n;                                               // số điểm cần phục vụ
    int Q;                                               // tải trọng tối đa của xe
    vector<vector<double>> &Expensive;                   // ma trận khoảng cách trong bài này ta coi chi phí chính là thời gian di chuyển giữa các điểm
    vector<double> &Weight;                              // khối lượng hàng phục vụ tại vị trí i
    vector<double> &TimeServe;                           // thời gian phục vụ
    vector<pair<double, double>> &Timewindow;            // cửa số thời gian tường khách hàng
    
    vector<Route> Member;                                // thành viên quần thể

    Population(int _size, int N, int q, vector<vector<double>> &expensive, vector<double> &weight, vector<double> &timeserve, vector<pair<double, double>> &timewindow) : size(_size), n(N), Q(q), Expensive(expensive), Weight(weight), TimeServe(timeserve), Timewindow(timewindow)
    {
        // khởi tạo quần thể
        set<Route> inserted;
        int n_nearest_neighbor = int(size/10);
        int n_random_suffle = size - n_nearest_neighbor;

        cout << "Tạo cá thể bởi RD: " << n_random_suffle << endl;

        while (inserted.size() < n_random_suffle)
        {
            vector<vector<int>> u;
            unordered_set<int> S;
            vector<int> deleted;
            for (int j = 1; j <= n; j++)
            {
                S.insert(j);
            }
            while (!S.empty())
            {
                vector<int> x;
                x.push_back(0);

                int index = getRandomNumber(0, S.size() - 1);

                auto it = S.begin();
                it = next(it, index);
                x.push_back(*it);

                S.erase(it);

                for (int v : S)
                {
                    if (checkValid(x, v, Expensive, Timewindow, Weight, TimeServe, Q))
                    {
                        x.push_back(v);
                        deleted.push_back(v);
                    }
                }
                for (int v : deleted)
                {
                    S.erase(v);
                }
                x.push_back(0);
                u.push_back(x);
            }

            Route r(u, n, Expensive, Timewindow, Weight, TimeServe, Q);

            if (r.isValid)
            {
                inserted.insert(r);
            }
        }
        while (inserted.size() < size)
        {
            vector<vector<int>> u;
            vector<int> S;
            for (int j = 1; j <= n; j++)
            {
                S.push_back(j);
            }
            while (!S.empty())
            {
                vector<int> x;
                vector<int> deleted;
                x.push_back(0);
                int index = getRandomNumber(0, S.size() - 1);
                x.push_back(S[index]);
                S.erase(S.begin() + index);
                sort(S.begin(), S.end(), [&](int a, int b)
                     { return Expensive[x.back()][a] < Expensive[x.back()][b]; });
                for (int v : S)
                {
                    if (checkValid(x, v, Expensive, Timewindow, Weight, TimeServe, Q))
                    {
                        x.push_back(v);
                        deleted.push_back(v);
                    }
                }
                for (int v : deleted)
                {
                    S.erase(find(S.begin(), S.end(), v));
                }
                x.push_back(0);
                u.push_back(x);
            }
            Route r(u, n, Expensive, Timewindow, Weight, TimeServe, Q);
            if (r.isValid)
            {
                inserted.insert(r);
            }
        }
        for (Route r : inserted)
        {
            Member.push_back(r);
        }
        Member[0].print();
    }

    void sort_by_domination_crowdingdistance()
    {
        vector<vector<int>> F(1); // F[0] chứa tập Pareto đầu tiên
        vector<int> n(Member.size(), 0);
        vector<vector<int>> S(Member.size());
        map<Route, double> crowding_distance;

        // Xác định tập Pareto front
        for (int i = 0; i < Member.size(); i++)
        {
            for (int j = 0; j < Member.size(); j++)
            {
                if (Member[i] < Member[j]) // i thống trị j
                {
                    S[i].push_back(j);
                }
                else if (Member[j] < Member[i]) // j thống trị i
                {
                    n[i]++;
                }
            }
            if (n[i] == 0) // Nếu không ai thống trị i, thêm vào F[0]
            {
                F[0].push_back(i);
            }
        }

        // Xây dựng các Pareto front tiếp theo
        int i = 0;
        while (i < F.size() && !F[i].empty())
        {
            vector<int> H;
            for (int x : F[i])
            {
                for (int y : S[x])
                {
                    n[y]--;
                    if (n[y] == 0)
                    {
                        H.push_back(y);
                    }
                }
            }
            if (!H.empty())
                F.push_back(H);
            i++;
        }

        // Xóa phần tử trống cuối (nếu có)
        if (F.back().empty())
            F.pop_back();

        // Xác định rank của từng cá thể
        map<Route, int> rank;
        for (int i = 0; i < F.size(); i++)
        {
            for (int x : F[i])
            {
                rank[Member[x]] = i;
            }
        }

        // Tính crowding distance cho từng Pareto front
        for (auto &front : F)
        {
            if (front.size() < 2)
                continue; // Nếu có 1 phần tử, không tính crowding distance

            // Khởi tạo khoảng cách chen chúc
            for (int x : front)
                crowding_distance[Member[x]] = 0;

            // Tính crowding distance trên từng mục tiêu f1, f2, f3
            auto compute_crowding_distance = [&](auto cmp, auto accessor)
            {
                sort(front.begin(), front.end(), [&](int a, int b)
                     { return accessor(Member[a]) < accessor(Member[b]); });

                crowding_distance[Member[front[0]]] = numeric_limits<double>::infinity();
                crowding_distance[Member[front.back()]] = numeric_limits<double>::infinity();

                double minVal = accessor(Member[front[0]]);
                double maxVal = accessor(Member[front.back()]);

                if (maxVal == minVal) // Tránh chia cho 0
                    return;

                for (int i = 1; i < front.size() - 1; i++)
                {
                    crowding_distance[Member[front[i]]] += (accessor(Member[front[i + 1]]) - accessor(Member[front[i - 1]])) / (maxVal - minVal);
                }
            };

            compute_crowding_distance([](const Route &a, const Route &b)
                                      { return a.f1 < b.f1; },
                                      [](const Route &r)
                                      { return r.f1; });

            compute_crowding_distance([](const Route &a, const Route &b)
                                      { return a.f2 < b.f2; },
                                      [](const Route &r)
                                      { return r.f2; });

            compute_crowding_distance([](const Route &a, const Route &b)
                                      { return a.f3 < b.f3; },
                                      [](const Route &r)
                                      { return r.f3; });
        }

        // Sắp xếp `Member` theo rank trước, crowding distance sau
        sort(Member.begin(), Member.end(), [&](const Route &a, const Route &b)
             {
                 if (rank[a] != rank[b])
                     return rank[a] < rank[b];                       // Ưu tiên rank thấp hơn
                 return crowding_distance[a] > crowding_distance[b]; // Nếu cùng rank, ưu tiên crowding distance lớn hơn
             });
    }

    Route TNselection(int k = 3)
    {
        vector<Route> selected;
        while (selected.size() < k)
        {
            int index = getRandomNumber(0, Member.size() - 1);
            selected.push_back(Member[index]);
        }
        sort(selected.begin(), selected.end(), [&](Route a, Route b)
             { return a < b; });
        return selected[0];
    }

    Route crossover(Route parent1, Route parent2)
    {
        vector<vector<int>> u;
        set<int> S;
        for (int i = 1; i <= n; i++)
        {
            S.insert(i);
        }

        int k = getRandomNumber(1, parent1.u.size() - 2); // Số hành trình sẽ lấy từ cha 1
        set<int> chosen;
        while (chosen.size() < k)
        {
            int index = getRandomNumber(0, parent1.u.size() - 1);
            if (chosen.find(index) == chosen.end())
            {
                chosen.insert(index);
                u.push_back(parent1.u[index]);
                for (int v : parent1.u[index])
                {
                    S.erase(v);
                }
            }
        }

        for (vector<int> x : parent2.u)
        {
            vector<int> y;
            bool flag = true;
            for (int v : x)
            {
                if (S.find(v) == S.end())
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                u.push_back(x);
                for (int v : x)
                {
                    S.erase(v);
                }
            }
        }

        for (auto it = S.begin(); it != S.end();)
        {
            bool inserted = false;
            for (vector<int> &x : u)
            {
                if (x.size() > 1)
                    x.pop_back();

                vector<int> temp_x = x;
                temp_x.push_back(*it);
                temp_x.push_back(0);

                if (checkValid(temp_x, *it, Expensive, Timewindow, Weight, TimeServe, Q))
                {
                    x.push_back(*it);
                    x.push_back(0);
                    it = S.erase(it); // Xóa phần tử an toàn
                    inserted = true;
                    break;
                }
                x.push_back(0);
            }
            if (!inserted)
                ++it;
        }

        while (!S.empty())
        {
            vector<int> x;
            x.push_back(0);
            if (!S.empty())
            {
                int index = getRandomNumber(0, S.size() - 1);
                auto it = S.begin();
                advance(it, index);
                x.push_back(*it);
                S.erase(it);
            }
            for (auto it = S.begin(); it != S.end();)
            {
                vector<int> temp_x = x;

                if (checkValid(temp_x, *it, Expensive, Timewindow, Weight, TimeServe, Q))
                {
                    x.push_back(*it);
                    it = S.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            x.push_back(0);
            u.push_back(x);
        }

        return Route(u, n, Expensive, Timewindow, Weight, TimeServe, Q);
    }
    Route mutate(Route parent, double mutation_rate)
    {
        vector<vector<int>> u = parent.u;
        if (getRandomDouble(0, 1) < mutation_rate)
        {
            bool mutated = false;
            while (!mutated)
            {
                int choose = getRandomNumber(0, 2);
                if (choose == 0) // Đảo đoạn tuyến
                {
                    int index = getRandomNumber(0, u.size() - 1);
                    if (u[index].size() > 3) // Đảm bảo có đủ phần tử
                    {
                        int l = getRandomNumber(1, u[index].size() - 3);
                        int r = getRandomNumber(l + 1, u[index].size() - 2);
                        vector<int> tempRoute = u[index];
                        reverse(tempRoute.begin() + l, tempRoute.begin() + r + 1);

                        // Kiểm tra xem tuyến sau khi đảo có bị trùng không
                        bool isDuplicate = false;
                        for (const auto &route : u)
                        {
                            if (route == tempRoute)
                            {
                                isDuplicate = true;
                                break;
                            }
                        }

                        if (checkValid(tempRoute, 0, Expensive, Timewindow, Weight, TimeServe, Q) && !isDuplicate)
                        {
                            u[index] = tempRoute;
                            mutated = true;
                            cout << "mutated đảo đoạn tuyến" << endl;
                        }
                    }
                }
                else if (choose == 1) // Thêm điểm vào tuyến này, xóa ở tuyến khác
                {
                    int index1 = getRandomNumber(0, u.size() - 1);
                    int index2 = getRandomNumber(0, u.size() - 1);
                    if (index1 != index2 && u[index2].size() > 2)
                    {
                        int v = getRandomNumber(1, u[index2].size() - 2);
                        vector<int> temp1 = u[index1];
                        int insertPos = getRandomNumber(1, temp1.size() - 1);
                        temp1.insert(temp1.begin() + insertPos, u[index2][v]);
                        vector<int> temp2 = u[index2];
                        temp2.erase(temp2.begin() + v);

                        if (checkValid(temp1, 0, Expensive, Timewindow, Weight, TimeServe, Q) &&
                            checkValid(temp2, 0, Expensive, Timewindow, Weight, TimeServe, Q) &&
                            temp1 != u[index1] && temp2 != u[index2])
                        {
                            u[index1] = temp1;
                            u[index2] = temp2;
                            mutated = true;
                            cout << "mutated thêm tuyến" << endl;
                        }
                    }
                }
                else // Ghép tuyến
                {
                    int index1 = getRandomNumber(0, u.size() - 1);
                    int index2 = getRandomNumber(0, u.size() - 1);
                    if (index1 != index2)
                    {
                        vector<int> mergedRoute = {0};
                        mergedRoute.insert(mergedRoute.end(), u[index1].begin() + 1, u[index1].end() - 1);
                        mergedRoute.insert(mergedRoute.end(), u[index2].begin() + 1, u[index2].end() - 1);
                        mergedRoute.push_back(0);

                        if (checkValid(mergedRoute, 0, Expensive, Timewindow, Weight, TimeServe, Q))
                        {
                            vector<vector<int>> newRoutes;
                            newRoutes.push_back(mergedRoute);

                            for (int i = 0; i < u.size(); i++)
                            {
                                if (i != index1 && i != index2)
                                {
                                    newRoutes.push_back(u[i]);
                                }
                            }

                            u = newRoutes;
                            mutated = true;
                            cout << "mutated ghép tuyến" << endl;
                        }
                    }
                }
            }
        }
        Route r(u, n, Expensive, Timewindow, Weight, TimeServe, Q);
        r.print();
        return r;
    }


    Route SelectByParent1(Route parent1)
    {
        double jaccard = 1;
        int last = -1;
        for (int i = 0; i < Member.size(); i++)
        {
            if (jaccardSimilarity(parent1.u, Member[i].u) < jaccard)
            {
                jaccard = jaccardSimilarity(parent1.u, Member[i].u);
                last = i;
            }
        }
        return Member[last];
    }

    void create_next_member()
    {
        vector<Route> next_member;
        for (int i = 0; i < size; i++)
        {
            next_member.push_back(Member[i]);
        }
        Member = next_member;
    }
    

    void Genetic(string Method = "NSGA_II",double mutation_rate = 0.4, int stagnation = 100)
    {
        cout << " Start genetic";
        // for (int i = 0; i < stagnation; i++)
        // {
        //     cout << "i " << i << endl;
        //     vector<Route> Child;

        //     while (Child.size() < size)
        //     {
        //         Route parent1 = TNselection();
        //         Route parent2 = TNselection();

        //         Route child1 = crossover(parent1, parent2);
        //         Route child2 = crossover(parent2, parent1);

                

        //         child1 = mutate(child1, mutation_rate);
        //         child2 = mutate(child2, mutation_rate);

        //         child1.print();

        //         Child.push_back(child1);
        //         Child.push_back(child2);
        //     }

        //     for (auto &x : Child)
        //     {
        //         Member.push_back(x);
        //     }

        //     cout << "sort_by_domination_crowdingdistance" << endl;

        //     sort_by_domination_crowdingdistance();

        //     cout << "create_next_member" << endl;
        //     create_next_member();
        // }
        if(Method == "NSGA_II")
        {
        NSGA_II<Population, Route> nsga_ii(*this);

        nsga_ii.NSGA_II_Genetic(mutation_rate, stagnation);
        }
        else
        {
            MOEA<Population, Route> moea(*this);
            moea.MOEA_Genetic(mutation_rate, stagnation);
        }
    };
};

int main()
{
    string path_in = "D:\\Tinh toan tien hoa\\data\\solomon_dataset\\C1\\C101.txt";
    string path_out = "D:\\Tinh toan tien hoa\\data\\solomon_dataset\\C1\\C101_out.txt";
    freopen(path_in.c_str(), "r", stdin);
    int n, Q;
    cin >> n >> Q;
    vector<vector<double>> Expensive(n + 1, vector<double>(n + 1));
    vector<double> Weight(n + 1);
    vector<double> TimeServe(n + 1);
    vector<pair<double, double>> Timewindow(n + 1);
    for (int i = 0; i <= n; i++)
    {
        for (int j = 0; j <= n; j++)
            cin >> Expensive[i][j];
    }

    for (int i = 0; i <= n; i++)
        cin >> Timewindow[i].first >> Timewindow[i].second;
    for (int i = 0; i <= n; i++)
        cin >> Weight[i];
    for (int i = 0; i <= n; i++)
        cin >> TimeServe[i];

    Population population(20, n, Q, Expensive, Weight, TimeServe, Timewindow);

    population.Genetic("NSGA_II",0.5, 500);
    cout << "Kết quả cuối cùng: \n";
    population.Member[0].print();
    population.Member[2].print();
    return 0;
}