CAPSTONE_PATH=/home/test/SkCodecFuzzer/deps/capstone
LIBBACKTRACE_PATH=/home/test/SkCodecFuzzer/deps/libbacktrace

CC=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang
CXX=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang++
CXXFLAGS=-D_LIBCPP_ABI_NAMESPACE=__1 -I$(CAPSTONE_PATH)/include -I$(LIBBACKTRACE_PATH)/include
LDFLAGS=-L$(CAPSTONE_PATH) -lcapstone -L$(ANDROID_PATH)/lib64 -lhwui -ldl -lbacktrace -Wl,-rpath -Wl,$(ANDROID_PATH)/lib64 -L$(ANDROID_PATH)/lib64 -limagecodec.quram  -Wl,--dynamic-linker=$(ANDROID_PATH)/bin/linker64 -llog
OBJS=dngdriver.o

all: loader

%.o: %.cc $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)


loader: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) loader $(OBJS)

