#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <android/log.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

/*

public static boolean parseMetadata(String pathName) {
    if((QuramBitmapFactory.sQuramEnabled) && !TextUtils.isEmpty(pathName)) {
        try {
            QuramCodecMetadata dngMetadataInfo = new QuramCodecMetadata();
            QuramCodecInterface.nativeParseMetadata(pathName, dngMetadataInfo);
            String make = dngMetadataInfo.getMake();
            if(make != null) {
                return make.toLowerCase(Locale.getDefault()).equals("samsung");
            }
        }
        catch(UnsatisfiedLinkError e) {
            QuramBitmapFactory.sQuramEnabled = false;
            Log.e("QuramBitmapFactory", "parseMetadata failed");
            return false;
        }
    }

    return false;
}

if(QuramBitmapFactory.isEnabled()) {
    if((arg8) && (ImageDecoder.checkIsDNGFile(arg6))) {
        bitmap = QuramBitmapFactory.decodeDNGPreview(arg6);
        int w = arg7.outWidth;
        int h = arg7.outHeight;
        if(bitmap != null && w > 0 && h > 0 && w != h) {
            bitmap = BitmapUtils.resizeBitmapByRatio(bitmap, ((float)w), ((float)h));
        }
    }

        fseek(v8, 0, 2);
    filelength = ftell(v10);
    fseek(v10, 0, 0);
    if ( filelength >= 1 && (filebuffer = malloc(filelength)) != 0 )
    {
      if ( !fread(filebuffer, 1u, filelength, v10) )
        goto LABEL_17;
      v4 = 0;
      if ( j_QuramGetImageInfoFromBuffer(filebuffer, 0, filelength, &v21, &v20) )
        goto LABEL_17;
      v13 = j_QrParseDNGPreview((int)filebuffer, filelength, (int)&v21, (int)&v20);
      if ( v13 < 1 )
        goto LABEL_17;
      v4 = 0;
      v19 = v13;
      v14 = j_createBitmap(env, v21, v20, 5, 0);
      if ( v14 )
      {
        v4 = v14;
        v15 = (*(int (__fastcall **)(int))(*(_DWORD *)env + 124))(env);
        if ( v15 )
        {
          v16 = (*(int (__fastcall **)(int, int, const char *, const char *))(*(_DWORD *)env + 132))(
                  env,
                  v15,
                  "setPremultiplied",
                  "(Z)V");
          if ( v16 )
            (*(void (__fastcall **)(int, int, int, signed int))(*(_DWORD *)env + 244))(env, v4, v16, 1);
        }
        if ( AndroidBitmap_lockPixels(env, v4, &bitmapbuffer) <= -1 )
        {
          _android_log_print(6, "QrBitampFactory", "AndroidBitmap_lockPixels() failed ! error=%d", v19);
LABEL_23:
          j_recycleBitmap(env, v4);
          v4 = 0;
          goto LABEL_17;
        }
        v17 = j_QrDecodeDNGPreview(filebuffer, filelength, bitmapbuffer, v21, v20);

*/
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
extern int QuramGetImageInfoFromBuffer(char* filebytes, int kind, int filelength, int* i, int* j);//should return 0
extern int QrParseDNGPreview(char* filebytes, int filelength, int* i, int *j);//should return >=1
extern int QrDecodeDNGPreview(char* filebytes, int filelength, char* bitmapbuffer, int i, int j);
extern int QrDecodeDNGFile(char* filename, char* outbuf, int end, int rgbopt, struct DecodeArg* arg);

extern int QrParseMetadata(char* filename, char* outbuf);

int parseDNG(const char* filepath)
{
    printf("file name %s\n", filepath);

    int width = 0, height = 0;
    int noquramflag = 0, otherflag = 0;
    int ret = QuramGetImageInfoFromFile2(filepath, 0, 0, &width, &height, &noquramflag, &otherflag);
    printf("ret is %d\n", ret);
    printf("width 0x%x height 0x%x noquramflag 0x%x otherflag 0x%x\n", width, height, noquramflag, otherflag);

    if(ret == 0)
    {
        //a possible raw image
        int* metabuf = (int*)malloc(71*sizeof(int));
        memset(metabuf, 0, 0x11c);
        ret = QrParseMetadata(filepath, metabuf);
        if(ret != 0)
        {
            struct DecodeArg arg;
            memset(&arg, 0, sizeof(arg));

            int width = metabuf[65];
            if(metabuf[69] > 0)
                width = metabuf[69];
            int height = metabuf[66];
            if(metabuf[70] > 0)
                height = metabuf[70];

            arg.imagetype = 0;
            arg.height = height;
            arg.width = width;
            arg.somergboptions = 7;
            arg.insamplesize = 1;

            printf("width is is 0x%x height 0x%x\n", width, height);

            printf("begin malloc\n");
            char* outbuf = (char*)malloc(width * height * 4);//for rgb-8888
            if(outbuf == NULL) return -1;
            //memset(outbuf, 0, width * height * 4);
            printf("memset done");

            ret = QrDecodeDNGFile(filepath, outbuf, 0, 7, &arg);
            printf("QrDecodeDng ret %d\n", ret);
            free(outbuf);
/*
  v7 = 0;
  v43 = v112[70];
  width = v112[65];
  if ( v112[69] > 0 )
    width = v112[69];
  height = v112[66];
  decodearg->imagetype = 0;
  if ( v43 > 0 )
    height = v43;
  decodearg->width = width;
  decodearg->height = height;
*/
        }
        free(metabuf);
    }
    return ret;
}
int main(int argc, char** argv)
{
    parseDNG(argv[1]);
    
    return 0;
}
