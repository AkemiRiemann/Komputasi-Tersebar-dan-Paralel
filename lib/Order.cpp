#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

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

        static std::vector<Order> read_orders(const std::string& filename) {
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
};