# Briefing

This report contains 11 distinct DNG samples from fuzzing outputs that cause different memory corruption sites in libimagecodec.quram, triggerable in Gallery.

# Analysis

QuramCodec supports DNG file parsing, most obvious of which is in the Gallery application.

There are two ways Gallery parses DNG with QuramCodec:

```java
    public static Bitmap decodeFile(String arg6, BitmapFactory.Options arg7, boolean arg8) {
        Bitmap bitmap;
        ThreadUtil.assertOnMainThread("ImageDecoder should run on background thread");
        try {
            Trace.beginSection("decodeFile");
            bitmap = null;
            if(!arg7.inJustDecodeBounds) {
                if((Features.isEnabled(Features.IS_POS)) && (ImageDecoder.USE_BITMAP_POOL)) {
                    BitmapUtils.applyBitmapPool(arg7);
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
                    if(bitmap == null) {
                        bitmap = QuramBitmapFactory.decodeFile(arg6, arg7);
                    }
                }
            }

```

1) for filename ending with DNG and whose `make` field is `Samsung` and `checkIsDNGFile` passes, goes to `decodeDNGPreview` and then calls into QuramCodec
2) For filename not ending with DNG, goes to `QuramBitmapFactory.decodeFile`. If the file extension is not DNG but `QuramGetImageInfoFromFile2` decided it's a RAW format, it will treat the file as DNG file and finally calls  `QrDecodeDNGFile`.
Which means that even if the filename ends with JPEG, it may still be treated and parsed as DNG file, there's different parsing route for the two parsing situations.

I conducted a fuzzing campaign on this file format and preliminary collected 11 distinct samples in the crashes with different corruption sites after de-duplication, as listed in the following table.

The corruption sites are decided by either QEMU-AddressSanitizer or libdislocator because I'm doing binary fuzzing. 

I wrote a command-line loader that mimics the behavior when parsing image files as reverse engineered from Gallery and libimagecodec.quram.so, source and binary of which is in the attachment zip file.

# Reproduce
There are two ways to reproduce these vulnerabilities, reproducing directly in Gallery or reproduce use command-line parser. 

Note that when reproducing in Gallery, some bugs’ corruptions may not directly lead to crash, e.g. minor overflows. Also, due to different memory impact, some crash sites may differ between runs or crashes in different functions. Command-line reproduction with ASAN/libdislocator/Qemu-ASAN is more straightforward.

I’ve attached all behaviors of provided 11 samples under the following table:

| CaseName |FaultAddr(ASAN or dislocator)  | Crash in Gallery? | Crash in Loader? | Crash with ASAN(QASAN,dislocator)? | Extensions  | Type |
| --- | --- | --- | --- | --- | --- | --- |
| buf-overflow-0x218a44-QuramDngLinearizationInfoSparse | 0x218a44 | Yes | Yes | Yes | DNG,JPEG |OOB write of size 2|
| buf-overflow-0x218c30-QuramDngLinearizationInfo5parse | 0x218c30 | No | No | Yes |DNG, JPEG |OOB write of size 8 |
| buf-overflow-0x218c7c-QuramDngLinearizationInfo5parse | 0x218c7c | Yes | Yes | Yes | DNG,JPEG | OOB write of size 8 |
|buf-overflow-0x218d34-doRoundBlacks  | 0x218d34 | Yes |Yes  |Yes  | DNG,JPEG |OOB Read of size 8 |
|buf-overflow-0x218d98-doRoundBlacks  | 0x218d98 | Yes | Yes | Yes | DNG,JPEG | OOB Read of size 8 |
|buf-overflow-0x231468-DoBaseLineABCToRGB  | 0x231468 (\*) | Yes | Yes | Yes | DNG,JPEG(only JPEG crashes gallery) |OOB Read of size 8 |
|buf-overflow-0x231db4-DoBaseLineRGBtoRGB  |0x231db4 (\*)| Yes  | Yes | Yes | DNG,JPEG(only JPEG crashes gallery) |OOB Write of size 8 |
|buf-overflow-0x242f44-Simd4Neo21  | 0x242f44(\*\*) | No | No | Yes | JPEG |OOB Read of size 8|
|buf-overflow-0x242f58-SimdNeonBgrToBgr  | 0x242f58(\*\*) | No  | No | Yes | JPEG |OOB Read of size 8|
| buf-overflow-0xbf720-rgba888toyuv420 | 0xbf720 | No | No | Yes | JPEG | OOB Write of size 1 |
| use-after-free | \* | No | No | Yes | JPEG | use-after-free(free canary corrupted) |

\
\*: QASAN does not provide accurate faulting address. The address is retrived from loader SIGSEGV log (if any). use-after-free bugs currently does not have faulting address.
\*\*: QASAN does not provide accurate faulting address. The address is treived from dislocator loader SIGSEGV log.

Extensions: extensions means which filename extension will trigger the corresponding bug. This only affects Gallery's behavior. The `loader` does not consider file extensions. e.g. `JPEG` means the bug will be triggered when the testcase is renamed as JPEG.


## Reproduce steps in Gallery
1. Copy the image to some location, e.g. /sdcard/Download
2. Rename the file's extension if needed as listed on the table above
3. Browse the image in Gallery. Gallery will crash
    1. note: if the image is not refresh and listed in Gallery in time, run `adb shell am broadcast -a "android.intent.action.MEDIA_SCANNER_SCAN_FILE" -d "file:///sdcard/Download/"` to force a refresh

## Reproduce steps in cli loader
1. cross compile binary loader
    1. export ANDROID_NDK={NDK_PATH}
    2. export ANDROID_PATH={Samsung system location}, e.g.  export ANDROID_PATH=~/samsung/system, with libimagecodec.so and all other libraries in ~/samsung/system/lib64/
    3. Run `make`, got a binary `loader`
    4. adb push loader /data/local/tmp/

2. On device
    1. ./loader {image_filename}
    2. observer logcat crashes
    3. LD_PRELOAD=./libdislocator64.so ./loader {image_filename} to catch more corruptions

