export AFL_QEMU_PERSISTENT_GPR=1
export AFL_QEMU_PERSISTENT_ADDR=0x400000307c
export AFL_CUSTOM_MUTATOR_LIBRARY=$HOME/androidemu/AFLplusplus/custom_mutators/radamsa/radamsa-mutator.so
export AFL_COMPCOV_LEVEL=2 
export AFL_INST_LIBS=1

export ANDROID_PATH=$HOME/android-ndk-r20b/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android
export SAMSUNG_PATH=$HOME/samsung/system/lib64
export APP_PATH=$HOME/notesfuzz/samsung_app/lib/arm64-v8a

for i in $(seq 2 16); do
    AFL_INST_LIBS=1 QEMU_SET_ENV="LD_LIBRARY_PATH=$ANDROID_PATH:$SAMSUNG_PATH:$APP_PATH" ~/androidemu/AFLplusplus/afl-fuzz -Q -S fuzzer$i $@ >/dev/null 2>&1 &
    sleep 10
done
AFL_INST_LIBS=1 QEMU_SET_ENV="LD_LIBRARY_PATH=$ANDROID_PATH:$SAMSUNG_PATH:$APP_PATH" ~/androidemu/AFLplusplus/afl-fuzz -Q -M master $@
