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

class Route{
public:
    vector<vector<int>> u; // lưu trữ tuyến đường
    double Q; //tải trọng tối đa của xe
    int f1; // số xe
    vector<vector<double>>& Expensive; // ma trận khoảng cách trong bài này ta coi chi phí chính là thời gian di chuyển giữa các điểm
    vector<double>& Weight; // khối lượng hàng phục vụ tại vị trí i
    vector<double>& TimeServe; // thời gian phục vụ
    vector<pair<double,double>>& Timewindow; // cửa số thời gian tường khách hàng
    double f2; // tổng khoảng cách di chuyển
    double f3; // tổng thơi gian di chuyển giả thiết khách hàng đầu tiên được phục vụ ngay tại thời điểm bắt đầu cửa sổ thời gian
    bool isValid; // Kiểm tra tuyến có hợp lệ không


    Route(vector<vector<int>> U, vector<vector<double>>& expensive,vector<pair<double,double>>& timewindow, vector<double>& weight,  vector<double>& timeServe, double q):
        u(U),Expensive(expensive), Timewindow(timewindow),Weight(weight),TimeServe(timeServe),isValid(true),Q(q){
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


    bool operator <(const Route& other){
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
    vector<vector<double>>& Expensive; // ma trận khoảng cách trong bài này ta coi chi phí chính là thời gian di chuyển giữa các điểm
    vector<double>& Weight; // khối lượng hàng phục vụ tại vị trí i
    vector<double>& TimeServe; // thời gian phục vụ
    vector<pair<double,double>>& Timewindow; // cửa số thời gian tường khách hàng
    vector<Route> Member;// thành viên quần thể

    Population(int _size, vector<vector<double>>& expensive, vector<double>& weight, vector<double>& timeserve,  vector<pair<double,double>>& timewindow):
        size(_size), Expensive(expensive), Weight(weight), TimeServe(timeserve), Timewindow(timewindow){
            // khởi tạo quần thể


        }

};

int main(){
    cout << "Hello world";
}

