#include "logger.h"
#include <cstdlib>
#include <ctime>
#include <cstring>
/*
链式编程文件结尾不支持endl，需要使用"\r\n"
*/
std::ofstream Logger::m_error_log_file;
std::ofstream Logger::m_info_log_file;
std::ofstream Logger::m_warn_log_file;

void initLogger(const std::string&info_log_filename,
	const std::string&warn_log_filename,
	const std::string&error_log_filename) {
	Logger::m_info_log_file.open(info_log_filename.c_str(),std::ios::app);//已追加方式写入文件
	Logger::m_warn_log_file.open(warn_log_filename.c_str(), std::ios::app);
	Logger::m_error_log_file.open(error_log_filename.c_str(), std::ios::app);
}

std::ostream& Logger::getStream(log_rank_t log_rank) {
	return (INFO == log_rank) ?
		(m_info_log_file.is_open() ? m_info_log_file : std::cout) :
		(WARNING == log_rank ?
		(m_warn_log_file.is_open() ? m_warn_log_file : std::cerr) :
			(m_error_log_file.is_open() ? m_error_log_file : std::cerr));
}

std::ostream& Logger::start(log_rank_t log_rank,
	const unsigned int line,
	const std::string&function) {
	time_t tm;
	struct tm* t_tm;
	time(&tm);
	t_tm = localtime(&tm);
	char time_string[128];
	sprintf(time_string,"%4d-%02d-%02d %02d:%02d:%02d",t_tm->tm_year+1900,t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);
	//ctime_s(time_string,128,&tm);//windows shiyong
	//ctime_r(&tm,time_string);//linux shiyong
	return getStream(log_rank) << "[ "<<time_string<<" ]"
		<< " function (" << function << ")"
		<< " line " << line<<":\r\n"
		<< std::flush<<std::endl;
}

Logger::~Logger() {
	getStream(m_log_rank) << std::endl << std::flush;

	if (FATAL == m_log_rank) {
		m_info_log_file.close();
		m_info_log_file.close();
		m_info_log_file.close();
		abort();
	}
}
