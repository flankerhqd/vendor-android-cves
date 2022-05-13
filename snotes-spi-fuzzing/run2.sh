#!/bin/bash

#export TARGET_CPU=aarch64
#export AFL_AUTORESUME=1
#export AFL_QEMU_DEBUG_MAPS=1
#export AFL_DEBUG=2 
#export AFL_INST_LIBS=1 
#export AFL_COMPCOV_LEVEL=2
#export AFL_INST_RATIO=10
#export AFL_QEMU_PERSISTENT_GPR=1
#export AFL_QEMU_PERSISTENT_ADDR=0x4000002900
export AFL_PRELOAD= QEMU_SET_ENV="LD_LIBRARY_PATH=/home/xxx/Desktop/Library/Android/sdk/ndk/android-ndk-r21e-linux-x86_64/android-ndk-r21e/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android:/home/xxx/Desktop/work/repo/a70s/lib64:/home/xxx/Desktop/work/infos/sobuild/samsung_app/libs"
#export AFL_CUSTOM_MUTATOR_LIBRARY=/home/xxx/Desktop/fuzzer/AFLplusplus/custom_mutators/radamsa/radamsa-mutator.so
#export AFL_CUSTOM_MUTATOR_LIBRARY=/home/xxx/Desktop/fuzzer/AFLplusplus/custom_mutators/radamsa/radamsa-mutator.so



if [[ $# == 4 ]]; then
    if [[ $3 == "-m" ]]; then
        if [[ $4 == "radamsa" ]]; then
            export AFL_CUSTOM_MUTATOR_LIBRARY=/fuzz/AFLplusplus/custom_mutators/radamsa/radamsa-mutator.so
        elif [[ $4 == "honggfuzz" ]]; then
            export AFL_CUSTOM_MUTATOR_LIBRARY=/fuzz/AFLplusplus/custom_mutators/honggfuzz/honggfuzz-mutator.so 
        elif [[ $4 == "libfuzzer" ]]; then
            export AFL_CUSTOM_MUTATOR_LIBRARY=/fuzz/AFLplusplus/custom_mutators/libfuzzer/libfuzzer-mutator.so 
        else 
            echo "custom_mutators"
        fi 
    fi 
    if [[ $1 == "-M" ]]; then
        /fuzz/AFLplusplus/afl-fuzz -i spifiles -o out -m none -t 100000+ -Q -M $2 ./spiloader -f @@
    elif [[ $1 == "-S" ]]; then
        /fuzz/AFLplusplus/afl-fuzz -i spifiles -o out -m none -t 100000+ -Q -S $2 ./spiloader -f @@
    else
        echo "wrong cmd"
    fi
else 
    echo "wrong cmd"
fi

