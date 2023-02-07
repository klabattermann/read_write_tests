
OUT = bin

# ALL: bin/rww_reader bin/rww_writer bin/rww_reader_single
ALL: $(OUT)/rww_writer $(OUT)/rww_reader_single

$(shell mkdir -p $(OUT))

$(OUT)/%: %.cpp
	g++ -std=c++17 -o $@ $< -lstdc++fs
