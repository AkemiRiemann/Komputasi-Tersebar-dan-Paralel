void urutkanPrioritas(std::vector<Order>& orders, Koordinat koor){
    auto distance = [](const Koordinat& a, const Koordinat& b) {
        int dx = a.getLongitude() - b.getLongitude();
        int dy = a.getLatitude() - b.getLatitude();
        return std::sqrt(dx * dx + dy * dy);
    };

    std::sort(orders.begin(), orders.end(), [&](const Order& a, const Order& b) {
        if (a.getPrioritas() != b.getPrioritas()) {
            return a.getPrioritas() < b.getPrioritas();
        } else {
            return distance(a.getLokasi(), koor) < distance(b.getLokasi(), koor);
        }
    });
}

void urutkanPrioritasParalel(std::map<Sortir, std::vector<Order>>& distribusi, ThreadPool& pool) {
    std::vector<std::future<void>> futures;

    for (auto& pair : distribusi) {
        // Kirim tugas ke thread pool untuk memproses setiap entry di distribusi
        futures.push_back(pool.enqueue([&pair]() {
            urutkanPrioritas(pair.second, pair.first.getLokasi());
        }));
    }

    // Tunggu semua thread selesai mengurutkan
    for (auto& future : futures) {
        future.get();
    }
}


void urutkanPrioritasSerial(std::map<Sortir, std::vector<Order>> &distribusi){
    for (auto& pair : distribusi) {
        urutkanPrioritas(pair.second, pair.first.getLokasi());
    }
}
