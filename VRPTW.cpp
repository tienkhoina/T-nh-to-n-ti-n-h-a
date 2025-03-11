#include<bits/stdc++.h>
using namespace std;

random_device rd;
mt19937 gen(rd());

int getRandomNumber(int a, int b) {
    uniform_int_distribution<int> dist(a, b);
    return dist(gen);
}

double getRandomDouble(double a, double b) {
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
class Route{
public:
    vector<vector<int>> u; // lưu trữ tuyến đường
    int n; // số điểm cần phục vụ
    double Q; //tải trọng tối đa của xe
    int f1; // số xe
    vector<vector<double>>& Expensive; // ma trận khoảng cách trong bài này ta coi chi phí chính là thời gian di chuyển giữa các điểm
    vector<double>& Weight; // khối lượng hàng phục vụ tại vị trí i
    vector<double>& TimeServe; // thời gian phục vụ
    vector<pair<double,double>>& Timewindow; // cửa số thời gian tường khách hàng
    double f2; // tổng khoảng cách di chuyển
    double f3; // tổng thơi gian di chuyển giả thiết khách hàng đầu tiên được phục vụ ngay tại thời điểm bắt đầu cửa sổ thời gian
    bool isValid; // Kiểm tra tuyến có hợp lệ không


    Route(vector<vector<int>> U,int N, vector<vector<double>>& expensive,vector<pair<double,double>>& timewindow, vector<double>& weight,  vector<double>& timeServe, double q):
        u(U),n(N),Q(q),Expensive(expensive), Weight(weight),TimeServe(timeServe),Timewindow(timewindow),isValid(true){
            f1= u.size();
            f2=0;
            for(vector<int> x : u){
                for(int i=0; i<x.size()-1;i++){
                    f2+= Expensive[x[i]][x[i+1]];
                }
            }
            f3=0;
            double b = 0; // thời gian bắt đầu hành trình
            for (vector<int> x : u) {
                if (x.empty()) continue;

                for (size_t i = 0; i < x.size() - 1; i++) {
                    int from = x[i], to = x[i + 1];
                    double travel_time = Expensive[from][to];

                    if (i == 0) {
                        // Xe đến điểm đầu tiên ít nhất là tại cửa sổ thời gian mở hoặc thời gian di chuyển
                        b = max(Timewindow[to].first, travel_time);
                    } else {
                        // Xe đến sớm -> phải đợi, nếu trễ thì đi thẳng luôn
                        b = max(b + travel_time, Timewindow[to].first);
                    }


                    // Kiểm tra nếu xe đến sau cửa sổ thời gian đóng thì tuyến không hợp lệ
                    if (b > Timewindow[to].second) {
                        isValid = false;
                        f1 = f2 = f3 = -1; // Đánh dấu tuyến không hợp lệ
                        return; // Dừng ngay lập tức
                    }

                    // Cập nhật tổng thời gian di chuyển
                    f3 += travel_time;
                    b += TimeServe[to];
                }
            }

            for(vector<int> x : u){
                double w =0;
                for(int i =1 ;i<x.size()-1; i++){
                    w+=Weight[x[i]];
                }
                if(w> Q){
                    isValid = false;
                    f1 = f2 = f3 = -1; // Đánh dấu tuyến không hợp lệ
                    return; // Dừng ngay lập tức
                }
            }
        }


    Route(const Route& other):
         u(other.u),f1(other.f1),Expensive(other.Expensive),Weight(other.Weight), TimeServe(other.TimeServe),Timewindow(other.Timewindow),f2(other.f2),f3(other.f3),isValid(other.isValid){}


    Route& operator=(const Route& other){
        if(this == &other) return *this;
        u = other.u;
        f1 = other.f1;
        f2 = other.f2;
        f3 = other.f3;
        isValid = other.isValid;
        return *this;
    }


    bool operator <(const Route& other) const{
        return((f1<=other.f1)&&(f2<=other.f2)&&(f3<=other.f3)&&(f1<other.f1||f2<other.f2||f3<other.f3)); // x<y tức là x thống trị y
    }

    void print() {
        if (!isValid) {
            cout << "Tuyến đường không hợp lệ!\n";
            return;
        }
        cout << "Số xe: " << f1 << "\n";
        cout << "Tổng quãng đường: " << f2 << "\n";
        cout << "Tổng thời gian di chuyển: " << f3 << "\n";
        cout << "Các tuyến: " << endl;
        for(vector<int> x : u){
            for(int v : x){
                cout << v << " ";
            }
            cout << endl;
        }
    }

};


template<typename T1, typename T2>
class NSGA_II {
public:
    T1& population;

    NSGA_II(T1& _population) : population(_population) {}

    void NSGA_II_Genetic(double mutation_rate, int stagnation) {
        for (int i = 0; i < stagnation; i++) {
            vector<T2> Child;

            while (Child.size() < population.size) {
                T2 parent1 = population.TNselection();
                T2 parent2 = population.TNselection();

                T2 child1 = population.crossover(parent1, parent2);
                T2 child2 = population.crossover(parent2, parent1);

                child1 = population.mutate(child1, mutation_rate);
                child2 = population.mutate(child2, mutation_rate);

                Child.push_back(child1);
                Child.push_back(child2);
            }

            for (auto& x : Child) {
                population.Member.push_back(x);
            }

            population.sort_by_domination_crowdingdistance();
            population.create_next_member();
        }
    }
};

template<typename T1, typename T2>
class MOEA{
public:
    T1 population;
    MOEA(T1 _population): population(_population) {}
    void MOEA_Genetic(double mutation_rate, int stagnation) {
         for (int i = 0; i < stagnation; i++) {
            vector<T2> Child;

            while (Child.size() < population.size) {
                T2 parent1 = population.TNselection();
                T2 parent2 = population.SelectByParent1(parent1);

                T2 child1 = population.crossover(parent1, parent2);
                T2 child2 = population.crossover(parent2, parent1);

                child1 = population.mutate(child1, mutation_rate);
                child2 = population.mutate(child2, mutation_rate);

                Child.push_back(child1);
                Child.push_back(child2);
            }

            for (auto& x : Child) {
                population.Member.push_back(x);
            }

            population.sort_by_domination_crowdingdistance();
            population.create_next_member();
        }
    }
};


class Population{
public:
    int size; // kích thước quần thể
    int n;// số điểm cần phục vụ
    int Q; // tải trọng tối đa của xe
    vector<vector<double>>& Expensive; // ma trận khoảng cách trong bài này ta coi chi phí chính là thời gian di chuyển giữa các điểm
    vector<double>& Weight; // khối lượng hàng phục vụ tại vị trí i
    vector<double>& TimeServe; // thời gian phục vụ
    vector<pair<double,double>>& Timewindow; // cửa số thời gian tường khách hàng
    NSGA_II<Population,Route> nsga_ii;
    vector<Route> Member;// thành viên quần thể


    Population(int _size,int N, int q, vector<vector<double>>& expensive, vector<double>& weight, vector<double>& timeserve,  vector<pair<double,double>>& timewindow):
        size(_size),n(N),Q(q), Expensive(expensive), Weight(weight), TimeServe(timeserve), Timewindow(timewindow), nsga_ii(*this){
            // khởi tạo quần thể
            set<Route> inserted;
            int n_nearest_neighbor = size/8;
            int n_random_suffle = size -n_nearest_neighbor;


            while (inserted.size() < n_random_suffle)
            {
                vector<vector<int>> u;
                unordered_set<int> S;
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
                    for(int v : S){
                        if(checkValid(x,v,Expensive,Timewindow,Weight,TimeServe,Q)){
                            x.push_back(v);
                            S.erase(v);
                        }
                    }
                    x.push_back(0);
                    u.push_back(x);
                }
                Route r(u,n,Expensive,Timewindow,Weight,TimeServe,Q);
                if(r.isValid) {inserted.insert(r);
                cout << "đã tạo cá thể bởi RDSF: " << endl;
                r.print();
                }
            }
            while(inserted.size() < size){
                vector<vector<int>> u;
                vector<int> S;
                for (int j = 1; j <= n; j++)
                {
                    S.push_back(j);
                }
                while (!S.empty())
                {
                    vector<int> x;
                    x.push_back(0);
                    int index = getRandomNumber(0, S.size() - 1);
                    x.push_back(S[index]);
                    S.erase(S.begin()+index);
                    sort(S.begin(),S.end(),[&](int a, int b){
                        return Expensive[x.back()][a]<Expensive[x.back()][b];
                    });
                    for(int v : S){
                        if(checkValid(x,v,Expensive,Timewindow,Weight,TimeServe,Q)){
                            x.push_back(v);
                            S.erase(find(S.begin(),S.end(),v));
                        }
                    }
                    x.push_back(0);
                    u.push_back(x);
                }
            }
            for(Route r : inserted){
                Member.push_back(r);
            }
        }

        void sort_by_domination_crowdingdistance()
        {
            vector<vector<int>> F;
            vector<int> n(Member.size(), 0);
            vector<vector<int>> S(Member.size());
            map<Route,double> crowding_distance;

            // Xác định tập Pareto front
            for (int i = 0; i < Member.size(); i++)
            {
                for (int j = 0; j < Member.size(); j++)
                {
                    if (Member[i] < Member[j])
                    {
                        S[i].push_back(j);
                    }
                    else if (Member[j] < Member[i])
                    {
                        n[i]++;
                    }
                }
                if (n[i] == 0)
                {
                    F.push_back({i});
                }
            }

            int i = 0;
            while (!F[i].empty())
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
                i++;
                F.push_back(H);
            }
            F.pop_back();

            // Xác định rank của từng cá thể
            map<Route,int> rank;
            for (int i = 0; i < F.size(); i++)
            {
                for (int x : F[i])
                {
                    rank[Member[x]] = i;
                }
            }

            // Duyệt qua từng tập mặt Pareto front riêng biệt
            for (auto &front : F)
            {
                if (front.size() < 2)
                    continue; // Nếu mặt có 1 điểm, không tính crowding distance
                sort(front.begin(), front.end(), [&](int a, int b)
                     { return Member[a].f1 < Member[b].f1; });
                crowding_distance[Member[front[0]]] = crowding_distance[Member[front.back()]] = numeric_limits<double>::infinity();
                for (int i = 1; i < front.size() - 1; i++)
                {
                    if ((Member[front.back()].f1 - Member[front[0]].f1)!=0)
                    {
                        crowding_distance[Member[front[i]]] += (Member[front[i + 1]].f1 - Member[front[i - 1]].f1) / (Member[front.back()].f1 - Member[front[0]].f1);
                    }
                }
                sort(front.begin(), front.end(), [&](int a, int b)
                     { return Member[a].f2 < Member[b].f2; });
                crowding_distance[Member[front[0]]] = crowding_distance[Member[front.back()]] = numeric_limits<double>::infinity();
                for (int i = 1; i < front.size() - 1; i++)
                {
                    if ((Member[front.back()].f2 - Member[front[0]].f2) != 0 && crowding_distance[Member[front[i]]] != numeric_limits<double>::infinity())
                    {
                        crowding_distance[Member[front[i]]] += (Member[front[i + 1]].f2 - Member[front[i - 1]].f2) / (Member[front.back()].f2 - Member[front[0]].f2);

                    }

                }
                sort(front.begin(), front.end(), [&](int a, int b)
                     { return Member[a].f3 < Member[b].f3; });
                crowding_distance[Member[front[0]]] = crowding_distance[Member[front.back()]] = numeric_limits<double>::infinity();
                for (int i = 1; i < front.size() - 1; i++)
                {
                    if ((Member[front.back()].f3 - Member[front[0]].f3) != 0 && crowding_distance[Member[front[i]]] != numeric_limits<double>::infinity())
                    {
                        crowding_distance[Member[front[i]]] += (Member[front[i + 1]].f3 - Member[front[i - 1]].f3) / (Member[front.back()].f3 - Member[front[0]].f3);
                    }
                }
            }

            sort(Member.begin(), Member.end(), [&](Route a, Route b)
                 {
                     if (rank[a] != rank[b])
                         return rank[a] < rank[b];
                     return crowding_distance[a] > crowding_distance[b];
                 });

        }

        Route TNselection(int k= 3){
            vector<Route> selected;
            while(selected.size()<k){
                int index = getRandomNumber(0,Member.size()-1);
                selected.push_back(Member[index]);
            }
            sort(selected.begin(),selected.end(),[&](Route a, Route b){
                return a<b;
            });
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
            int k = getRandomNumber(1, parent1.u.size() - 2); // số hành trình sẽ lấy từ cha 1
            set<int> chosen;
            while (chosen.size() < k)
            {
                int index = getRandomNumber(0, parent1.u.size() - 1);
                chosen.insert(index);
                u.push_back(parent1.u[index]);
                for (int v : parent1.u[index])
                {
                    S.erase(v);
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
            for (int v : S)
            {
                for (vector<int> &x : u)
                {
                    x.pop_back();
                    if (checkValid(x, v, Expensive, Timewindow, Weight, TimeServe, Q))
                    {
                        x.push_back(v);
                        x.push_back(0);
                        S.erase(v);
                        break;
                    }
                    x.push_back(0);
                }
            }

            while (!S.empty())
            {
                vector<int> x;
                x.push_back(0);
                int index = getRandomNumber(0, S.size() - 1);
                auto it = S.begin();
                advance(it, index);
                x.push_back(*it);
                S.erase(it);
                for (int v : S)
                {
                    if (checkValid(x, v, Expensive, Timewindow, Weight, TimeServe, Q))
                    {
                        x.push_back(v);
                        S.erase(v);
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
                    if (choose == 0)
                    {                                                 // đảo đoạn tuyến
                        int index = getRandomNumber(0, u.size() - 1); // tuyến bị đảo
                        if (u[index].size() > 2)
                        {                                                        // Kiểm tra kích thước tuyến
                            int l = getRandomNumber(1, u[index].size() - 2);     // điểm bắt đầu đảo
                            int r = getRandomNumber(l + 1, u[index].size() - 2); // điểm kết thúc đảo
                            reverse(u[index].begin() + l, u[index].begin() + r + 1);
                            if (checkValid(u[index], 0, Expensive, Timewindow, Weight, TimeServe, Q))
                            {
                                mutated = true;
                            }
                            else
                            {
                                reverse(u[index].begin() + l, u[index].begin() + r + 1);
                            }
                        }
                    }
                    else if (choose == 1)
                    {                                                 // thêm điểm vào tuyến này xóa ở tuyến khác
                        int index = getRandomNumber(0, u.size() - 1); // tuyến bị thêm
                        int v = getRandomNumber(1, n);                // điểm cần thêm
                        for (int i = 0; i < u.size(); i++)
                        {
                            if (i != index)
                            {
                                auto it = find(u[i].begin(), u[i].end(), v);
                                if (it != u[i].end())
                                {
                                    u[i].erase(it);
                                    if (checkValid(u[i], 0, Expensive, Timewindow, Weight, TimeServe, Q))
                                    {
                                        int pos = getRandomNumber(1, u[index].size() - 1);
                                        u[index].insert(u[index].begin() + pos, v);
                                        if (checkValid(u[index], 0, Expensive, Timewindow, Weight, TimeServe, Q))
                                        {
                                            mutated = true;
                                        }
                                        else
                                        {
                                            u[index].erase(u[index].begin() + pos);
                                            u[i].insert(it, v);
                                        }
                                    }
                                    else
                                    {
                                        u[i].insert(it, v);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {                                                  // ghép tuyến
                        int index1 = getRandomNumber(0, u.size() - 1); // tuyến bị ghép
                        int index2 = getRandomNumber(0, u.size() - 1); // tuyến ghép
                        if (index1 != index2)
                        {
                            vector<int> x = u[index1];
                            vector<int> y = u[index2];
                            vector<int> z;
                            z.push_back(0);
                            for (int i = 1; i <= x.size() - 2; i++)
                            {
                                z.push_back(x[i]);
                            }
                            for (int i = 1; i <= y.size() - 2; i++)
                            {
                                z.push_back(y[i]);
                            }
                            z.push_back(0);
                            vector<vector<int>> z1;
                            if (checkValid(z, 0, Expensive, Timewindow, Weight, TimeServe, Q))
                            {
                                z1.push_back(z);
                                for (int i = 0; i < u.size(); i++)
                                {
                                    if (i != index1 && i != index2)
                                    {
                                        z1.push_back(u[i]);
                                    }
                                }
                                u = z1;
                                mutated = true;
                            }
                        }
                    }
                }
            }
            return Route(u, n, Expensive, Timewindow, Weight, TimeServe, Q);
        }

        void create_next_member(){
            vector<Route> next_member;
            for (int i = 0; i < size; i++)
            {
                next_member.push_back(Member[i]);
            }
            Member = next_member;
        }

        void Genetic(string Method ="NSGA_II", double mutation_rate = 0.4, int stagnation = 100){
            if(Method=="NSGA_II"){
                nsga_ii.NSGA_II_Genetic(mutation_rate,stagnation);
            }
        }
};

int main()
{
    // Số lượng điểm khách hàng (bao gồm điểm xuất phát 0)
    int N = 5;
    double Q = 100.0; // Tải trọng tối đa của xe

    // Ma trận khoảng cách giữa các điểm
    vector<vector<double>> Expensive = {
        {0, 10, 15, 20, 25},
        {10, 0, 35, 25, 30},
        {15, 35, 0, 30, 40},
        {20, 25, 30, 0, 15},
        {25, 30, 40, 15, 0}};

    // Cửa sổ thời gian (mỗi cặp <mở, đóng>)
    vector<pair<double, double>> Timewindow = {
        {0, 100}, // Điểm xuất phát
        {5, 20},  // Khách hàng 1
        {10, 25}, // Khách hàng 2
        {15, 30}, // Khách hàng 3
        {20, 35}  // Khách hàng 4
    };

    // Khối lượng hàng tại mỗi điểm
    vector<double> Weight = {0, 10, 20, 15, 25};

    // Thời gian phục vụ tại mỗi điểm
    vector<double> TimeServe = {0, 2, 3, 4, 5};

    // Khởi tạo quần thể ban đầu
    int size_population = 10;
    Population population(size_population, N, Q, Expensive, Weight, TimeServe, Timewindow);

    // Chạy thuật toán NSGA-II
    double mutation_rate = 0.1;
    int stagnation = 50;
    population.nsga_ii.NSGA_II_Genetic(mutation_rate, stagnation);

    // In kết quả tốt nhất tìm được
    cout << "Tuyến đường tối ưu:\n";
    population.Member[0].print();

    return 0;
}