#include "judgment.h"
#include <boost/tuple/tuple.hpp>
#include <vector>
#include "tagger.h"
#include "invertedIndexer.h"
#include "scorer.h"
#include "wordNet.h"
#include "utils.h"

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

	// set the path to read the reviews' inverted index
	string ii_path = project_path+"bin/invertedIndexes/"+"test2008ii";

	// set the WordNet path
	string wordnet_path = "/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/bin/WordNet/dict/";

	// set the number of the judgment cases
	int jud_numbers =100;

	// set the results filename
	string results_file = "results.txt";

// #######################################################################

	// help functions
	Utils* utils = new Utils();

	// parse reviews
	utils->parse_reviews(reviews_src_path, reviews_path);
	cout << "parse reviews ....... [ok]" << endl;

	// get judgement files
	vector<string> files = vector<string>();
	utils->getFilesList(judge_path,files, true);

	// open file to write the results
	ofstream results (results_file.c_str());
	if (results.is_open())
	{
		int result_counter = 1;

		// load WordNet
		WordNet* wordnet = new WordNet(wordnet_path);
		
		// load Pos Tagger
		Tagger* pos_tagger = new Tagger();

		// create an invertedIndexer instance
		InvertedIndexer* invertedIndexer = new InvertedIndexer();

		// load the invertedIndex from the disk to memory
		invertedIndexer->fileToInvertedIndex(ii_path);
		cout << "load invertedIndex of reviews ...... [ok]" << endl;

		// for every judgment
		for(int it_j=0; it_j<jud_numbers; it_j++)
		{
			string filename = string(files[it_j]);

			typedef vector< boost::tuple<string, string> > judge_list;
			typedef vector< boost::tuple<string, string> > query_list;

			Judgment* my_judgment = new Judgment(filename.c_str());

			// parse judgment and keep it in memory
			query_list query = my_judgment->get_query();
			judge_list judge = my_judgment->get_judge();
			
			vector<string> category = my_judgment->get_cat();
			string category_str = utils->getQueryStr(category);
	
			// aspect expansion using WordNet
			vector<string> aspects;
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

			// score entities using BM25 value of review file on query aspects
			Scorer* scorer = new Scorer();
			vector<boost::tuple<string, double> > scores;

			for(judge_list::const_iterator i = judge.begin(); i != judge.end(); ++i)
			{
				string entity = i->get<0>();
				double score = 0.0;

				string entityPath = reviews_path + entity;

				// calculate the BM25 value of the review file (entityPath) to the aspects query
				score = invertedIndexer->getBM25(entityPath, aspects);

				scores.push_back( boost::tuple<std::string, double>( entity, score ) );
			}

                        // get ideal scores
                        vector<boost::tuple<string, double > > i_scores = my_judgment->getIdealScores();

			// calculate the nDCG value of the ranking list, on the top 10 results
			double nDCG = scorer->getNDCG(scores, i_scores);

			cout << " nDCG= " << nDCG << endl;

			// write results to file
			results << result_counter << "\t" << nDCG << "\n";
			result_counter++;

		}

		results.close();

	}
	else
	{
		cout << "Unable to open file to write results" << endl;
	}

	utils->deleteFiles("reviews");
}



