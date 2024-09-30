// Adaptor for nanoflann to work with std::vector<Sortir>
struct SortirCloud {
    std::vector<Sortir> sortirs;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return sortirs.size(); }

    // Returns the dim'th component of the idx'th point in the class
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
        if (dim == 0) return sortirs[idx].getLokasi().getLatitude();
        else return sortirs[idx].getLokasi().getLongitude();
    }

    // Optional bounding-box computation
    template <class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

using namespace nanoflann;

std::map<Sortir, std::vector<Order>> distribusiBarangSerial(const std::vector<Order>& orders, std::vector<Sortir>& sortirs) { 
    SortirCloud cloud;
    cloud.sortirs = sortirs;

    // Construct a kd-tree index
    typedef KDTreeSingleIndexAdaptor<
        L2_Simple_Adaptor<double, SortirCloud>,
        SortirCloud,
        2 /* dim */
    > my_kd_tree_t;

    my_kd_tree_t kd_tree(2 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    kd_tree.buildIndex();

    // Map to store the result of linking each Sortir to its assigned orders
    std::map<Sortir, std::vector<Order>> sortir_map;

    for (const auto& order : orders) {
        const size_t num_results = 1;
        size_t ret_index;
        double out_dist_sqr;
        nanoflann::KNNResultSet<double> resultSet(num_results);
        resultSet.init(&ret_index, &out_dist_sqr);

        // Store coordinates in local variables
        double query_pt[2] = { order.getLokasi().getLatitude(), order.getLokasi().getLongitude() };
        kd_tree.findNeighbors(resultSet, query_pt, nanoflann::SearchParameters(10));

        if (ret_index < sortirs.size()) {
            // Insert the order into the map, using the sortir as the key
            sortir_map[sortirs[ret_index]].push_back(order);
        }
    }

    // After processing all orders, return the map
    return sortir_map;
}

// Fungsi untuk distribusi barang menggunakan ThreadPool dan batch processing
std::map<Sortir, std::vector<Order>> distribusiBarangParalel(const std::vector<Order>& orders, std::vector<Sortir>& sortirs, ThreadPool& pool, size_t batchSize) {
    SortirCloud cloud;
    cloud.sortirs = sortirs;

    typedef KDTreeSingleIndexAdaptor<
        L2_Simple_Adaptor<double, SortirCloud>,
        SortirCloud,
        2 /* dim */
    > my_kd_tree_t;

    my_kd_tree_t kd_tree(2 /*dim*/, cloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    kd_tree.buildIndex();

    std::mutex mtx;
    std::vector<std::future<void>> futures;
    std::map<Sortir, std::vector<Order>> sortir_map;

    // Divide orders into batches
    for (size_t i = 0; i < orders.size(); i += batchSize) {
        futures.push_back(pool.enqueue([i, &kd_tree, &orders, &sortirs, &sortir_map, &mtx, batchSize]() {
            size_t end = std::min(i + batchSize, orders.size());
            std::map<Sortir, std::vector<Order>> local_sortir_map;

            for (size_t j = i; j < end; ++j) {
                const auto& order = orders[j];
                const size_t num_results = 1;
                size_t ret_index;
                double out_dist_sqr;
                nanoflann::KNNResultSet<double> resultSet(num_results);
                resultSet.init(&ret_index, &out_dist_sqr);

                double query_pt[2] = { order.getLokasi().getLatitude(), order.getLokasi().getLongitude() };
                kd_tree.findNeighbors(resultSet, query_pt, nanoflann::SearchParameters(10));

                if (ret_index < sortirs.size()) {
                    local_sortir_map[sortirs[ret_index]].push_back(order);
                }
            }

            // Lock mutex only when updating the shared map
            std::lock_guard<std::mutex> lock(mtx);
            for (const auto& pair : local_sortir_map) {
                sortir_map[pair.first].insert(sortir_map[pair.first].end(), pair.second.begin(), pair.second.end());
            }
        }));
    }

    for (auto& future : futures) {
        future.get();  // Wait for all threads to complete
    }

    return sortir_map;
}

void tampilkanDistribusi(const std::map<Sortir, std::vector<Order>>& distribusi) {
    for (const auto& pair : distribusi) {
        const Sortir& sortir = pair.first;
        const std::vector<Order>& orders = pair.second;

        sortir.print(); // Menampilkan informasi Sortir

        std::cout << "Pesanan yang terkait:" << std::endl;
        for (const auto& order : orders) {
            order.print(); // Menampilkan informasi setiap Order
        }
        std::cout << std::endl; // Menambahkan baris kosong untuk pemisahan
    }
}