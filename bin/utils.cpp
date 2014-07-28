#include <iostream>
#include "utils.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <cstdio>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <math.h>
#include <cmath>
#include "fileMapIO.h"

using namespace std;

Utils::Utils()
{
	//
}

Utils::~Utils()
{
	//
}

// get the files of a directory as a vector of strings
int Utils::getFilesList(string dir, vector<string> &files, bool fullPath)
{
	DIR *dp;
	struct dirent *dirp;

	if((dp  = opendir(dir.c_str())) == NULL)
	{
	        cout << "Error(" << errno << ") opening " << dir << endl;
	        return errno;
    	}

	while((dirp = readdir(dp)) != NULL)
	{
		string dir_name = string(dirp->d_name);

		if( (dir_name != ".") && (dir_name != "..") )
		{
			if( fullPath == true )
			{
				files.push_back(dir + dir_name);
			}
			else
			{
        			files.push_back(dir_name);
			}
		}
    	}

    	closedir(dp);

    	return 0;
}

// delete all files in a directory 
int Utils::deleteFiles(string dir_path)
{
	string dir = string(dir_path+"/");
	vector<string> files = vector<string>();

	getFilesList(dir,files, false);

	for (unsigned int i = 0; i < files.size(); i++)
	{
		string filename = dir_path + "/" + string(files[i]);

		remove(filename.c_str());
	}
	
	return 0;
}

// parse reviews
int Utils::parse_reviews(string src_review_dir, string dst_review_dir)
{
	// get the list of the reviews to process
	vector<string> files = vector<string>();
	getFilesList(src_review_dir, files, false);

	for(int i=0; i<files.size(); i++)
	{
		parse_review( src_review_dir + files.at(i), dst_review_dir + files.at(i) );
	}

	return 0;
}

// parse a review
int Utils::parse_review(string src_review_path, string dst_review_path)
{
	unsigned int npos = 4294967295;

	string line;

	ofstream output (dst_review_path.c_str());

	if (output.is_open())
	{
		ifstream input (src_review_path.c_str());

		if (input.is_open())
		{
			while ( input.good() )
			{
				getline(input,line);

				string paragraph = "";

				unsigned start_text = line.find("<TEXT>");
				unsigned end_text = line.find("</TEXT>");

				if(start_text != npos)
				{
					list<string> sentences;			// get paragraph and slit to sentences
					paragraph = line.substr(start_text+6, (end_text-start_text-6)); 
 					sentences = getSentences(paragraph);
  					for(list<string>::iterator it = sentences.begin(); it != sentences.end(); it++)
					{
					    output << *it << endl;
					}
				}

				unsigned start_favorite = line.find("<FAVORITE>");
				unsigned end_favorite = line.find("</FAVORITE>");

				if(start_favorite != npos)
				{
					list<string> sentences;			 // get paragraph and split to sentences
					paragraph = line.substr(start_favorite+10, (end_favorite-start_favorite-10)); 
 					sentences = getSentences(paragraph);
  					for(list<string>::iterator it = sentences.begin(); it != sentences.end(); it++)
					{
					    output << *it << endl;
					}
				}
    			}

			input.close();
  		}
  		else
		{
			cout << "Unable to open review file for read:" << src_review_path << endl; 
		}

		output.close();
	}
	else
	{
		cout << "Unable to open file to write review " << dst_review_path << endl;
	}

	return 0;
}

// get sentences from paragraph, splits sentences using the symbols "." and "!"
list<string> Utils::getSentences(string paragraph)
{
	list<string> sentences;

	unsigned start_sentence = 0;
	unsigned end_sentence = 0;

	do
	{
		string sentence= "";
		
		unsigned end_sentence_dot = paragraph.find(".", start_sentence,1);
		unsigned end_sentence_owau = paragraph.find("!", start_sentence,1);	

		if(end_sentence_dot < end_sentence_owau)
		{
			end_sentence = end_sentence_dot;
		}
		else
		{
			end_sentence = end_sentence_owau;
		}

		if(start_sentence != string::npos)
		{
			sentence = paragraph.substr(start_sentence, (end_sentence-start_sentence)); 

			if( (sentence.length() > 0) && (sentence.compare(" ")) )
			{
				sentences.push_back(sentence);
			}
		}
		
		start_sentence = end_sentence+1;

	}while( end_sentence < paragraph.size());

	return sentences;
}

// breaks a query (term1 term2, term3, term4 term5) to terms and returns them as a vector of strings
vector<string> Utils::getQueryVec(string f_query)
{
	vector<string> query_vec;

	vector<string> q_vec;
	boost::split(q_vec, f_query, boost::is_any_of(","));

	for(int i=0; i<q_vec.size(); i++)
	{
		vector<string> p_vec;
		boost::split(p_vec, q_vec[i], boost::is_any_of(" "));

		for(int j=0; j<p_vec.size(); j++)
		{
			query_vec.push_back(p_vec[j]);
		}
	}

	return query_vec;
}

// makes a query string from a vector of string terms (term1_term2_term3_term4_term5)
string Utils::getQueryStr(vector<string> query_vec)
{
	string query_str="";

	for(int i=0; i<query_vec.size(); i++)
	{
		query_str = query_str + query_vec[i] + "_";
	}

	return query_str;
}

// calculates the cosine similarity of documents a and b
double Utils::getSimilarity(string a, string b)
{
	double sim = 0.0;

	// loads a's feature
	map<string,double> feature_a;
	feature_a = FileMapIO::fileToSentiMap(a);
	vector<double> feature_vector_a;
	for (map< string,double >::iterator it_a=feature_a.begin(); it_a!=feature_a.end(); ++it_a)
	{
		feature_vector_a.push_back(it_a->second);
	}

	// load b's feature
	map<string,double> feature_b;
	feature_b = FileMapIO::fileToSentiMap(b);
	vector<double> feature_vector_b; 
	for (map< string,double >::iterator it_b=feature_b.begin(); it_b!=feature_b.end(); ++it_b)
	{
		feature_vector_b.push_back(it_b->second);
	}

	// get the cosine similarity between document a and b
	sim = cosineSimilarity(feature_vector_a, feature_vector_b);

	return sim;
}

// calculates the cosine similarity of documents a and b
double Utils::cosineSimilarity( vector<double> a, vector<double> b)
{
	double similarity=0.0;
	double product = 0.0;

	if( (eucl_norm(a)!=0) && (eucl_norm(b)!=0) )
	{
		similarity = dot_product(a, b) / ( eucl_norm(a) * eucl_norm(b) );
	}	

	return similarity;
}

// calculates the dot product of vectors a and b
double Utils::dot_product(vector<double> a, vector<double> b)
{
	double product = 0.0;

	if( a.size() != b.size() )
	{
		cout << "vectors do not have the same size" << endl;

		return 0.0;
	}

	for(int i=0; i<a.size(); i++)
	{
		product += a[i] * b[i];
	}	

	return product;
}

// calculates the euclidean norm of vector a 
// (!) improve here - make sure that the members of the vector are bound -- not a big problem -- vectors are identical
double Utils::eucl_norm(vector<double> a)
{
	double norm_v = 0.0;

	for(int i=0; i<a.size(); i++)
	{
		norm_v += pow(a[i],2);
	}

	norm_v = sqrt(norm_v);

	return norm_v;
}

// returns the lines of a text file as a vector of strings
vector<string> Utils::getLines(string file_path)
{
	vector<string> lines;

	ifstream myfile(file_path.c_str());

	if (myfile.is_open())
	{
		string line;

		while ( myfile.good() )
    		{
      			getline (myfile,line);

		    	if(line.size()>0)
		    	{
				lines.push_back(line);
		    	}
		    	else
		    	{
				continue;
		    	}
		}
	}

	return lines;
}

// base 2 logarithm of n
double Utils::Log2( double n )
{
    // log(n)/log(2) is log2
    return log( n ) / log( 2 );
}


vector<string> Utils::getCL(string category)
{
	vector<string> CL = vector<string>();

	string dir_dinf = string("/home/panos/Desktop/Opinion_MIning_and_Sentiment_Analysis/project/D_inf/"+category+"/");

	getFilesList(dir_dinf,CL, false);

	return CL;
}

vector<string> Utils::getCL2(vector< boost::tuple<string, string> > *judge)
{
	vector<string> CL2;

	for(vector< boost::tuple<string, string> >::const_iterator i = judge->begin(); i != judge->end(); ++i)
	{
		string entity = i->get<0>();
		CL2.push_back(entity);
	}

	return CL2;
}

vector<string> Utils::getQueryAspects(string f_query)
{
	vector<string> q_vec;
	boost::split(q_vec, f_query, boost::is_any_of(","));

	return q_vec;
}

vector<string> Utils::getQueryTerms(string f_query)
{
	vector<string> p_vec;
	boost::split(p_vec, f_query, boost::is_any_of(" "));

	return p_vec;
}


