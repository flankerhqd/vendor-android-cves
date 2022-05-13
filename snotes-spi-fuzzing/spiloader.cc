#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include <locale.h>


typedef          long long ll;
typedef unsigned long long ull;
typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;
typedef ll              int64;
typedef ll              sint64;
typedef ull             uint64;

#define _BYTE  uint8
#define _WORD  uint16
#define _DWORD uint32
#define _QWORD uint64
#define __int64 long long
#define __int32 int
#define __int16 short
#define __int8  char
#define ll long long
#define uchar unsigned char
// extern int64_t loadGifImage(GifFileType* a1 ,__int64 a2, signed int *a3, unsigned int *a4, int a5);
// extern long long loadGifImage(long long a1, long long *a2, signed int *a3, unsigned int *a4, int a5);
// extern long long loadGifImage(GifFileType *,uchar **&,int &,int &,int);
// extern int64_t loadGifImage(int64_t a1, int64_t *a2, signed int *a3, unsigned int *a4, int a5);
// extern GifFileType* DGifOpenFileHandle(int a1, int* a2);
// extern "C" int64_t ipred_fast_c(int64_t a1, unsigned int a2);

extern void* read_maetel_argb(const uint8_t *a1, int *a2, int *a3, int *a4);
extern "C" void* _Z16read_maetel_argbPKN4SPen6StringEPiS3_S3_(void* a1, int* a2, int* a3, int* a4);

extern "C" int64_t maet_init();

extern "C" int64_t maetd_create(_QWORD *a1, int *a2);

extern "C" int64_t maetd_decode(int64_t a1);

extern "C" void* _ZN4SPen6StringC2Ev(void* obj);
extern "C" int _ZN4SPen6String9ConstructEPKti(void* obj, char* str, int len);

#define __STD_UTF_16__

//Pointer arrays must always include the array size, because pointers do not know about the size of the supposed array size.
void utf8_to_utf16(unsigned char* const utf8_str, int utf8_str_size, char16_t* utf16_str_output, int utf16_str_output_size) {
	//First, grab the first byte of the UTF-8 string
	unsigned char* utf8_currentCodeUnit = utf8_str;
	char16_t* utf16_currentCodeUnit = utf16_str_output;
	int utf8_str_iterator = 0;
	int utf16_str_iterator = 0;

	//In a while loop, we check if the UTF-16 iterator is less than the max output size. If true, then we check if UTF-8 iterator
	//is less than UTF-8 max string size. This conditional checking based on order of precedence is intentionally done so it
	//prevents the while loop from continuing onwards if the iterators are outside of the intended sizes.
	while (*utf8_currentCodeUnit && (utf16_str_iterator < utf16_str_output_size || utf8_str_iterator < utf8_str_size)) {
		//Figure out the current code unit to determine the range. It is split into 6 main groups, each of which handles the data
		//differently from one another.
		if (*utf8_currentCodeUnit < 0x80) {
			//0..127, the ASCII range.

			//We directly plug in the values to the UTF-16 code unit.
			*utf16_currentCodeUnit = (char16_t) (*utf8_currentCodeUnit);
			utf16_currentCodeUnit++;
			utf16_str_iterator++;

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator++;
		}
		else if (*utf8_currentCodeUnit < 0xC0) {
			//0x80..0xBF, we ignore. These are reserved for UTF-8 encoding.
			utf8_currentCodeUnit++;
			utf8_str_iterator++;
		}
		else if (*utf8_currentCodeUnit < 0xE0) {
			//128..2047, the extended ASCII range, and into the Basic Multilingual Plane.

			//Work on the first code unit.
			char16_t highShort = (char16_t) ((*utf8_currentCodeUnit) & 0x1F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Work on the second code unit.
			char16_t lowShort = (char16_t) ((*utf8_currentCodeUnit) & 0x3F);

			//Increment the current code unit pointer to the next code unit
			utf8_currentCodeUnit++;

			//Create the UTF-16 code unit, then increment the iterator
			int unicode = (highShort << 8) | lowShort;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				//Directly set the value to the UTF-16 code unit.
				*utf16_currentCodeUnit = (char16_t) unicode;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 2;
		}
		else if (*utf8_currentCodeUnit < 0xF0) {
			//2048..65535, the remaining Basic Multilingual Plane.

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 1110aaaa 10bbbbcc 10ccdddd
			//Where a is 4th byte, b is 3rd byte, c is 2nd byte, and d is 1st byte.
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

			//Create the resulting UTF-16 code unit, then increment the iterator.
			int unicode = (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;

			//Check to make sure the "unicode" is in the range [0..D7FF] and [E000..FFFF].
			//According to math, UTF-8 encoded "unicode" should always fall within these two ranges.
			if ((0 <= unicode && unicode <= 0xD7FF) || (0xE000 <= unicode && unicode <= 0xFFFF)) {
				//Directly set the value to the UTF-16 code unit.
				*utf16_currentCodeUnit = (char16_t) unicode;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 3;
		}
		else if (*utf8_currentCodeUnit < 0xF8) {
			//65536..10FFFF, the Unicode UTF range

			//Work on the UTF-8 code units one by one.
			//If drawn out, it would be 11110abb 10bbcccc 10ddddee 10eeffff
			//Where a is 6th byte, b is 5th byte, c is 4th byte, and so on.
			char16_t sixthChar = (char16_t) ((*utf8_currentCodeUnit) & 0x4) >> 2;
			char16_t fifthCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t fifthCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t fourthChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;
			char16_t thirdChar = (char16_t) ((*utf8_currentCodeUnit) & 0x3C) >> 2;
			char16_t secondCharHigh = (char16_t) ((*utf8_currentCodeUnit) & 0x3);
			utf8_currentCodeUnit++;
			char16_t secondCharLow = (char16_t) ((*utf8_currentCodeUnit) & 0x30) >> 4;
			char16_t firstChar = (char16_t) ((*utf8_currentCodeUnit) & 0xF);
			utf8_currentCodeUnit++;

			int unicode = (sixthChar << 4) | (fifthCharHigh << 2) | fifthCharLow | (fourthChar << 12) | (thirdChar << 8) | (secondCharHigh << 6) | (secondCharLow << 4) | firstChar;
			char16_t highSurrogate = (unicode - 0x10000) / 0x400 + 0xD800;
			char16_t lowSurrogate = (unicode - 0x10000) % 0x400 + 0xDC00;

			//Set the UTF-16 code units
			*utf16_currentCodeUnit = lowSurrogate;
			utf16_currentCodeUnit++;
			utf16_str_iterator++;

			//Check to see if we're still below the output string size before continuing, otherwise, we cut off here.
			if (utf16_str_iterator < utf16_str_output_size) {
				*utf16_currentCodeUnit = highSurrogate;
				utf16_currentCodeUnit++;
				utf16_str_iterator++;
			}

			//Increment the iterator to keep track of where we are in the UTF-8 string
			utf8_str_iterator += 4;
		}
		else {
			//Invalid UTF-8 code unit, we ignore.
			utf8_currentCodeUnit++;
			utf8_str_iterator++;
		}
	}

	//We clean up the output string if the UTF-16 iterator is still less than the output string size.
	while (utf16_str_iterator < utf16_str_output_size) {
		*utf16_currentCodeUnit = '\0';
		utf16_currentCodeUnit++;
		utf16_str_iterator++;
	}
}


template<typename T>
void out(T str){
    std::cout << str << std::endl;
}

template<typename T1, typename T2>
void out(T1 str1, T2 str2){
    std::cout << str1 << str2 << std::endl;
}

void getbuffer(char* buffer, size_t len){
  for( int i = 0; i < len; i++ ){
    std::cout << std::setbase(16) << std::setw(2) << std::setfill('0')
              << (unsigned)buffer[i];
  }
}

void ProcessSpi(const char* filename, char* buffer){
    FILE * pFile;
    long lSize;
    // char * buffer1;
    size_t result;
    
    pFile = fopen(filename, "rb" );
    if (pFile==NULL){
        fputs("File error",stderr);
        exit (1);
    }
 
    fseek(pFile , 0 , SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);
 
    buffer = (char* )malloc(sizeof(char) * lSize);
    if (buffer == NULL){
        fputs("Memory error",stderr); 
        exit(2);
    }
    // out("size: ", lSize);
    result = fread(buffer, 1, lSize, pFile);
    buffer[lSize] = '\0';
    if (result != lSize){
        fputs ("Reading error",stderr);
        exit(3);
    }
    fclose(pFile);
    // getbuffer(buffer, result);    
    unsigned char* ubuffer = (unsigned char*)malloc(sizeof(char) * result);
    memcpy(ubuffer, buffer, result);

    int a2[2] = {0};
    int a3;
    std::cout << "arg2: " << &a2[1] << ", arg3: " << a2 << ", arg4: " << &a3 << std::endl;
    char* res = (char*)read_maetel_argb(ubuffer, &a2[1], a2, &a3);
    printf("returned pointer %p\n", res);
//   0x4D415444
//   v48 = 4;
//   maetd_config)(v16, 0xC9LL, a2, &v48); 
//   maetd_config)(v16, 0xCALL, a3, &v48); 
//   maetd_config)(v16, 0x19DLL, &v49, &v48);

    // if(maet_init()){
    //     out("[-]maetd_init failed\n");
    //     return;
    // }
    // // int v51 = 1;
    // ull v51 = 1;
    // int64_t res = maetd_create(&v51, 0);
    // if(!res){
    //     out("[-}maetd_create failed\n");
    //     return;
    // }
    // _DWORD* v13 = (_DWORD *)res;
    // unsigned int v17 = maetd_decode((int64_t)v13);
    // if(v17 & 0x80000000){
    //     out("[-]maetd_decode failed\n");
    // }

    return;
}


void Process(char* path, int flag){
    if(flag == 1){
        out("[+]Deal one file");
        char* buffer;
        const char* filename = path;
        ProcessSpi(filename, buffer);
    }
    else if(flag == 2){
        DIR* dir = opendir(path);
        dirent* dirent;
        std::vector<std::string> targets;
        std::string pathname = path;
        if(dir){
            while((dirent = readdir(dir)) != NULL){
                std::string filename = dirent->d_name;
                if(filename != "." && filename != "..")
                    targets.push_back(pathname + filename);
            }
        }
        for(int i = 0; i < targets.size(); i++){
            const char* filename = targets[i].c_str();
            out("[+]Dealing file: ", filename);
            char* buffer;
            ProcessSpi(filename, buffer);
        }
    }
}

void ProcessFile(char* path)
{
    char spenstring[16];
    char16_t buffer[256];
    memset(buffer, 0, sizeof(buffer));
    utf8_to_utf16((unsigned char*)path, strlen(path), buffer, 256);
    int len = strlen(path);
    void* spen = _ZN4SPen6StringC2Ev(spenstring);
    int ret = _ZN4SPen6String9ConstructEPKti(spen, (char*)buffer, len);
    if((ret & 1 ) != 0)
    {
        int end = 0;
        int res[2];
        res[0] = 0;
        res[1] = 0;
        printf("parsed %p\n", _Z16read_maetel_argbPKN4SPen6StringEPiS3_S3_(spen, &res[1], res, &end));
    }
}

int main(int argc, char** argv){

    if(argc != 3){
        printf("Usage: %s <choice> <seedspath>\nChoice: -a -o -f\n", argv[0]);
        return 0;
    }
    const char *const short_opts = "a:o:f:";
    int opt;
    while((opt = getopt(argc, argv, short_opts)) != -1){
        if(opt == 'a'){
            Process(argv[2], 2);
        }else if(opt == 'o'){
            Process(argv[2], 1);
        }
        else if(opt == 'f'){
            ProcessFile(argv[2]);
        }
    }
    return 0;
    
}
