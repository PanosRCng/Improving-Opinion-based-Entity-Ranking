#include "fileMapIO.h"
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <iostream>
#include <stdlib.h> 

using namespace std;

typedef map<string,int> stringMap;

bool FileMapIO::sentiMapToFile(string filename, map<string,double> fileMap)
{
	ofstream ofile;
	ofile.open(filename.c_str());
	if(!ofile)
	{
		return false;			//file does not exist and cannot be created.
	}
	for(map<string,double>::const_iterator iter= fileMap.begin(); iter!=fileMap.end(); ++iter)
	{
		ofile<<iter->first<<"|"<<iter->second;
		ofile << std::endl;
	}
	return true;
}

bool FileMapIO::mapToFile(string filename, stringMap fileMap)		//Write Map
{
	ofstream ofile;
	ofile.open(filename.c_str());
	if(!ofile)
	{
		return false;			//file does not exist and cannot be created.
	}
	for(stringMap::const_iterator iter= fileMap.begin(); iter!=fileMap.end(); ++iter)
	{
		ofile<<iter->first<<"|"<<iter->second;
		ofile << std::endl;
	}
	return true;
}

std::map<std::string,double> FileMapIO::fileToSentiMap(std::string filename) 
{
	std::map<std::string,double> fileMap;

	unsigned int npos = 4294967295;

	ifstream ifile(filename.c_str());

	if (ifile.is_open())
	{
		string line;
		string key;
		vector<string> v_str;

		while ( ifile.good() )
    		{
      			getline (ifile,line);

//	ifstream ifile;
//	ifile.open(filename.c_str());
//	if(!ifile)
//		return fileMap;  	
//	string line;
//	string key;
//	vector<string> v_str;
//	while(ifile>>line)
//	{
			int pos = line.find("|");

			string term = line.substr(0,pos);

			string polarity = line.substr(pos+1, npos);

			fileMap.insert( pair<string,double>( term, strtod(polarity.c_str(),NULL) ) );
		}
	}
	
	return fileMap;
}

std::map<std::string,int> FileMapIO::fileToMap(std::string filename)  //Read Map
{
	std::map<std::string,int> fileMap;

	ifstream ifile;
	ifile.open(filename.c_str());
	if(!ifile)
		return fileMap;  	
	string line;
	string key;
	vector<string> v_str;
	while(ifile>>line)
	{
		int pos = line.find("|");

		string term = line.substr(0,pos);

		string tf = line.substr(pos+1,string::npos);

		fileMap.insert( pair<string,int>( term, atoi(tf.c_str()) ) );
	}
	
	return fileMap;
}




