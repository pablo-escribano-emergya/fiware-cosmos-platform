

#include <stdio.h>              // printf
#include <stdlib.h>             // exit
#include <string.h>             // memcpy
#include <iostream>             // std::cout
#include <time.h>               // strptime, struct tm
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free

#include <unistd.h>             // close, lseek
#include <fcntl.h>              // O_RDONLY
#include <sys/stat.h>           // stat
#include <dirent.h>             // opendir, scandir
#include <errno.h>              // errno
#include <inttypes.h>           // uint64_t etc.
#include <errno.h>              // errno

#include <vector>               // std::vector

#include "logMsg/logMsg.h"      // traces LM_E, LM_W....
#include "LogsDataSet.h" // Own interface


char *fgetsFromFd( char * str, int line_max_size, int fd )
{
  char *p_str = str;

  for (int i = 0; (i < line_max_size); i++, p_str++)
  {
    size_t nbytes_to_read = 1;
    ssize_t bytes_read = 0;

    if ((bytes_read = read(fd, (void *) p_str,  nbytes_to_read)) != (ssize_t)  nbytes_to_read)
    {
      return NULL;
    }
    if (*p_str == '\n')
    {
      *(p_str+1) = '\0';
      return str;
    }
  }
  return NULL;
}

void SplitInWords( char *line , std::vector<char*>& words , char separator )
{
  size_t pos = 0;
  size_t previous = 0;

  bool finish = false;

  // Clear words vector
  words.clear();

  while( !finish )
  {
    if( ( line[pos] == separator ) || ( line[pos] == '\0' ) )
    {
      if(( line[pos] == '\0' )|| (line[pos] == '\n'))
        finish = true;

      // Artificial termination of string
      line[pos] = '\0';

      // Add the found word
      words.push_back(  &line[previous] );

      // Point to the next words
      // Jumps blank spaces
      pos++;
      while (line[pos] == ' ')
      {
        pos++;
      }
      previous = pos;

    }
    else
      pos++;
  }
}

#define  YEAR0   1900
#define  EPOCH_YR   1970
#define  SECS_DAY   (24L * 60L * 60L)
#define  LEAPYEAR(year)   (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define  YEARSIZE(year)   (LEAPYEAR(year) ? 366 : 365)
#define  FIRSTSUNDAY(timp)   (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define  FIRSTDAYOF(timp)   (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define  TIME_MAX   ULONG_MAX
#define  ABB_LEN   3

time_t GetTimeUTCFromCalendar(struct tm *tm)
{
  register long day = 0;
  register long year = EPOCH_YR;
  register int tm_year;
  int yday, month;
  register unsigned long seconds = 0;
  time_t value;
  //int overflow = 0;
  //unsigned dst;

  /* Assume that when day becomes negative, there will certainly
   * be overflow on seconds.
   * The check for overflow needs not to be done for leapyears
   * divisible by 400.
   * The code only works when year (1970) is not a leapyear.
   */

  tm_year = tm->tm_year + YEAR0;

  //if (LONG_MAX / 365 < tm_year - year)
  //  overflow++;
  day = (tm_year - year) * 365;
  //        if (LONG_MAX - day < (tm_year - year) / 4 + 1)
  //          overflow++;
  day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
  day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
  day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

  yday = month = 0;
  switch (tm->tm_mon)
  {
  case 0:
    break;
  case 1:
    yday += 31;
    break;
  case 2:
    yday += 59 + LEAPYEAR(tm_year);
    break;
  case 3:
    yday += 90 + LEAPYEAR(tm_year);
    break;
  case 4:
    yday += 120 + LEAPYEAR(tm_year);
    break;
  case 5:
    yday += 151 + LEAPYEAR(tm_year);
    break;
  case 6:
    yday += 181 + LEAPYEAR(tm_year);
    break;
  case 7:
    yday += 212 + LEAPYEAR(tm_year);
    break;
  case 8:
    yday += 243 + LEAPYEAR(tm_year);
    break;
  case 9:
    yday += 273 + LEAPYEAR(tm_year);
    break;
  case 10:
    yday += 304 + LEAPYEAR(tm_year);
    break;
  case 11:
    yday += 334 + LEAPYEAR(tm_year);
    break;
  }

  yday += (tm->tm_mday - 1);
  //        if (day + yday < 0)
  //          overflow++;
  day += yday;

  tm->tm_yday = yday;
  tm->tm_wday = (day + 4) % 7; /* day 0 was thursday (4) */

  seconds = ((tm->tm_hour * 60L) + tm->tm_min) * 60L + tm->tm_sec;

  //        if ((TIME_MAX - seconds) / SECS_DAY < day)
  //          overflow++;
  seconds += day * SECS_DAY;

  /* Now adjust according to timezone and daylight saving time */

  /* Not for us */
#ifdef ELIMINADO

  if (((_timezone > 0) && (TIME_MAX - _timezone < seconds))
      || ((_timezone < 0) && (seconds < -_timezone)))
    overflow++;
  seconds += _timezone;

  if (tm->tm_isdst < 0)
    dst = _dstget(tm);
  else if (tm->tm_isdst)
    dst = _dst_off;
  else
    dst = 0;

  if (dst > seconds)
    overflow++; // dst is always non-negative
  seconds -= dst;

  if (overflow)
  {
    value = (time_t) -1;
    return value;
  }
#endif /* de ELIMINADO */

  if ((time_t) seconds != (signed) seconds)
  {
    value = (time_t) - 1;
  }
  else
  {
    value = (time_t) seconds;
  }
  return value;
}


time_t GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(const char *strTimeDate)
{
  struct tm tm;
  const char *p_month;
  time_t value;

#define Char_to_int(x) ((x)-48)

  if ((strchr(strTimeDate, '/') != NULL) || (strchr(strTimeDate, '-') != NULL))
  {
    // DD/MMM/YY
    tm.tm_year = 100 + Char_to_int( strTimeDate[7] ) * 10 + Char_to_int( strTimeDate[8] );
    p_month = &(strTimeDate[3]);
    tm.tm_mday = Char_to_int( strTimeDate[0] ) * 10 + Char_to_int( strTimeDate[1] );
  }
  else
  {
    // YYYYMMMDD
    tm.tm_year = 100 + Char_to_int( strTimeDate[2] ) * 10 + Char_to_int( strTimeDate[3] );
    p_month = &(strTimeDate[4]);
    tm.tm_mday = Char_to_int( strTimeDate[7] ) * 10 + Char_to_int( strTimeDate[8] );
  }
  tm.tm_mon = 12;
  if (strncmp(p_month, "JAN", strlen("JAN")) == 0)
  {
    tm.tm_mon = 0;
  }
  else if (strncmp(p_month, "FEB", strlen("FEB")) == 0)
  {
    tm.tm_mon = 1;
  }
  else if (strncmp(p_month, "MAR", strlen("MAR")) == 0)
  {
    tm.tm_mon = 2;
  }
  else if (strncmp(p_month, "APR", strlen("APR")) == 0)
  {
    tm.tm_mon = 3;
  }
  else if (strncmp(p_month, "MAY", strlen("MAY")) == 0)
  {
    tm.tm_mon = 4;
  }
  else if (strncmp(p_month, "JUN", strlen("JUN")) == 0)
  {
    tm.tm_mon = 5;
  }
  else if (strncmp(p_month, "JUL", strlen("JUL")) == 0)
  {
    tm.tm_mon = 6;
  }
  else if (strncmp(p_month, "AUG", strlen("AUG")) == 0)
  {
    tm.tm_mon = 7;
  }
  else if (strncmp(p_month, "SEP", strlen("SEP")) == 0)
  {
    tm.tm_mon = 8;
  }
  else if (strncmp(p_month, "OCT", strlen("OCT")) == 0)
  {
    tm.tm_mon = 9;
  }
  else if (strncmp(p_month, "NOV", strlen("NOV")) == 0)
  {
    tm.tm_mon = 10;
  }
  else if (strncmp(p_month, "DEC", strlen("DEC")) == 0)
  {
    tm.tm_mon = 11;
  }

  tm.tm_hour = Char_to_int(strTimeDate[10]) * 10 + Char_to_int(strTimeDate[11]);
  tm.tm_min = Char_to_int(strTimeDate[13]) * 10 + Char_to_int(strTimeDate[14]);
  tm.tm_sec = Char_to_int(strTimeDate[16]) * 10 + Char_to_int(strTimeDate[17]);

  //change hour from AM/PM to 24H
  const char *am_pm = &(strTimeDate[26]);
  if ((strncmp(am_pm, "pm", strlen("pm")) == 0) || (strncmp(am_pm, "PM", strlen("PM")) == 0))
  {
    tm.tm_hour += 12;
  }

#undef Char_to_int
  value = GetTimeUTCFromCalendar(&tm);
  return value;
}

time_t GetTimeFromStrTimeDate_YYYY_mm_dd_24H(const char *strTimeDate)
{
  struct tm tm;
  time_t value;

#define Char_to_int(x) ((x)-48)
  // YYYY_mm-dd
  tm.tm_year = 100 + Char_to_int( strTimeDate[2] ) * 10 + Char_to_int( strTimeDate[3] );
  tm.tm_mon = Char_to_int( strTimeDate[5] ) * 10 + Char_to_int( strTimeDate[6] ) - 1;
  tm.tm_mday = Char_to_int( strTimeDate[8] ) * 10 + Char_to_int( strTimeDate[9] );

  tm.tm_hour = Char_to_int(strTimeDate[11]) * 10 + Char_to_int(strTimeDate[12]);
  tm.tm_min = Char_to_int(strTimeDate[14]) * 10 + Char_to_int(strTimeDate[15]);
  tm.tm_sec = Char_to_int(strTimeDate[17]) * 10 + Char_to_int(strTimeDate[18]);

#undef Char_to_int
  value = GetTimeUTCFromCalendar(&tm);
  return value;
}


LogsDataSet::LogsDataSet(const char *dir_path, const char *extension, int timestamp_position, int timestamp_type, const char *queue_name)
{
  dir_path_ = strdup(dir_path);
  extension_ = strdup(extension);
  timestamp_position_ = timestamp_position;
  timestamp_type_ = timestamp_type;
  first_timestamp_ = -1;
  queue_name_ = strdup(queue_name);
}

LogsDataSet::~LogsDataSet()
{
  free(dir_path_);
  free(extension_);
  free(queue_name_);
}

bool LogsDataSet::InitDir()
{
  if (chdir(dir_path_) == -1)
  {
    LM_E(("Error changing to directory '%s': %s", dir_path_, strerror(errno)));
    return false;
  }

  DIR * dir = opendir(".");
  if (dir == NULL)
  {
    LM_E(("opendir(%s): %s", dir_path_, strerror(errno)));
    return false;
  }


  //
  // Count number of files in the directory
  //
  num_files_ = 0;
  struct dirent*  entry = NULL;

  while ((entry = readdir(dir)) != NULL)
  {
    const char* suff;

    if (entry->d_name[0] == '.')
      continue;

    suff = strrchr(entry->d_name, '.');
    if (suff == NULL)
      continue;
    ++suff;
    if (strcmp(suff, extension_) != 0)
      continue;

    ++num_files_;
  }
  LM_V(("Initially found %d files in '%s'", num_files_, dir_path_));

  //
  // Create a vector to hold the files
  //
  file_vector_ = (File*) calloc(num_files_, sizeof(File));
  if (file_vector_ == NULL)
  {
    LM_E(("error allocating vector for %d files: %s", num_files_, strerror(errno)));
    closedir(dir);
    return false;
  }


  //
  // Gather info on the files and fill the vector
  //
  rewinddir(dir);
  int ix = 0;
  while ((entry = readdir(dir)) != NULL)
  {
    char*        suff;
    struct stat  statBuf;

    if (entry->d_name[0] == '.')
      continue;

    suff = strrchr(entry->d_name, '.');
    if (suff == NULL)
      continue;
    ++suff;
    if (strcmp(suff, extension_) != 0)
      continue;

    if (stat(entry->d_name, &statBuf) == -1)
    {
      LM_E(("stat(%s): %s", entry->d_name, strerror(errno)));
      continue;
    }

    if (!S_ISREG(statBuf.st_mode))
      continue;

    if (strlen(entry->d_name) > sizeof(file_vector_[ix].name))
    {
      LM_E(("File name too long: '%s'", entry->d_name));
      continue;
    }

    LM_V2(("Adding file '%s'", entry->d_name));

    strcpy(file_vector_[ix].name, entry->d_name);
    file_vector_[ix].date         = statBuf.st_mtime;
    file_vector_[ix].size         = statBuf.st_size;
    file_vector_[ix].fd           = -1;
    file_vector_[ix].already_read = false;

    ++ix;
  }
  closedir(dir);

  num_files_ = ix;
  LM_V(("%d files in vector", num_files_));


  //
  // Sort the vector in name order (smallest first)
  //
  int    first;
  int    second;
  int    smallest_index = -1;
  File*  smallest;
  File   copy;

  for (first = 0; first < num_files_ - 1; first++)
  {
    smallest = &file_vector_[first];

    for (second = first + 1; second < num_files_; second++)
    {
      if (strcmp(file_vector_[second].name, smallest->name) < 0)
      {
        smallest = &file_vector_[second];
        smallest_index = second;
      }
    }
    LM_V(("%05d: '%s'", first, smallest->name));

    // Swap
    if (smallest != &file_vector_[first])
    {
      LM_VV(("Copying smallest from index %d to %d", smallest_index, first));
      memcpy(&copy, &file_vector_[first], sizeof(copy));
      memcpy(&file_vector_[first], smallest, sizeof(file_vector_[first]));
      memcpy(smallest, &copy, sizeof(copy));
    }
  }
  return true;
}

bool LogsDataSet::GetLogLineEntry(char **log, time_t *timestamp)
{
#define LOGSDATASET_LINE_MAX_LENGTH 1024
  char temporal_buffer[LOGSDATASET_LINE_MAX_LENGTH + 1];

  for (int file_index = 0; file_index < num_files_; file_index++)
  {
    if (file_vector_[file_index].already_read == true)
    {
      continue;
    }

    if (file_vector_[file_index].fd == -1)
    {
      if ((file_vector_[file_index].fd = open(file_vector_[file_index].name, O_RDONLY)) == -1)
      {
        LM_E(("Error opening file:'%s' (%d of %d), errno:%s %d", file_vector_[file_index].name, file_index, num_files_, strerror(errno), errno));
        file_vector_[file_index].already_read = true;
        continue;
      }
    }

    if ( fgetsFromFd (temporal_buffer , LOGSDATASET_LINE_MAX_LENGTH , file_vector_[file_index].fd) != NULL )
    {
      *log = strdup(temporal_buffer);
      if (timestamp_position_ != -1)
      {
        std::vector<char*> fields;

        SplitInWords(temporal_buffer , fields , separator_);

        if (timestamp_type_ == 1)
        {
          *timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(fields[timestamp_position_]);
        }
        else if (timestamp_type_ == 1)
        {
          *timestamp = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[timestamp_position_]);
          if (first_timestamp_ == -1)
          {
            first_timestamp_ = *timestamp;
          }
        }
      }
      else
      {
        *timestamp = -1;
      }
      return true;
    }
    else
    {
    close(file_vector_[file_index].fd);
      file_vector_[file_index].fd = -1;
      file_vector_[file_index].already_read = true;
    }

  }
  LM_W(("End of files in directory '%s' reached after scanning %d valid files", dir_path_, num_files_));
  return false;
}


bool LogsDataSet::LookAtNextLogLineEntry(char **log, time_t *timestamp)
{
#define LOGSDATASET_LINE_MAX_LENGTH 1024
  char temporal_buffer[LOGSDATASET_LINE_MAX_LENGTH + 1];

  for (int file_index = 0; file_index < num_files_; file_index++)
  {
    if (file_vector_[file_index].already_read == true)
    {
      continue;
    }

    if (file_vector_[file_index].fd == -1)
    {
      if ((file_vector_[file_index].fd = open(file_vector_[file_index].name, O_RDONLY)) == -1)
      {
        LM_E(("Error opening file:'%s' (%d of %d), errno:%s %d", file_vector_[file_index].name, file_index, num_files_, strerror(errno), errno));
        file_vector_[file_index].already_read = true;
        continue;
      }
    }


    if ( fgetsFromFd (temporal_buffer , LOGSDATASET_LINE_MAX_LENGTH , file_vector_[file_index].fd) != NULL )
    {
      if ((lseek(file_vector_[file_index].fd, -1 * strlen(temporal_buffer), SEEK_CUR)) == -1)
      {
        LM_E(("Error rewinding file descriptor for file '%s'", file_vector_[file_index].name));
      }
      *log = strdup(temporal_buffer);
      if (timestamp_position_ != -1)
      {
        std::vector<char*> fields;

        SplitInWords(temporal_buffer , fields , separator_);

        if (timestamp_type_ == 1)
        {
          *timestamp = GetTimeFromStrTimeDate_YYYY_mm_dd_24H(fields[timestamp_position_]);
        }
        else if (timestamp_type_ == 1)
        {
          *timestamp = GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(fields[timestamp_position_]);
          if (first_timestamp_ == -1)
          {
            first_timestamp_ = *timestamp;
          }
        }
      }
      else
      {
        *timestamp = -1;
      }
      return true;
    }
    else
    {
    close(file_vector_[file_index].fd);
      file_vector_[file_index].fd = -1;
      file_vector_[file_index].already_read = true;
    }

  }
  LM_W(("End of files in directory '%s' reached after scanning %d valid files", dir_path_, num_files_));
  return false;
}

