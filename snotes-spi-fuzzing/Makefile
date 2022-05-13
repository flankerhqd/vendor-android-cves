ANDROID_NDK=/home/test/android-ndk-r20b/
#ANDROID_PATH=/home/test/samsung11/
ANDROID_PATH=/home/test/samsung/system
APPLIB_PATH=/home/test/notesfuzz/samsung_app/lib/arm64-v8a

CC=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang
CXX=$(ANDROID_NDK)/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android29-clang++
CXXFLAGS=-D__ANDROID_API__=29 -D_LIBCPP_ABI_NAMESPACE=__1 -DANDROID_STL=c++_shared
LDFLAGS= -Wl,-rpath -Wl,$(ANDROID_PATH)/lib64 -L$(ANDROID_PATH)/lib64 -lSPenBase  -L$(APPLIB_PATH) -Wl,-rpath -Wl,$(APPLIB_PATH) -Wl,--dynamic-linker=$(ANDROID_PATH)/bin/linker64
#LDFLAGS= -Wl,-rpath -Wl,$(ANDROID_PATH)/lib64  -L$(ANDROID_PATH)/lib64 -lc++  -lhwui -ldl -lbacktrace -Wl,-rpath -Wl,$(APPLIB_PATH) -L$(APPLIB_PATH)  -lSPenBase -Wl,--dynamic-linker=$(ANDROID_PATH)/bin/linker64
#DEPS=loader
OBJS=spiloader.o

all: spiloader

%.o: %.cc #$(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

spiloader: $(OBJS) 
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) spiloader $(OBJS)
