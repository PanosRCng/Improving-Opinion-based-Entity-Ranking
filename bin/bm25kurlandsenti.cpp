#include <iostream>
#include "judgment.h"
#include <boost/tuple/tuple.hpp>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "tagger.h"
#include "invertedIndexer.h"
#include "scorer.h"
#include "utils.h"
#include "fileMapIO.h"
#include "kmeans.h"
#include "knearest.h"
#include "kurland.h"
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

	// set the path for the review files
	string reviews_src = project_path + "dataset/cars/data/2008/";

	// set the path to save the parsed reviews
	string reviews_dst = project_path + "bin/reviews/";

	// set the path for the judgment files	
	string judge_path = project_path+"dataset/cars/judgments/2008/";

	// set the path to read the review's inverted index
	string ii_path = project_path+"bin/invertedIndexes/"+"test2008ii";

	// set the WordNet path
	string wordnet_path = "/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/bin/WordNet/dict/";

	// set the path to save the documents' features this is the sentiments directory (we are doing opinion-based clustering)
 	string features_dir = project_path+"bin/sentimentsOffline/";

	// set the number of the judgment cases
	int jud_numbers =100;

	// set the results filename
	string results_file = "results.txt";

// #######################################################################

	// help functions
	Utils* utils = new Utils();

	// parse reviews
	utils->parse_reviews(reviews_src, reviews_dst);
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

			vector<string> category = my_judgment->get_cat();
			string category_str = utils->getQueryStr(category);

			cout << category_str << endl;

			// get the parsed reviews list, CL is used from the ClustFust algorithm (Kurland's schema)
			vector<string> CL = vector<string>();
			utils->getFilesList(reviews_dst, CL, false);

			// cluster reviews files
			map<int, vector<string> > clusters;

			// create a knearest instance
			Knearest* knearest = new Knearest();
			// construct clusters on the documents using the knearest algorithm (modified to construct overlapping clusters - Kurland paper)
			clusters = knearest->do_knearest(features_dir, 5);

			// alternative clustering algorithm
			//Kmeans* kmeans = new Kmeans();
			//clusters = kmeans->do_kmeans(features_dir, 3);

			cout << "get clusters	...... [ok]" << endl;

			// use only 5 disambiguation queries for the ClustFuse algorithm --> speed choice, tested with all queries, the same results
			int query_counter = 5;
			int counter = 0;


			// for every judgment query use 5 disambiguation queries and for every disambiguation query get a document ranking list using BM25
			map<string, vector<double> > lists;
			for(query_list::const_iterator iq = query.begin(); iq != query.end(); iq++)
			{
				if(counter>=query_counter)
				{
					break;
				}

				counter++;

				// make query expansion
				string q = iq->get<0>();
				vector<string> query_vector = utils->getQueryVec(q);

				vector<string> exp_query_vector;
	                        for(int i=0; i<query_vector.size(); i++)
	                        {
					string query_term = query_vector.at(i);
	                        	vector<string> exp = wordnet->getSynonyms(query_term);

					exp_query_vector.push_back(query_term);
					for(int i=0; i<exp.size(); i++)
					{
						exp_query_vector.push_back(exp.at(i));
					}
	                      	}


			/*
				get ranking lists speed up code -- get the first N most relevant documents
				-- !!!! for test puprpose only !!! ---

				// get bm25 for every document
				vector< boost::tuple<string, double > > tmp_rlist;
				vector< boost::tuple<string, double > >	rlist;
				for(int i=0; i<CL.size(); i++)
				{
					string doc_name = string(CL[i]);
					double bm25 = invertedIndexer->getBM25(doc_name, exp_query_vector);	

					tmp_rlist.push_back( boost::tuple<string,double>(doc_name, bm25) );
				}
			
				// get the first N most relevant
				sort(tmp_rlist.begin(),tmp_rlist.end(),b_compare); 
				for(int i=0; i<N; i++)
				{
					rlist.push_back(tmp_rlist.at(i));
				}

				for(int i=0; i<rlist.size(); i++)
				{
					string d = rlist.at(i).get<0>();
					double bm25 = rlist.at(i).get<1>(); 

					map< string, vector<double > >::iterator iter_l = lists.find(d);
	
					if( iter_l != lists.end() )
					{
						vector<double> *list;
						list = &iter_l->second;

						list->push_back(bm25);
					}
					else
					{
						vector<double> list;
						list.push_back(bm25);

						lists.insert( pair< string,vector<double> >(d, list) );
					}
				}
			}
			cout << "get ranking lists	...... [ok]" << endl;

			*/


				for(int i=0; i<CL.size(); i++)
				{
					string doc_name = string(CL[i]);

					double bm25 = invertedIndexer->getBM25(doc_name, query_vector);

					map< string, vector<double > >::iterator iter_l = lists.find(doc_name);

					if( iter_l != lists.end() )
					{
						vector<double> *list;
						list = &iter_l->second;

						list->push_back(bm25);
					}
					else
					{
						vector<double> list;
						list.push_back(bm25);

						lists.insert( pair< string,vector<double> >(doc_name, list) );
					}
				}
			}
			cout << "get ranking lists	...... [ok]" << endl;

			// do the ClustFuse algorithm using the Kurland object to give a score to every document -- same variable naming as in Kurlands' paper, kind of
			vector< boost::tuple<string, double> > P_q_ds;
			Kurland* kurland = new Kurland();
			P_q_ds = kurland->getP_q_ds(CL, clusters, lists, features_dir);
			cout << "do Kurland 	...... [ok]" << endl;

			vector< boost::tuple<string, double> > scores;	

			for(judge_list::const_iterator is = judge.begin(); is != judge.end(); ++is)
			{
				string entity = is->get<0>();
				double p_q_di = 0.0;

				for(int k=0; k<P_q_ds.size(); k++)
				{
					if( P_q_ds[k].get<0>() == entity )
					{
						p_q_di = P_q_ds[k].get<1>();
						break;
					}
				}

				cout << entity << " " << p_q_di << endl;

				scores.push_back( boost::tuple<string, double>( entity, p_q_di ) );				
			}

			Scorer* scorer = new Scorer();

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

		// delete temporary parsed review files
		utils->deleteFiles("reviews");
	}
	else
	{
		cout << "Unable to open file to write results" << endl;
		return 0;
	}

}



