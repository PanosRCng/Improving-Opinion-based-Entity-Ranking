#include <iostream>
#include "judgment.h"
#include "boost/tuple/tuple.hpp"
#include <vector>
#include <boost/algorithm/string.hpp>
#include "tagger.h"
#include "utils.h"
#include "fileMapIO.h"
#include "wordNet.h"
#include "sentimenter.h"

using namespace std;

bool b_compare(const boost::tuple<string,double> &lhs, const boost::tuple<string,double> &rhs)
{
	return lhs.get<1>() > rhs.get<1>();
}

int main()
{
// #############  set some enviroment variables here  ####################

	// set the project path here
	string project_path = string("/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/");

	// set the reviews' source directory
	string reviews_src_path = project_path+"dataset/cars/data/2008/";

	// set the directory to save the parsed reviews
	string reviews_path = project_path+"bin/reviews/";

	// set the path for the judgment files	
	string judge_path = project_path+"dataset/cars/judgments/2008/";

	// set the WordNet path
	string wordnet_path = "/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/bin/WordNet/dict/";

	// set the path to store the sentiments
	string senti_path = project_path+"bin/sentimentsOffline/";

	// set the number of the judgment cases
	int jud_numbers = 100;

// #######################################################################

	// help functions
	Utils* utils = new Utils();

	// parse reviews
	utils->parse_reviews(reviews_src_path, reviews_path);
	cout << "parse reviews ....... [ok]" << endl;

	// get judgement files
	vector<string> files = vector<string>();
	utils->getFilesList(judge_path,files, true);

	// load WordNet
	WordNet* wordnet = new WordNet(wordnet_path);

	// load Pos Tagger
	Tagger* pos_tagger = new Tagger();

	// load sentimenter
	Sentimenter* sentimenter = new Sentimenter();

	typedef vector< boost::tuple<string, string> > judge_list;
	typedef vector< boost::tuple<string, string> > query_list;

	query_list query;
	judge_list judge;

	map< string,vector<string> > aspects_full;


	// collect all aspects

	// for every judgment
	for(int it_j=0; it_j<jud_numbers; it_j++)
	{
		string filename = string(files[it_j]);

		Judgment* my_judgment = new Judgment(filename.c_str());

		// parse judgment and keep it in memory
		query = my_judgment->get_query();
		judge = my_judgment->get_judge();

		// get the query category
		vector<string> category = my_judgment->get_cat();
		string category_str = utils->getQueryStr(category);

		vector<string> aspects;

		// for every query term find synonyms using the WordNet
                for(int i=0; i<category.size(); i++)
                {
			string aspect = category.at(i);
                        vector<string> synonyms = wordnet->getSynonyms(aspect);

			aspects_full.insert( pair<string, vector<string> >( aspect, synonyms ) );
                }
	}
	cout << "collect all aspects ..... [ok]" << endl;

	map<string,double> sentiments;

	// do sentiment analysis
	for(judge_list::const_iterator i = judge.begin(); i != judge.end(); ++i)
	{
		string entity = i->get<0>();

		string reviewPath = reviews_path + entity;

		sentiments = sentimenter->do_sentimentAnalysis(reviewPath, aspects_full, pos_tagger);

		string sentiment_file = string(senti_path + entity);

		FileMapIO::sentiMapToFile(sentiment_file, sentiments);
	}
	cout << "extract sentiments from reviews ...... [ok]" << endl;


	utils->deleteFiles("reviews");
}



