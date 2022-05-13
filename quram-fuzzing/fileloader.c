#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <android/log.h>
struct DecodeArg{
    int f0;
    int f1;
    int width;
    int height;
    int insamplesize;
    int somergboptions;
    int f18;
    int imagetype;
    int f20;
    int f201;
    int f24;
    int f2c;
    int f30;
    int f34;
    int f38;
};

extern int QuramGetImageInfoFromFile2(char* filename, int start, int end, int* width, int* height, int* noquramflag, int* otherflag);
extern int QrDecodeImage(char* filename, char* outbuf, int end, int rgbopt, struct DecodeArg* arg, int opt);
extern int QuramGetImageInfoFromBuffer2(char* filebytes, int start, int end, int* width, int* height, int* noquramflag, int* otherflag);
int func(char* filepath)
{
    printf("file name %s\n", filepath);
    __android_log_print(ANDROID_LOG_INFO, "FILELOADER", "loading file %s", filepath);
    int width = 0, height = 0;
    int noquramflag = 0, otherflag = 0;
    int ret = QuramGetImageInfoFromFile2(filepath, 0, 0, &width, &height, &noquramflag, &otherflag);
    printf("ret is %d\n", ret);
    printf("width 0x%x height 0x%x noquramflag 0x%x otherflag 0x%x\n", width, height, noquramflag, otherflag);

    char* outbuf = (char*)malloc(width * height * 4);//for rgb-8888
    if(outbuf == NULL) return -1;
    memset(outbuf, 0, width * height * 4);
    struct DecodeArg arg;
    memset(&arg, 0, sizeof(arg));
    arg.width = width;
    arg.height = height;
    arg.imagetype = ret;
    arg.somergboptions = 7;
    arg.insamplesize = 1;
    ret = QrDecodeImage(filepath, outbuf, 0, 7, &arg, 1);
    printf("decode bytes 1-5 %x %x %x %x %x\n", outbuf[0], outbuf[1], outbuf[2], outbuf[3], outbuf[4]);
    printf("decode ret %d\n", ret);
    free(outbuf);

    return ret;
}

int funcbytearrcaller(const char* filepath)
{
    FILE* file = fopen(filepath, "rb");
    fseek(file, 0, SEEK_END); // seek to end of file
    size_t size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file

    char* buf = (char*)malloc(size);
    fread(buf, size, 1, file);
    fclose(file);

    printf("file name %s\n", filepath);
    //__android_log_print(ANDROID_LOG_INFO, "FILELOADER", "loading file in bytearray way %s", filepath);
    return funcbytearr(buf, size);
}

int funcbytearr(char* buf, size_t size)
{
    int width = 0, height = 0;
    int noquramflag = 0, otherflag = 0;
    int ret = QuramGetImageInfoFromBuffer2(buf, 0, size, &width, &height, &noquramflag, &otherflag);
    printf("ret is %d\n", ret);
    printf("width 0x%x height 0x%x noquramflag 0x%x otherflag 0x%x\n", width, height, noquramflag, otherflag);

    char* outbuf = (char*)malloc(width * height * 4);//for rgb-8888
    if(outbuf == NULL) return -1;
    memset(outbuf, 0, width * height * 4);
    struct DecodeArg arg;
    memset(&arg, 0, sizeof(arg));
    arg.width = width;
    arg.height = height;
    arg.imagetype = ret;
    arg.somergboptions = 7;
    arg.insamplesize = 1;
    ret = QrDecodeImage(buf, outbuf, size, 7, &arg, 1);
    printf("decode bytes 1-5 %x %x %x %x %x\n", outbuf[0], outbuf[1], outbuf[2], outbuf[3], outbuf[4]);
    printf("decode ret %d\n", ret);
    free(outbuf);
    free(buf);

    return ret;
}
int main(int argc, char** argv)
{
    for(int i=1;i<argc;i++)
    {
        funcbytearrcaller(argv[i]);
    }
    return 0;
}
