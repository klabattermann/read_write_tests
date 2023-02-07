#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <chrono>
#include <thread>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

using namespace std::chrono; 
using namespace std::chrono_literals;

/*
 Test reading fomr a file while it is written to

 Testing
 1) start the writer on a node, usually different from the reader node
 2) start the reader

*/

int write_out(uint8_t *buf, size_t bl) {
    /* write the read block to a file 
    int fout = open("/tmp/wk_rww_1", O_TRUNC | O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if ( fout < 0 ) {
        std::cout << "lskdfjlkdjf \n";
    } else {
        write(fout, buf, bl);
        close(fout);
    }
    */

    auto ed = uint8_t(0xff);
    for ( int i=0; i<bl; i++ ) {
        if (buf[i] != ed) {
            //std::cout << std::hex << std::setw(2) << std::setfill('0') \
            //         << (unsigned int) buf[i] << " " << (unsigned int) ed \
            //        << std::dec << " " << i << "\n";
            return i;
        }
    }
    return -1;
}

int main(int argc, char** argv) {

    fs::path fpath { argv[1] };
    auto offset = std::stol(argv[2]);

    while ( true ) {
        if ( fs::exists(fpath) ) {
            break;
        }
        std::this_thread::sleep_for(200ms);
    }

    int fd = open(fpath.c_str(), O_RDONLY | O_DIRECT);
    if ( fd < 0 ) {
        std::cout << "Bad File Descriptor \n";
        return 1;
    }

    constexpr size_t rd_size = 1024 * 1024;
    //std::cout << "Read from " << fpath << " offset " << offset << " read-size " << rd_size << "\n";

    uint8_t* buf = new uint8_t[rd_size];
    uint8_t* ref = new uint8_t[rd_size];
    std::fill(ref, ref + rd_size, uint8_t(0xff));

    int rc;
    struct stat st_file_b, st_file_a;
    st_file_a.st_size = 0;
    st_file_b.st_size = 0;

    while ( true ) {
        auto start = high_resolution_clock::now();

        //rc = fstat(fd, &st_file_b);
        size_t nb = pread(fd, buf, rd_size, offset);

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        //std::cout << nb << " " << duration.count() << "\n";
        //std::cout << nb << " " << st_file_a.st_size << "\n";

        if ( nb == 0 ) {
            continue;
        }

        rc = fstat(fd, &st_file_a);
        auto rc = std::memcmp(buf, ref, nb);
        int err_pos = -2;
        if ( rc != 0 ) {
            err_pos = write_out(buf, nb);
        }
        std::cout << "match memcmp: " << rc << " size: " << nb << " file-size: " \
                  << st_file_b.st_size << " " << st_file_a.st_size << " offset: " << offset \
                  << " errorpos " << err_pos << "\n";
           break;
    }

    close(fd);

    return 0;
}
