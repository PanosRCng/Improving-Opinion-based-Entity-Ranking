#ifndef FileMapIO_H
#define FileMapIO_H

#include <map>
#include <string>

namespace FileMapIO
{
	bool mapToFile(std::string, std::map<std::string,int>);
	std::map<std::string,int> fileToMap(std::string);
	bool sentiMapToFile(std::string, std::map<std::string,double>);
	std::map<std::string,double> fileToSentiMap(std::string);
}

#endif




