#ifndef _CICVLOG_H_
#define _CICVLOG_H_

#include <string>
#include <fstream>
#include <iostream>

namespace cicvlog
{
    // bracket
    #define LEFT_BRACKET "["
    #define RIGHT_BRACKET "] "
    // STRING of the TAGs
    #define INFO_STR    "INFO   "
    #define WARNING_STR "WARNING"
    #define ERROR_STR   "ERROR  "
    #define FATAL_STR   "FATAL  "
    // output message
    #define OUT_MES     std::to_string(stime_->tm_year+1900) + "/" + std::to_string(stime_->tm_mon+1)\
                        + "/" + std::to_string(stime_->tm_mday) + " |" + std::to_string(stime_->tm_hour)\
                        + ":" + std::to_string(stime_->tm_min) + ":" + std::to_string(stime_->tm_sec) + " |"
    // log file config
    #define MAX_LOG_STORE  100000
    #define MAX_LOG_DAYS   100000

    typedef enum{
        INFO,
        WARNING,
        ERROR,
        FATAL,
    } TAG_TYPE_E;


    class CicvLog
    {
    public:
        CicvLog(const std::string file_header);
        ~CicvLog();

        // function way
        std::string CreatLog(std::string message, TAG_TYPE_E tag_type);
        void LogPrint(std::string log);
        void LogFile(std::string log);

        // stream way(if file is open,to file;or to cout)
        std::ostream& LogStream(TAG_TYPE_E tag_type);
        std::string log_check_date_;
        // log file config
        void SetMaxLogStore(long long max_log_store);
        void SetMaxLogDays(long long max_log_days);

    private:
        void CreateLogFile();

        long long GetCurLogStore();
        std::string GetCurDateStr();

        long long CalDateDiff(std::string cur_date, std::string before_date);
        void CheckFileDate();
        void getFiles(const std::string path, std::vector<std::string> &files);
        
        // log date check flag
        

        // log file
        std::string file_name_;
        std::ofstream log_file_;

        // get current time
        time_t t_ = time(NULL);
        struct tm *stime_ = localtime(&t_);

        // config for log file
        long long max_log_store_  = MAX_LOG_STORE;
        long long cur_log_store_  = 0; // kBytes
        long long max_log_days_   = MAX_LOG_DAYS;
    };/* class CicvLog */

} /* namespace cicvlog */

#endif