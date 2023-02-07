

# ALL: bin/rww_reader bin/rww_writer bin/rww_reader_single
ALL: bin/rww_writer bin/rww_reader_single

bin/%: %.cpp
	g++ -std=c++17 -o $@ $< -lstdc++fs
