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

        bool operator<(const Sortir& other) const {
            return this->id_sortir < other.id_sortir; // Bandingkan nama secara leksikografis
        }
        void print() const {
            std::cout << "Sortir ID: " << id_sortir 
                      << ", Nama: " << nama 
                      << ", Longitude: " << lokasi.getLongitude() 
                      << ", Latitude: " << lokasi.getLatitude() 
                      << std::endl;
        }

        static std::vector<Sortir> read_sortir(const std::string& filename) {
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
};