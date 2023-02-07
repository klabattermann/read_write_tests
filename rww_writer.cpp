
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <csignal>

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
using namespace std::chrono;


constexpr size_t size = 128 * 1024 * 1024;
constexpr size_t io_size = size * sizeof(uint32_t);
static uint32_t data[io_size];
volatile sig_atomic_t do_shutdown = 0;

/* 
   Usage:
     rww_writer path sz[GB] slee[mus]
*/

void signalHandler( int signum ) {
    do_shutdown = 1;
    // std::cout << " Interrupt signal (" << signum << ") received.\n";
}


int main(int argc, char** argv) {

    signal(SIGINT, signalHandler); 

    fs::path fpath { argv[1] };
    auto mus_sleep = std::stoi(argv[3]);
    auto szGB = std::stoi(argv[2]);

    std::cout << "Write to " << fpath << "\n";
    int fd = open(fpath.c_str(), O_WRONLY | O_CREAT | O_DIRECT, S_IRUSR);
    if ( fd < 0 ) {
        std::cout << "Bad File Descriptor \n";
        return 1;
    }

    std::memset(data, 0xffffffff, io_size * sizeof(uint32_t));

    size_t max_size = pow(2, 30) * szGB;
    auto start_time = high_resolution_clock::now();

    size_t total_sz = 0;
    //while (true)  {
    while ( !do_shutdown )  {
        auto start = high_resolution_clock::now();
        auto n_written = write(fd, data, io_size);
        auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
        //std::cout << n_written << " " << duration.count() << "\n";

        std::this_thread::sleep_for(std::chrono::microseconds(mus_sleep));

        if (n_written > 0) {
            total_sz += n_written;
            if ( total_sz >= max_size )
                break;
        }
    }

    auto elap = duration_cast<milliseconds>(high_resolution_clock::now() - start_time);
    std::cout << "Done elap[ms] " << elap.count() << " sz-written/sz-max " << total_sz << " " << max_size << "\n";
    close(fd);
}
