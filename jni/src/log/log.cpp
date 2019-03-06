#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <time.h>

#include <string>
#include <chrono>

#include <dirent.h> 
#include <sys/statfs.h>  
#include <sys/stat.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <errno.h>


#define DEBUG_BUFFER_MAX_LENGTH (1024)
#ifndef LOG_TAG
#define  LOG_TAG  "ANDROID_TEST"
#endif

static std::string getTimeString()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = { 0 };
    sprintf(date, "%d-%02d-%02d_%02d-%02d-%02d",
        (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
        (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

/*****************************************************************************
 * �� �� ��  : zBaseUtil.getMilliSecond
 * �� �� ��  : ������
 * ��������  : 2015��12��03��
 * ��������  : ��ȡ����ֵ -- 1/1000��
 * �������  : void
 * �������  : ��
 * �� �� ֵ  : std
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
static std::int64_t getMilliSecond(void)
{
    struct timespec ts = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (std::int64_t)ts.tv_sec * 1000 + ts.tv_nsec/1000000;
}

/*****************************************************************************
 * �� �� ��  : zLog.zLogPrint_debug
 * �� �� ��  : ������
 * ��������  : 2015��11��28��
 * ��������  : ���Դ�ӡ
 * �������  : const char *filename  �ļ���
               const char *func      ������
               const char *format    ��ʽ
               ...                   ����
 * �������  : ��
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 

*****************************************************************************/
int zLogPrint_debug(const char *filename, const char *func, int line, const char *format, ...)
{
    char buffer[DEBUG_BUFFER_MAX_LENGTH + 1] = { 0 };

    // filename example: D:/workspace/develop/Software/source/md_noScreen/jni/../zVideoThread.cpp
    char * sub_filename =  strrchr(filename, '/');
    int size = snprintf(buffer, DEBUG_BUFFER_MAX_LENGTH, "%s, %lld ms, %s, %s, %d: ", 
        getTimeString().c_str(), 
        (getMilliSecond()),
        (NULL != sub_filename ? (sub_filename + 1) : filename), 
        (NULL != func ? func : ""), line);

    va_list arg;
    va_start(arg, format);
    (void)vsnprintf(buffer + size, DEBUG_BUFFER_MAX_LENGTH - size, format, arg);
    va_end(arg);

    //printf("%s", buffer);
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s\n", buffer);
    return 0;
}

