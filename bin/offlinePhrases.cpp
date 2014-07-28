#include <iostream>
#include "judgment.h"
#include "boost/tuple/tuple.hpp"
#include <vector>
#include <boost/algorithm/string.hpp>
#include "tagger.h"
#include "utils.h"
#include "wordNet.h"
#include "sentimenter.h"
#include "invertedIndexer.h"
#include "fileMapIO.h"

using namespace std;

int main()
{
// #############  set some enviroment variables here  ####################

	// set the project path here
	string project_path = string("/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/");

	// set the path for the judgment files	
	string judge_path = project_path+"dataset/cars/judgments/2008/";

	// set the reviews' source directory
	string reviews_src_path = project_path+"dataset/cars/data/2008/";

	// set the directory for the parsed reviews
	string reviews_path = project_path+"bin/reviews/";

	// set the path to store the phrases' inverted index
	string ii_path = project_path+"bin/invertedIndexes/"+"phrases2008ii";

	// set the path to store the sentiment orientations
	string so_path = project_path+"bin/SOS/"+"2008sos";

	// set the WordNet path
	string wordnet_path = "/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/bin/WordNet/dict/";

	// set the number of the judgment cases
	int jud_numbers =1;

//	string Dinf = string(project_path + "D_inf_2009");

// #######################################################################

	unsigned int npos = 4294967295;


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

	vector<string> aspects_terms;
	map<string, int> aspects_full;

	// for every judgment
	for(int it_j=0; it_j<jud_numbers; it_j++)
	{
		string filename = string(files[it_j]);

		Judgment* my_judgment = new Judgment(filename.c_str());

		// parse judgment and keep it in memory
		query = my_judgment->get_query();
		judge = my_judgment->get_judge();
			
		vector<string> category = my_judgment->get_cat();
		string category_str = utils->getQueryStr(category);

		vector<string> aspects;

		// aspect expansion using WordNet
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

	// coolect all aspects
	for(map<string, int>::const_iterator iter_m= aspects_full.begin(); iter_m!=aspects_full.end(); ++iter_m)
	{
		aspects_terms.push_back(iter_m->first);
	}
	cout << "aspect collection ..... [ok]" << endl;

	vector<string> vector_phrases;

	map<string, int> full_phrases;

	// get opinion phrases using pos tag patterns
	for(judge_list::const_iterator it = judge.begin(); it != judge.end(); ++it)
	{
		vector<string> phrases = sentimenter->getPhrases(it->get<0>(), aspects_terms, pos_tagger);

		for(int i=0; i<phrases.size(); i++)
		{
			boost::algorithm::to_lower(phrases.at(i));

			pair<map<string,int>::iterator,bool> ret;
			ret = full_phrases.insert( pair<string,int>( phrases.at(i), 1 ) );
		  	if (ret.second==false)
			{
				//
			}
		}
	}

	// collect all phrases
	for(map<string, int>::const_iterator iter_p= full_phrases.begin(); iter_p!=full_phrases.end(); ++iter_p)
	{
		vector_phrases.push_back(iter_p->first);
	}

	cout << "extract phrases from reviews ...... [ok]" << endl;

	
	// get some standard positive and negative terms

	vector<string> good_terms;
	good_terms.push_back("excellent");
	good_terms.push_back("good");
	vector<string> good_sentis_terms = wordnet->synonymExpansion(good_terms);

	vector<string> bad_terms;
	bad_terms.push_back("horrible");
	bad_terms.push_back("bad");
	vector<string> bad_sentis_terms = wordnet->synonymExpansion(bad_terms);

	cout << "get standard positive and negative terms ...... [ok]" << endl;


/*
	// get all Dinf files
	vector<string> dinf_folders = vector<string>();
	utils->getdir(Dinf,dinf_folders);
	vector<string> dinf_files;

	for(int i=0; i<dinf_folders.size(); i++)
	{
		string folder = string( Dinf + "/" + dinf_folders.at(i) );
		vector<string> files;
		utils->getdir(folder,files);

		for(int j=0; j<files.size(); j++)
		{
			string file = string(folder + "/" + files.at(j));
			dinf_files.push_back(file); 
	
			// only one file fron every category
			break;
		}
	}
	cout << "get all Dinf files: " << dinf_files.size() << endl;
*/

	// get reviews filelist
	vector<string> reviewsFilelist = vector<string>();
	utils->getFilesList(reviews_path,reviewsFilelist, true);

	InvertedIndexer* invertedIndexer = new InvertedIndexer();

	map<string, int> ii = invertedIndexer->buildPhrasesInvertedIndex(&reviewsFilelist, pos_tagger, &vector_phrases,
								 &good_sentis_terms, &bad_sentis_terms);

	invertedIndexer->InvertedIndexPhrasesToFile(ii, ii_path);
	map<string, int> ii_phrases = invertedIndexer->fileToInvertedIndexPhrases(ii_path);

	// make a map with phrases and the SOs, and save it to disk
	map<string, double> sos = invertedIndexer->calculateSOS(&ii);
	FileMapIO::sentiMapToFile(so_path, sos);

	map<string,double> sos_load = FileMapIO::fileToSentiMap(so_path) ;
	cout << "load SOs ........ [ok]" << endl;
	cout << "SOs size: " << sos_load.size() << endl;

	utils->deleteFiles("reviews");
}




