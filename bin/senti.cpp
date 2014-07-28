#include <iostream>
#include "judgment.h"
#include "boost/tuple/tuple.hpp"
#include <vector>
#include <boost/algorithm/string.hpp>
#include "tagger.h"
#include "scorer.h"
#include "wordNet.h"
#include "utils.h"
#include "fileMapIO.h"
#include "sentimenter.h"

using namespace std;

int main()
{
// #############  set some enviroment variables here  ####################

	// set the project path here
	string project_path = string("/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/");

	// set the reviews' source directory
	string reviews_src_path = project_path+"dataset/cars/data/2008/";

	// set the path for the judgment files	
	string judge_path = project_path+"dataset/cars/judgments/2008/";

	// set the path to store the sentiments
	string senti_path = project_path+"bin/sentimentsOffline/";

	// set the number of the judgment cases
	int jud_numbers = 1;

//	string dir_Dinf = project_path + "/D_inf/";

// #######################################################################


	// help functions
	Utils* utils = new Utils();

	// get judgement files
	vector<string> files = vector<string>();
	utils->getFilesList(judge_path,files, true);

	// open results file
	ofstream results ("results.txt");
	if (results.is_open())
	{
		int result_counter = 1;

		// load sentimenter
		Sentimenter* sentimenter = new Sentimenter();

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
			
			// get the query category
			vector<string> category = my_judgment->get_cat();
			string category_str = utils->getQueryStr(category);

			cout << category_str << endl;

			Scorer* scorer = new Scorer();
			vector<boost::tuple<string, double> > scores;

			// score entities using only sentiments on aspects
			for(judge_list::const_iterator i = judge.begin(); i != judge.end(); ++i)
			{
				string entity = i->get<0>();
				string entitySentiPath = string(senti_path + entity);
				double score = 0.0;
				score = sentimenter->getScore(entitySentiPath, category);

				cout << entity << " " << score << endl;

				scores.push_back( boost::tuple<std::string, double>( entity, score ) );
			}

                        // get ideal scores
                        vector<boost::tuple<string, double > > i_scores = my_judgment->getIdealScores();

                        // calculate nDCG value of ranking list
                        double nDCG = scorer->getNDCG(scores, i_scores);

                        cout << " nDCG= " << nDCG << endl;
                        results << result_counter << "\t" << nDCG << "\n";
                        result_counter++;
		}

		results.close();
	}
	else
	{
		cout << "Unable to open file to write results" << endl;
		return 0;
	}

}



