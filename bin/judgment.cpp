#include <iostream>
#include <fstream>
#include <string>
#include <boost/tuple/tuple.hpp>
#include <vector>
#include <list>
#include <boost/algorithm/string.hpp>
#include "judgment.h"

using namespace std;
using boost::tuple;

typedef std::vector< boost::tuple<std::string, std::string> > judge_list;
typedef std::vector< boost::tuple<std::string, std::string> > query_list;

Judgment::Judgment(const char* filename)
{
	parse_judgment(filename);
}

Judgment::~Judgment()
{
	//
}

int Judgment::parse_judgment(const char* filename)
{
	unsigned int npos = 4294967295;

	string line;
	int judge_flag = 0;
	int query_flag = 0;

	int max_entities = 10;
	int entity_counter = 0;

	ifstream myfile (filename);

	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
      			getline (myfile,line);

			unsigned cat_found = line.find("#cat=");
			unsigned query_found = line.find("#query=");
			unsigned judge_found = line.find("#judge");

			if(judge_flag == 1)
			{
				int pos_g = line.find(";");

				if(pos_g>-1)
				{
//					if( entity_counter < max_entities )
//					{
						string entity = line.substr(0, pos_g);
						string rel = line.substr(pos_g+1, npos);

						judge.push_back( tuple<string, string>(entity, rel) );

//						entity_counter++;
//					}
				}
			}

			if(cat_found!=npos)
			{
				cat = line.substr(5, npos);
			}
			else if(query_found!=npos)
			{
				query_flag = 1;
				judge_flag = 0;
			}
			else if(judge_found!=npos)
			{
				judge_flag = 1;
				query_flag = 0;
			}

			if(query_flag == 1)
			{
				int pos_q = line.find(";");

				if(pos_q>-1)
				{
					string query_str = line.substr(7, pos_q-7);
					string id_str = line.substr(pos_q+1, npos);

					query.push_back( tuple<string, string>(query_str, id_str) );
				}
			}
    		}

    		myfile.close();
	}
	else
	{
		cout << "Unable to open file";
	}

	return 0;
}


// pending delete

/*
int Judgment::parse_review(const char* entity_name)
{
	unsigned int npos = 4294967295;

	string line;

	const string review_path = "/home/panos/Desktop/Opinion_MIning_and_Sentiment_Analysis/project/dataset/cars/data/2007/";
	const string out_review_path = "reviews/";

	string review_out = out_review_path + string(entity_name);

	ofstream output (review_out.c_str());

	if (output.is_open())
	{
		string entity_review = review_path + string(entity_name);

		ifstream input (entity_review.c_str());

		if (input.is_open())
		{
			while ( input.good() )
			{
				getline (input,line);

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
			cout << "Unable to open review file for read"; 
		}

		output.close();
	}
	else
	{
		cout << "Unable to open file to write review";
	}

	return 0;
}

int Judgment::parse_hotel_review(const char* entity_name)
{
	unsigned int npos = 4294967295;

	string line;

	const string review_path = "/home/panos/Desktop/Opinion_MIning_and_Sentiment_Analysis/project/dataset/hotels/data/new-york-city/";
	const string out_review_path = "reviews/";

	string review_out = out_review_path + string(entity_name);

	ofstream output (review_out.c_str());

	if (output.is_open())
	{
		string entity_review = review_path + string(entity_name);

		ifstream input (entity_review.c_str());

		if (input.is_open())
		{
			while ( input.good() )
			{
				getline (input,line);

				string paragraph = "";

				unsigned start_text = line.find("\t");
				unsigned end_text = line.find(".\n");

				if(start_text != npos)
				{
					list<string> sentences;			// get paragraph and slit to sentences
					paragraph = line.substr(start_text+1, (end_text-start_text)); 

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
			cout << "Unable to open review file for read"; 
		}

		output.close();
	}
	else
	{
		cout << "Unable to open file to write review";
	}

	return 0;
}
*/

judge_list Judgment::get_judge()
{
	return judge;
}

query_list Judgment::get_query()
{
	return query;
}

vector<string> Judgment::get_cat()
{
	vector<string> vector_cat;

	vector<string> q_vec;
	boost::split(q_vec, cat, boost::is_any_of(","));

	for(int i=0; i<q_vec.size()-1; i++)
	{
		vector_cat.push_back(q_vec[i]);
	}

	return vector_cat;
}

list<string> Judgment::getSentences(string paragraph)
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

			sentences.push_back(sentence);
		}
		
		start_sentence = end_sentence+1;

	}while( end_sentence < paragraph.size());

	return sentences;
}

vector<boost::tuple<string, double > > Judgment::getIdealScores()
{
	vector<boost::tuple<string, double > > i_scores;

	for(judge_list::const_iterator i = judge.begin(); i != judge.end(); ++i)
	{
		string entity = i->get<0>();
		string is_score = i->get<1>();

		double i_score = atof(is_score.c_str());

		i_scores.push_back( boost::tuple<std::string, double >( entity, i_score ) );
	}

	return i_scores;
}











