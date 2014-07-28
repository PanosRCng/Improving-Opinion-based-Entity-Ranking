#include <boost/tuple/tuple.hpp>
#include <vector>
#include "tagger.h"
#include "invertedIndexer.h"
#include "utils.h"
#include "wordNet.h"
#include "judgment.h"

using namespace std;

int main()
{
	unsigned int npos = 4294967295;

// #############  set some enviroment variables here  ####################

	// set the project path here
	string project_path = string("/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/");

	// set the reviews' source directory
	string reviews_src_path = project_path+"dataset/cars/data/2008/";

	// set the directory to save the parsed reviews
	string reviews_dst_path = project_path+"bin/reviews/";

	// set the path for the judgment files	
	string judge_path = project_path+"dataset/cars/judgments/2008/";

	// set the WordNet path
	string wordnet_path = "/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/bin/WordNet/dict/";

	// set the path to save the reviews' inverted index
	string ii_path = project_path+"bin/invertedIndexes/"+"test2008ii";

	// set the number of the judgment cases
	int jud_numbers = 100;

// ######################################################################################


	// help functions
	Utils* utils = new Utils();

	// parse reviews
	utils->parse_reviews(reviews_src_path, reviews_dst_path);
	cout << "parse reviews ....... [ok]" << endl;

	// get judgement files
	vector<string> files = vector<string>();
	utils->getFilesList(judge_path,files, true);

	// load WordNet
	WordNet* wordnet = new WordNet(wordnet_path);

	// load Pos Tagger
	Tagger* pos_tagger = new Tagger();

	typedef vector< boost::tuple<string, string> > judge_list;
	typedef vector< boost::tuple<string, string> > query_list;

	query_list query;
	judge_list judge;

	map<string, int> aspects_full;

	
	// collect all important terms from the queries

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

			aspects.push_back(aspect);
			for(int i=0; i<synonyms.size(); i++)
			{
				aspects.push_back(synonyms.at(i));
			}
                }

                for(int i=0; i<aspects.size(); i++)
                {
			pair<map<string,int>::iterator,bool> ret;
			ret = aspects_full.insert( pair<string,int>( aspects.at(i), 1 ) );
		  	if (ret.second==false)
			{
				//
			}
                }
	}

	// get the reviews filelist
	vector<string> reviewsFilelist = vector<string>();
	utils->getFilesList(reviews_dst_path,reviewsFilelist, true);
	
	// build and save to disk the inverted index
	InvertedIndexer* invertedIndexer = new InvertedIndexer();

	invertedIndexer->buildInvertedIndex(&reviewsFilelist, pos_tagger, &aspects_full);
	cout << "build invertedIndex ...... [ok]" << endl;

	invertedIndexer->InvertedIndexToFile(ii_path);
}


