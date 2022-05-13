#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <android/log.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

void SetSignalHandler(void (*handler)(int, siginfo_t *, void *)) {
	  struct sigaction action;
	    memset(&action, 0, sizeof(action));

	      action.sa_flags = SA_SIGINFO | SA_NODEFER;
	        action.sa_sigaction = handler;
		  if (sigaction(SIGABRT, &action, NULL) == -1) {
			      perror("sigabrt: sigaction");
			          _exit(1);
				    }
		    if (sigaction(SIGFPE, &action, NULL) == -1) {
			        perror("sigfpe: sigaction");
				    _exit(1);
				      }
		      if (sigaction(SIGSEGV, &action, NULL) == -1) {
			          perror("sigsegv: sigaction");
				      _exit(1);
				        }
		        if (sigaction(SIGILL, &action, NULL) == -1) {
				    perror("sigill: sigaction");
				        _exit(1);
					  }
			  if (sigaction(SIGBUS, &action, NULL) == -1) {
				      perror("sigbus: sigaction");
				          _exit(1);
					    }
}

static void GeneralSignalHandler(int signo, siginfo_t *info, void *extra) {
	_exit(171);
}


void myStartupFun (void) __attribute__ ((constructor))
{
    SetSignalHandler(GeneralSignalHandler);
}
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
extern QrDecodeDNGPreview(char* filebytes, int filelength, char* bitmapbuffer, int i, int j);

int parseDNGPreviewFromBuf(char* buf, size_t size)
{
    int i,j;
    int ret = QuramGetImageInfoFromBuffer(buf, 0, size, &i, &j);
    if(ret != 0) {
        //printf("QuramGetImageInfoFromBuffer returned non-zero: %d\n", ret);
        return 0;
    }
    ret = QrParseDNGPreview(buf, size, &i, &j);
    if(ret < 1) {
        //printf("QrParseDNGPreview returned less than 1: %d\n", ret);
        return 0;
    }
    char* outbuf = (char*)malloc(i * j * 4);//for rgb-8888
    if(outbuf == NULL) return -1;
    ret = QrDecodeDNGPreview(buf, size, outbuf, i, j);
    if(ret <= -1) {
        //printf("QrDecodeDNGPreview returned less than -1: %d\n", ret);
        return 0;
    }
    //printf("decode ret %d i %d j %d\n", ret, i, j);
    return ret;
}
int func(char* filepath)
{
    //printf("file name %s\n", filepath);
    //__android_log_print(ANDROID_LOG_INFO, "FILELOADER", "loading file %s", filepath);
    int width = 0, height = 0;
    int noquramflag = 0, otherflag = 0;
    int ret = QuramGetImageInfoFromFile2(filepath, 0, 0, &width, &height, &noquramflag, &otherflag);
    //printf("ret is %d\n", ret);
    //printf("width 0x%x height 0x%x noquramflag 0x%x otherflag 0x%x\n", width, height, noquramflag, otherflag);

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
    //printf("decode bytes 1-5 %x %x %x %x %x\n", outbuf[0], outbuf[1], outbuf[2], outbuf[3], outbuf[4]);
    //printf("decode ret %d\n", ret);
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

    //printf("file name %s\n", filepath);
    //__android_log_print(ANDROID_LOG_INFO, "FILELOADER", "loading file in bytearray way %s", filepath);
    int ret = funcbytearr(buf, size);
    free(buf);
    return ret;
}

int funcbytearr(char* buf, size_t size)
{
    int width = 0, height = 0;
    int noquramflag = 0, otherflag = 0;
    int ret = QuramGetImageInfoFromBuffer2(buf, 0, size, &width, &height, &noquramflag, &otherflag);
    //printf("ret is %d\n", ret);
    //printf("width 0x%x height 0x%x noquramflag 0x%x otherflag 0x%x\n", width, height, noquramflag, otherflag);

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
    //printf("decode bytes 1-5 %x %x %x %x %x\n", outbuf[0], outbuf[1], outbuf[2], outbuf[3], outbuf[4]);
    //printf("decode ret %d\n", ret);
    free(outbuf);

    return ret;
}

extern int QrParseMetadata(char* filename, char* outbuf);

int ParseMetaDataWrapper(char* filename)
{
    char* buf = (char*)malloc(0x11c);
    QrParseMetadata(filename, buf);
    free(buf);
    return 0;
}

int parseDNGPreviewByteArr(char* buf, char* filepath, size_t size)
{
    char* metabuf = (char*)malloc(0x11c);
    memset(metabuf, 0, 0x11c);
    int ret = QrParseMetadata(filepath, metabuf);
    //printf("metaparse ret %d\n", ret);
    


    if(ret == 0){
        free(metabuf);
        return 0;
    }
    else{
/*
 *   char s; // [xsp+8h] [xbp-168h]
  int v21; // [xsp+Ch] [xbp-164h]
 */
	char* makeptr = (char*)(metabuf + 4);
	//printf("metabuf %p makeptr %p points to %p\n", metabuf, makeptr, *makeptr);
	if(*makeptr != NULL)
            printf("meta make %s\n", makeptr);
	else{
	    free(metabuf);
	    return 0;
	}
    }

    free(metabuf);
    ret = parseDNGPreviewFromBuf(buf, size);
    return ret;
}
int parseDNGPreview(const char* filepath)
{
    FILE* file = fopen(filepath, "rb");
    fseek(file, 0, SEEK_END); // seek to end of file
    size_t size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file

    char* buf = (char*)malloc(size);
    fread(buf, size, 1, file);
    fclose(file);

    //printf("file name %s\n", filepath);
    int ret = parseDNGPreviewByteArr(buf, filepath, size);
    free(buf);
    return ret;
}
int main(int argc, char** argv)
{
    //getchar();
//    for(int i=1;i<argc;i++)
    {
        //ParseMetaDataWrapper(argv[i]);
        parseDNGPreview(argv[1]);
    }
    return 0;
}
