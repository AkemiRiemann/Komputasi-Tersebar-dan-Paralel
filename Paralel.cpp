#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <string>
#include <fstream>
#include <sstream>
#include <numeric>
#include <chrono> 
#include <cmath>
#include <map>
#include <algorithm>

class Koordinat {
    private:
        double longitude;
        double latitude;
    public:
        double getLongitude() const { return longitude; }
        void setLongitude(double lon) { longitude = lon; }

        double getLatitude() const { return latitude; }
        void setLatitude(double lat) { latitude = lat; }

        Koordinat(double lon, double lat) : longitude(lon), latitude(lat) {}
};

class Sortir {
    private:
        std::string id_sortir, nama;
        Koordinat lokasi;
    public:
        Sortir(const std::string& id, const std::string& nama, double lon, double lat) 
            : id_sortir(id), nama(nama), lokasi(lon, lat) {}

        std::string getIdSortir() const { return id_sortir; }
        void setIdSortir(const std::string& id) { id_sortir = id; }

        std::string getNama() const { return nama; }
        void setNama(const std::string& n) { nama = n; }

        Koordinat getLokasi() const { return lokasi; }
        void setLokasi(const Koordinat& loc) { lokasi = loc; }

        void print() const {
            std::cout << "Sortir ID: " << id_sortir 
                      << ", Nama: " << nama 
                      << ", Longitude: " << lokasi.getLongitude() 
                      << ", Latitude: " << lokasi.getLatitude() 
                      << std::endl;
        }
};

class Order {
    private: 
        std::string id_order, id_pemesan;
        Koordinat lokasi;
        int prioritas;
    public:
        Order(const std::string& id_order, double lon, double lat, const std::string& id_pemesan, int prioritas)
            : id_order(id_order), lokasi(lon, lat), id_pemesan(id_pemesan), prioritas(prioritas) {}

        std::string getIdOrder() const { return id_order; }
        void setIdOrder(const std::string& id) { id_order = id; }

        std::string getIdPemesan() const { return id_pemesan; }
        void setIdPemesan(const std::string& id) { id_pemesan = id; }

        Koordinat getLokasi() const { return lokasi; }
        void setLokasi(const Koordinat& loc) { lokasi = loc; }

        int getPrioritas() const { return prioritas; }
        void setPrioritas(int p) { prioritas = p; }

        void print() const {
            std::cout << "Order ID: " << id_order << ", Pemesan ID: " << id_pemesan 
                      << ", Longitude: " << lokasi.getLongitude() 
                      << ", Latitude: " << lokasi.getLatitude() 
                      << ", Prioritas: " << prioritas << std::endl;
        }
};

std::vector<Order> read_orders(const std::string& filename) {
    std::vector<Order> orders;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id_order, id_pemesan;
        double longitude, latitude;
        int prioritas;
        char delimiter;

        std::getline(iss, id_order, '#');
        std::getline(iss, id_pemesan, '#');
        iss >> longitude >> delimiter >> latitude >> delimiter >> prioritas;

        orders.emplace_back(id_order, longitude, latitude, id_pemesan, prioritas);
    }

    return orders;
}

std::vector<Sortir> read_sortir(const std::string& filename) {
    std::vector<Sortir> sortirs;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string id_sortir, nama;
        double longitude, latitude;
        char delimiter;

        std::getline(iss, id_sortir, '#');
        std::getline(iss, nama, '#');
        iss >> longitude >> delimiter >> latitude >> delimiter;

        sortirs.emplace_back(id_sortir, nama, longitude, latitude);
    }

    return sortirs;
}

std::map<std::string, std::vector<Order>> distribusiBarang(std::vector<Sortir> sortirs, std::vector<Order> orders){
    std::map<std::string, std::vector<Order>> sortir_map;

    auto distance = [](const Koordinat& a, const Koordinat& b) {
        double dx = a.getLongitude() - b.getLongitude();
        double dy = a.getLatitude() - b.getLatitude();
        return std::sqrt(dx * dx + dy * dy);
    };

    for (const auto& order : orders) {
        Sortir* closest_sortir = nullptr;
        double min_distance = std::numeric_limits<double>::max();

        for (auto& sortir : sortirs) {
            double dist = distance(order.getLokasi(), sortir.getLokasi());
            if (dist < min_distance) {
                min_distance = dist;
                closest_sortir = &sortir;
            }
        }

        if (closest_sortir) {
            // std::cout << "Order: ";
            // order.print();
            // std::cout << "Assigned to Sortir: ";
            // closest_sortir->print();
            // std::cout << std::endl;

            sortir_map[closest_sortir->getNama()].push_back(order);
        }
        
    }

    return sortir_map;
}

void urutkanPrioritas(std::vector<Order>& orders){
    auto distance = [](const Koordinat& a, const Koordinat& b) {
        int dx = a.getLongitude() - b.getLongitude();
        int dy = a.getLatitude() - b.getLatitude();
        return std::sqrt(dx * dx + dy * dy);
    };

    std::sort(orders.begin(), orders.end(), [&](const Order& a, const Order& b) {
        if (a.getPrioritas() != b.getPrioritas()) {
            return a.getPrioritas() < b.getPrioritas();
        } else {
            Koordinat origin(0, 0);
            return distance(a.getLokasi(), origin) < distance(b.getLokasi(), origin);
        }
    });
}

void urutkanPrioritasParalel(std::map<std::string, std::vector<Order>> &distribusi){
    std::vector<std::future<void>> futures;

    for (auto& pair : distribusi) {
        futures.push_back(std::async(std::launch::async, [&pair]() {
            urutkanPrioritas(pair.second);
        }));
    }
}

void urutkanPrioritasSerial(std::map<std::string, std::vector<Order>> &distribusi){
    for (auto& pair : distribusi) {
        urutkanPrioritas(pair.second);
    }
}

int main() {

    std::vector<Order> orders = read_orders("order.txt");

    std::vector<Sortir> sortirs = read_sortir("sortir.txt");

    std::cout << orders.size() << ' ' << sortirs.size() << std::endl;

    std::map<std::string, std::vector<Order>> distribusi = distribusiBarang(sortirs, orders);
    auto hmm = distribusi;

    auto start = std::chrono::high_resolution_clock::now();
    urutkanPrioritasParalel(distribusi);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken for parallel sorting: " << duration.count() << " seconds" << std::endl;

    // orders = read_orders("order.txt");
    // sortirs = read_sortir("sortir.txt");
    std::cout << orders.size() << ' ' << sortirs.size() << std::endl;
    distribusi = hmm;

    start = std::chrono::high_resolution_clock::now();
    urutkanPrioritasSerial(distribusi);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Time taken for serial sorting: " << duration.count() << " seconds" << std::endl;

    return 0;
}