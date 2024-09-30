#include "nanoflann.hpp"
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
#include "lib/ThreadPool.cpp"
#include "lib/Koordinat.cpp"
#include "lib/Sortir.cpp"
#include "lib/Order.cpp"
#include "lib/Distribusi.cpp"
#include "lib/Urutkan.cpp"

int main() {
    std::vector<Order> orders = Order::read_orders("order.txt");
    std::vector<Sortir> sortirs = Sortir::read_sortir("sortir.txt");
    ThreadPool pool(std::thread::hardware_concurrency());

    // Serial
    auto start = std::chrono::high_resolution_clock::now();
    std::map<Sortir, std::vector<Order>> distribusi = distribusiBarangSerial(orders, sortirs);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Waktu untuk distribusi serial: " << duration.count() << " detik" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    urutkanPrioritasSerial(distribusi);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Waktu untuk mengurutkan serial: " << duration.count() << " seconds" << std::endl;
    //tampilkanDistribusi(distribusi);

    // Paralel
    start = std::chrono::high_resolution_clock::now();
    distribusi = distribusiBarangParalel(orders, sortirs, pool, 10000);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Waktu untuk distribusi paralel: " << duration.count() << " detik" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    urutkanPrioritasParalel(distribusi, pool);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Waktu untuk mengurutkan paralel: " << duration.count() << " seconds" << std::endl;
    


    return 0;
}

// g++ -std=c++11 -pthread -o main Main.cpp
// ./main
