#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include <cstdio>
#include <vector>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

#include "cicvlog.h"

namespace cicvlog
{
  CicvLog::CicvLog(const std::string file_header)
  {
    if (file_header == "")
    {
      file_name_ = "log_" + GetCurDateStr() +  ".txt";
    }
    else
    {
      file_name_ = file_header + "_" + "log_" + GetCurDateStr() +  ".txt";
    }
    CreateLogFile();
    log_check_date_ = GetCurDateStr();
  }

  CicvLog::~CicvLog()
  {
    log_file_.close();
  }

  void CicvLog::CreateLogFile()
  {
    log_file_.open(file_name_, std::ios::out);

    if (!log_file_.is_open())
    {
      std::cout << "log file create failed!" << std::endl;
    }
    // the heading of the log file
    log_file_ << "   TAG   |   DATE   |  TIME  |  MESSAGE" << std::endl;
  }

  void CicvLog::SetMaxLogStore(long long max_log_store)
  {
    max_log_store_ = max_log_store;// kBytes
  }

  void CicvLog::SetMaxLogDays(long long max_log_days)
  {
    max_log_days_ = max_log_days;
  }

  long long CicvLog::GetCurLogStore()
  {
    long long ret;
    if (log_file_.is_open())
    {
      log_file_.seekp(0,std::ios::end);
      ret = (long long)(log_file_.tellp() / 1024);
    }
    else
    {
      std::cout << "log_file_ cannot open!" << std::endl;
    }
    return ret;
  }
  
  std::string CicvLog::GetCurDateStr()
  {
    std::string ret;
    // year
    ret += std::to_string(stime_->tm_year+1900);
    // month
    if (stime_->tm_mon+1 < 10)
    {
      ret += "0" + std::to_string(stime_->tm_mon+1);
    }
    else
    {
      ret += std::to_string(stime_->tm_mon+1);
    }
    // day
    if (stime_->tm_mday < 10)
    {
      ret += "0" + std::to_string(stime_->tm_mday);
    }
    else
    {
      ret += std::to_string(stime_->tm_mday);
    }
    return  ret;
  }
  
  // calculate the difference(days) between tow date(cur_date is late than before_date!)
  long long CicvLog::CalDateDiff(std::string cur_date, std::string before_date)
  {
    std::vector<int> dayofmonth= {0,31,28,31,30,31,30,31,31,30,31,30,31};
    long long ret = 0;
    long long year1 = atoi(cur_date.substr(0,4).c_str());
    long long year2 = atoi(before_date.substr(0,4).c_str());

    int month1 = atoi(cur_date.substr(4,2).c_str());
    int month2 = atoi(before_date.substr(4,2).c_str());

    int day1 = atoi(cur_date.substr(6,2).c_str());
    int day2 = atoi(before_date.substr(6,2).c_str());
    
    if (year1 > year2)
    {
      ret += dayofmonth[month2] - day2;
      for (int i = month2 + 1;i <= 12;i++)
      {
        ret += dayofmonth[i];
      }
      if (month2<=2)
      {
          if ( ((int)(year2%4) == 0 && year2%100>0) || (int)(year2%400)==0 )
          {
            ret += 1;
          }
      }

      for (int j = year2+1; j < year1;j++)
      {
        ret += 365;
        if ( ((int)(j%4) == 0 && j%100>0) || (int)(j%400)==0 )
        {
          ret += 1;
        }
      }

      ret += day1;
      for (int k = 1; k < month1; k++)
      {
        ret += dayofmonth[k];
      }
      if (month1 >= 2)
      {
        if ( ((int)(year1%4) == 0 && year1%100>0) || (int)(year1%400)==0 )
        {
          ret += 1;
        }
      }
    }
    else// year equal
      {
        if (month1 > month2)
        {
          ret += day1;
          for(int i = month2+1; i<month1; i++)
          {
            ret += dayofmonth[i];
          }
          ret += dayofmonth[month2] - day2;
          if (month1 >= 2 && month2 <= 2)
          {
            if ( ((int)(year1%4) == 0 && year1%100>0) || (int)(year1%400)==0 )
            {
              ret += 1;
            }
          }
        }
        else// month equal
        {
          ret += day1 - day2;
        }
      }

    return ret;
  }

  void CicvLog::getFiles(const std::string path, std::vector<std::string> &files)
  {
    DIR *dir;
    struct dirent *ptr;
    
    if ((dir = opendir(path.c_str())) == NULL)
    {
      perror("Open dir error...");
      return;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
      if (std::strcmp(ptr->d_name, ".") == 0 || std::strcmp(ptr->d_name, "..") == 0) {
        continue;
      } else if (ptr->d_type == 8) {
        files.push_back(ptr->d_name);
      //   printf("file name is %s\n", ptr->d_name);
      } else if (ptr->d_type == 10) {
        continue;
      } else if (ptr->d_type == 4) {
        getFiles(path + ptr->d_name + "/", files);
      }
    }
    closedir(dir);
  }

  void CicvLog::CheckFileDate()
  {
    std::vector<std::string> file_name_vec;
    char Path[255];

    // get current path
    getcwd(Path, sizeof(Path));

    getFiles(Path, file_name_vec);

    for (int i = 0;i < file_name_vec.size();i++)
    {
      if (file_name_vec[i].find("log") != std::string::npos &&\
          file_name_vec[i].find(".txt") != std::string::npos)
      {
        // std::cout << "logfile:" << file_name_vec[i] << std::endl;
        std::string file_date;
        for (int j = 0;j<file_name_vec[i].size();j++)
        {
          if (file_name_vec[i].substr(j,4) == "log_")
          {
            file_date = file_name_vec[i].substr(j+4,8);
            // std::cout << "file_date = " << file_date << std::endl;
            break;
            
          }
        }
        if (CalDateDiff(GetCurDateStr(),file_date) > max_log_days_)
        {
          // std::cout << "remove!" << std::endl;
          remove(file_name_vec[i].c_str());
        }
      }
    }

    log_check_date_ = GetCurDateStr();
  }

  // function way
  std::string CicvLog::CreatLog(std::string message, TAG_TYPE_E tag_type)
  {
    std::string ret="";
    switch (tag_type)
    {
      case TAG_TYPE_E::INFO:
        ret = (std::string)LEFT_BRACKET + (std::string)INFO_STR + (std::string)RIGHT_BRACKET + OUT_MES + message;
        break;
      case TAG_TYPE_E::WARNING:
        ret = (std::string)LEFT_BRACKET + (std::string)WARNING_STR + (std::string)RIGHT_BRACKET + OUT_MES + message;
        break;
      case TAG_TYPE_E::ERROR:
        ret = (std::string)LEFT_BRACKET + (std::string)ERROR_STR + (std::string)RIGHT_BRACKET + OUT_MES + message;
        break;
      case TAG_TYPE_E::FATAL:
        ret = (std::string)LEFT_BRACKET + (std::string)FATAL_STR + (std::string)RIGHT_BRACKET + OUT_MES + message;
        break;
      default:
        break;
    }
    return ret;
  }

  void CicvLog::LogPrint(std::string log)
  {
      std::cout << log << std::endl;
  }

  void CicvLog::LogFile(std::string log)
  {
    if (GetCurDateStr() != log_check_date_)
    {
      CheckFileDate();
    }

    cur_log_store_ = GetCurLogStore();

    if (cur_log_store_ < max_log_store_)
    {
      log_file_ << log << std::endl;
    }
    else
    {
      remove(file_name_.c_str());
      log_file_.close();
      CreateLogFile();
      LogFile(log);
    }
  }
  
  // stream way
  std::ostream& CicvLog::LogStream(TAG_TYPE_E tag_type)
  {
    if (GetCurDateStr() != log_check_date_)
    {
      CheckFileDate();
    }

    cur_log_store_ = GetCurLogStore();
    
    if (cur_log_store_ < max_log_store_)
    {
      switch (tag_type)
      {
        case TAG_TYPE_E::INFO:
          return log_file_ << LEFT_BRACKET << INFO_STR << RIGHT_BRACKET << OUT_MES;
          break;
        case TAG_TYPE_E::WARNING:
          return log_file_ << LEFT_BRACKET << WARNING_STR << RIGHT_BRACKET << OUT_MES;
          break;
        case TAG_TYPE_E::ERROR:
          return log_file_ << LEFT_BRACKET << ERROR_STR << RIGHT_BRACKET << OUT_MES;
          break;
        case TAG_TYPE_E::FATAL:
          return log_file_ << LEFT_BRACKET << FATAL_STR << RIGHT_BRACKET << OUT_MES;
          break;
        default:
          return std::cout;
          break;
      }
    }
    else
    {
      remove(file_name_.c_str());
      log_file_.close();
      CreateLogFile();
      return LogStream(tag_type);
    }
  }

} /* namespace cicvlog */