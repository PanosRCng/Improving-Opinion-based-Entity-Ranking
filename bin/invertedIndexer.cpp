#include <iostream>
#include "invertedIndexer.h"
#include <boost/tuple/tuple.hpp>
#include <map>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <boost/algorithm/string.hpp>
#include <cmath>
#include "fileMapIO.h"
#include <algorithm>

using namespace std;

InvertedIndexer::InvertedIndexer()
{
	// help functions
	utils = new Utils();

	loadStopWords();
}

InvertedIndexer::~InvertedIndexer()
{
	//
}

void InvertedIndexer::loadStopWords(void)
{
	stopwords.push_back("!");
	stopwords.push_back("@");
	stopwords.push_back("#");
	stopwords.push_back("$");
	stopwords.push_back("%");
	stopwords.push_back("^");
	stopwords.push_back("&");
	stopwords.push_back("*");
	stopwords.push_back("(");
	stopwords.push_back(")");
	stopwords.push_back("_");
	stopwords.push_back("-");
	stopwords.push_back("+");
	stopwords.push_back("=");
	stopwords.push_back("~");
	stopwords.push_back("`");
	stopwords.push_back("/");
	stopwords.push_back("<");
	stopwords.push_back(">");
	stopwords.push_back("?");
	stopwords.push_back("|");
	stopwords.push_back("'");
	stopwords.push_back(":");
	stopwords.push_back(";");
	stopwords.push_back(",");
	stopwords.push_back(".");
	stopwords.push_back("1");
	stopwords.push_back("2");
	stopwords.push_back("3");
	stopwords.push_back("4");
	stopwords.push_back("5");
	stopwords.push_back("6");
	stopwords.push_back("7");
	stopwords.push_back("8");
	stopwords.push_back("9");
	stopwords.push_back("0");
}

// builds an inverted index for the files in directory
int InvertedIndexer::buildInvertedIndex(vector<string>* files, Tagger* pos_tagger, map<string, int>* aspects)
{
	unsigned int npos = 4294967295;

	total_documents = 0;
	avg_file_size = 0.0;

	for (int k=0; k<files->size(); k++)
	{
		string filename = files->at(k);

		total_documents++;

		vector<string> lines = utils->getLines(filename);

		int filesize = 0;

		for(int i=0; i<lines.size(); i++)
		{
			string line = lines.at(i);
			boost::algorithm::to_lower(line);

			vector<string> terms = getTerms(line, pos_tagger);

			for (vector<string>::iterator it = terms.begin(); it != terms.end(); ++it)
			{
				string term = *it;

				map<string,int>::iterator it_a = aspects->find(term);
				if(it_a != aspects->end())
				{
					//							
				}
				else
				{
					continue;
				}

				bool stop_char=false;
				for(int i=0; i<stopwords.size(); i++)
				{
					if( term.find(stopwords[i]) != npos )
					{
						stop_char=true;
					}
				}

				// reject stopwords and bad terms (3-letter terms == not aspects)
				if( (term.size()<4) || (stop_char) )
				{
					continue;
				}

				vector<boost::tuple<std::string, int> > tfs;
				tfs.push_back( boost::tuple<std::string, int>( filename, 1 ) );

				// insert or update term entry in the inverted index
				pair<map< string, vector<boost::tuple<std::string, int> > > ::iterator,bool> ret;
				ret = invertedIndex.insert( pair<string, vector<boost::tuple<std::string, int> > >( term, tfs ) );
			  	if (ret.second==false)
				{
					vector<boost::tuple<std::string, int> > *uptfs;

					uptfs = &ret.first->second;

					int flag = 1;

					for(int j=0; j < uptfs->size(); j++)
					{
						if( filename == uptfs->at(j).get<0>() )
						{
							uptfs->at(j).get<1>() = uptfs->at(j).get<1>() + 1;
							flag = 0;
							break;
						}
					}

					if(flag == 1)
					{
						uptfs->push_back( boost::tuple<std::string, int>( filename, 1 ) );
					}
				}
			}

			filesize += terms.size();
		}

		fileSizes.insert( pair<string, int>( filename, filesize ) );
	}

	double file_counter = 0.0;

	// calculate the average file size
	for(map<string,int>::const_iterator iter_f= fileSizes.begin(); iter_f!=fileSizes.end(); ++iter_f)
	{
		avg_file_size = avg_file_size + iter_f->second;
		file_counter++;
	}

	avg_file_size = avg_file_size / file_counter;

	return 1;
}

// returns the terms of a line as a vector of strings
vector<string> InvertedIndexer::getTerms(string line, Tagger* pos_tagger)
{
	vector<string> terms;

	if(line.size() > 0)
	{
		boost::algorithm::to_lower(line);

		vector<string> tmp;

		// use the pos tagger to locate only the possible aspect terms (NN, JJ, JJR, JJS, NNS, NNP, NNPS) 
		tmp = pos_tagger->do_sentence_tagging_for_aspects(line);

		for(int i=0; i<tmp.size(); i++)
		{
			terms.push_back(tmp[i]);
		}
	}

	return terms;
}

int InvertedIndexer::InvertedIndexPhrasesToFile(map<string,int> ii, string filename)
{
	ofstream ofile;
	ofile.open(filename.c_str());
	if(!ofile)
	{
		return 0;	//file does not exist and cannot be created.
	}

	// store invertedIndex
	for(map<string, int>::const_iterator it= ii.begin(); it!=ii.end(); ++it)
	{
		string term = it->first;
		int df = it->second;

		ofile<<term<<"|"<<df;

		ofile << std::endl;
	}

	return 0;
}


map<string,int> InvertedIndexer::fileToInvertedIndexPhrases(string filename) 
{
	map<string,int> ii;

	unsigned int npos = 4294967295;

	ifstream myfile(filename.c_str());

	if (myfile.is_open())
	{
		string line;

		while ( myfile.good() )
    		{
      			getline (myfile,line);
		
			vector<string> parts; 
			boost::split(parts, line, boost::is_any_of("|"));

			string term = parts[0];
			int df = atoi(parts[1].c_str());

			ii.insert( pair<string,int>( term, df ) );
		}
	}
	
	return ii;
}

// build inverted index of phrases of Dinf using map of phrases 
map<string,int> InvertedIndexer::buildPhrasesInvertedIndex(vector<string>* files, Tagger* pos_tagger, vector<string>* phrases, vector<string>* good_terms, vector<string>* bad_terms)
{
	map<string,int> ii;

	unsigned int npos = 4294967295;

	total_documents = 0;
	avg_file_size = 0.0;

	for (int k=0; k<files->size(); k++)
	{
		string filename = files->at(k);

		total_documents++;

		vector<string> lines = utils->getLines(filename);

		for(int i=0; i<lines.size(); i++)
		{
			string line = lines.at(i);
			boost::algorithm::to_lower(line);

			bool good_flag = false;
			bool bad_flag = false;

			for(int j=0; j<good_terms->size(); j++)
			{
			    string term = good_terms->at(j);

			    if( line.find(term) != npos)
			    {
				pair<map<string,int>::iterator,bool> ret;
				ret = ii.insert( pair<string,int>( "+", 1 ) );
				if (ret.second==false)
				{
					int* df = &ret.first->second;
					*df = *df + 1;
				}

				good_flag = true;
				break;
			    }
			}

			for(int j=0; j<bad_terms->size(); j++)
			{
			    string term = bad_terms->at(j);

			    if( line.find(term) != npos)
			    {
				pair<map<string,int>::iterator,bool> ret;
				ret = ii.insert( pair<string,int>( "-", 1 ) );
				if (ret.second==false)
				{
					int* df = &ret.first->second;
					*df = *df + 1;
				}

				bad_flag = true;
				break;
			    }
			}

			for(int j=0; j<phrases->size(); j++)
			{
			    string phrase = phrases->at(j);

			    if( line.find(phrase) != npos)
			    {
				if( good_flag )
				{
					pair<map<string,int>::iterator,bool> ret;
					ret = ii.insert( pair<string,int>( phrase+"(+)", 1 ) );
					if (ret.second==false)
					{
						int* df = &ret.first->second;
						*df = *df + 1;
					}
				}

				if( bad_flag )
				{
					pair<map<string,int>::iterator,bool> ret;
					ret = ii.insert( pair<string,int>( phrase+"(-)", 1 ) );
					if (ret.second==false)
					{
						int* df = &ret.first->second;
						*df = *df + 1;
					}
				}
			    }
			}
		}
	}

	map<string,int>::iterator iter_pos_f = ii.find("+");
	if( iter_pos_f != ii.end() )
	{
		iter_pos_f->second = (int) ( (iter_pos_f->second/good_terms->size()) + 0.5 );
	}

	return ii;
}

map<string, double> InvertedIndexer::calculateSOS(map<string, int>* ii_phrases)
{
	map<string, double> sos;

	unsigned int npos = 4294967295;

	int pos_hits=0;
	map<string,int>::iterator iter_pos = ii_phrases->find("+");
	if( iter_pos != ii_phrases->end() )
	{
		pos_hits = iter_pos->second;
	}

	int neg_hits=0;
	map<string,int>::iterator iter_neg = ii_phrases->find("-");
	if( iter_neg != ii_phrases->end() )
	{
		neg_hits = iter_neg->second;
	}

	//
//	neg_hits = pos_hits;
	//

	for(map<string, int>::iterator iter_i= ii_phrases->begin(); iter_i!=ii_phrases->end(); ++iter_i)
	{		
		string term = iter_i->first;
		int pos_phrase_hits = iter_i->second;
		int neg_phrase_hits = 0;
		double so = 0.0;

		if (npos != term.find("+"))
		{
			vector<string> parts; 
			boost::split(parts, term, boost::is_any_of("(+)"));

			string phrase = parts[0];

			map<string,int>::iterator iter_h = ii_phrases->find(phrase+"(-)");
			if( iter_h != ii_phrases->end() )
			{
				vector<string> par; 
				boost::split(par, iter_h->first, boost::is_any_of("(-)"));

				neg_phrase_hits = iter_h->second;
			}
			else
			{
				continue;
			}

			so = Log2( double((pos_phrase_hits)*(neg_hits)) / double((neg_phrase_hits)*(pos_hits)) );

			sos.insert( pair<string,double>( phrase, so ) );
		}
	}

	return sos;
} 

// saves the invertedIndex to file
int InvertedIndexer::InvertedIndexToFile(string filename)
{
	ofstream ofile;
	ofile.open(filename.c_str());
	if(!ofile)
	{
		return 0;	//file does not exist and cannot be created.
	}

	// store invertedIndex
	for(map<string, vector<boost::tuple<string, int> > >::const_iterator it= invertedIndex.begin(); it!=invertedIndex.end(); ++it)
	{
		string term = it->first;

		vector<boost::tuple<std::string, int> > uptfs;
		uptfs = it->second;

		ofile<<term<<"$";

		for(int i=0; i< uptfs.size(); i++)
		{
			string file = uptfs.at(i).get<0>();
			double tf = uptfs.at(i).get<1>();

			ofile<<file<<"@"<<tf<<"|";
		}
		ofile << std::endl;
	}

	// store filesizes map
	for(map<string,int>::const_iterator iter= fileSizes.begin(); iter!=fileSizes.end(); ++iter)
	{
		string file = iter->first;
		int filesize = iter->second;

		ofile<<file<<"&"<<filesize;
		ofile << std::endl;
	}

	// store total documents number
	ofile<<total_documents<<"%"<<std::endl;

	// store average file size
	ofile<<avg_file_size<<"*"<<std::endl;

	return 1;
}

// loads invertedIndex to memory from file
int InvertedIndexer::fileToInvertedIndex(string filename) 
{
	unsigned int npos = 4294967295;

	ifstream ifile;
	ifile.open(filename.c_str());

	if(!ifile)
	{
		return 0;
	}  	

	string line;

	while(ifile>>line)
	{
		int term_pos = line.find("$"); 
		int termFilesize_pos = line.find("&");
		int total_pos = line.find("%");
		int avg_filesize_pos = line.find("*");
		
		if(term_pos != npos)	// get the invertedIndex
		{
			string term = line.substr(0,term_pos);

			string uptfs = line.substr(term_pos+1);
			int dtf_pos = uptfs.find("|");

			vector<boost::tuple<std::string, int> > tfs;

			while( dtf_pos != npos )
			{
				string dtf = uptfs.substr(0,dtf_pos);
				int d_pos = dtf.find("@");
				string d = dtf.substr(0,d_pos);
				string tf_s = dtf.substr(d_pos+1);
				int tf = atoi( tf_s.c_str() );

				tfs.push_back( boost::tuple<std::string, int>( d, tf ) );

				uptfs = uptfs.substr(dtf_pos+1);
				dtf_pos = uptfs.find("|");
			}

			invertedIndex.insert( pair<string, vector<boost::tuple<std::string, int> > >( term, tfs ) );
		}
		else if(termFilesize_pos != npos)	// get filesizes map
		{
			string file = line.substr(0,termFilesize_pos);

			string filesize_s = line.substr(termFilesize_pos+1);
			int filesize = atoi( filesize_s.c_str() );

			fileSizes.insert( pair<string,double>( file, filesize ) );		
		}
		else if(total_pos != npos)	// get total documents number
		{
			string total_documents_s = line.substr(0,total_pos);
			total_documents = atoi( total_documents_s.c_str() );
		}
		else if(avg_filesize_pos != npos)	// get average file size
		{
			string avg_filesize_s = line.substr(0,avg_filesize_pos);
			avg_file_size = atoi( avg_filesize_s.c_str() );
		}
	}
	
	return 1;
}

// calculates the review's feature -> [term, term's_bm25_value]
map<string,double> InvertedIndexer::getFeature(string filename, vector<string> query_vector)
{
	map<string,double> feature;

	// for every query term
	for(int j=0; j<query_vector.size(); j++)
	{
		string term = query_vector.at(j);		

		int document_frequency = 0.0;
		double idf = 0.0;
		int tf = 0.0;
		double tf_idf = 0.0;	
		int file_size = 0;
		double bm25_t = 0.0;

		// if query term is in inverted index 
		map< string, vector<boost::tuple<std::string, int> > >::iterator iter = invertedIndex.find(term);
		if( iter != invertedIndex.end() )
		{
			vector<boost::tuple<std::string, int> > *uptfs;
			uptfs = &iter->second;

			// get df_t
			document_frequency = uptfs->size();

			for(int j=0; j < uptfs->size(); j++)
			{
				if( (filename) == uptfs->at(j).get<0>() )
				{
					// get tf_t
					tf = uptfs->at(j).get<1>();
					break;
				}
			}
		}
		else
		{
			feature.insert( pair<string, double>( term, 0.0 ) );

			continue;
		}

		map<string, int>::iterator iter2 = fileSizes.find( (filename) );
		if( iter2 != fileSizes.end() )
		{		
			// get file length
			file_size = iter2->second;
		}

		// calculate IDF
		idf = log( double(total_documents) / double(document_frequency) );

		double k1 = 1.2;
		double b = 0.75;

		double part2 = (double(tf)*(k1+1)) / ( k1*((1-b)+b*(double(file_size)/avg_file_size))+double(tf) );

		bm25_t = (idf * part2);

		feature.insert( pair<string, double>( term, bm25_t ) );		
	}

	return feature;
}

	
// calculates the BM25 value of the review file to the query vector
double InvertedIndexer::getBM25(string filename, vector<string> query_vector)
{
	double bm25 = 0.0;

	// for every query term
	for(int j=0; j<query_vector.size(); j++)
	{
		string term = query_vector.at(j);		

		int document_frequency = 0.0;
		double idf = 0.0;
		int tf = 0.0;
		double tf_idf = 0.0;	
		int file_size = 0;
		double bm25_t = 0.0;

		// if query term is in inverted index 
		map< string, vector<boost::tuple<std::string, int> > >::iterator iter = invertedIndex.find(term);
		if( iter != invertedIndex.end() )
		{
			vector<boost::tuple<std::string, int> > *uptfs;
			uptfs = &iter->second;
			
			// get df_t
			document_frequency = uptfs->size();

			for(int j=0; j < uptfs->size(); j++)
			{
				if( (filename) == uptfs->at(j).get<0>() )
				{
					// get tf_t
					tf = uptfs->at(j).get<1>();
			
					break;
				}
			}
		}
		else
		{
			continue;
		}

		map<string, int>::iterator iter2 = fileSizes.find( (filename) );
		if( iter2 != fileSizes.end() )
		{		
			// get file length
			file_size = iter2->second;
		}

		// calculate IDF
		idf = log( double(total_documents) / double(document_frequency) );

		double k1 = 1.2;
		double b = 0.75;

		double part2 = (double(tf)*(k1+1)) / ( k1*((1-b)+b*(double(file_size)/avg_file_size))+double(tf) );

		bm25_t = (idf * part2);

		bm25 = bm25 + bm25_t;	
	}

	return bm25;
}

// calculate BM25A of review file to query vector
double InvertedIndexer::getBM25A(string filename, vector<string> query_vector, map<string, double> aspects_score)
{
	double bm25 = 0.0;
					// for every query term
	for(int j=0; j<query_vector.size(); j++)
	{
		string term = query_vector.at(j);		

		int document_frequency = 0.0;
		double idf = 0.0;
		int tf = 0.0;
		double tf_idf = 0.0;	
		int file_size = 0;
		double bm25_t = 0.0;
								// if query term is in inverted index 
		map< string, vector<boost::tuple<std::string, int> > >::iterator iter = invertedIndex.find(term);
		if( iter != invertedIndex.end() )
		{
			vector<boost::tuple<std::string, int> > *uptfs;
			uptfs = &iter->second;
			document_frequency = uptfs->size();	// get df_t

			for(int j=0; j < uptfs->size(); j++)
			{
				if( (filename) == uptfs->at(j).get<0>() )
				{
					tf = uptfs->at(j).get<1>();	// get tf_t
					break;
				}
			}
		}
		else
		{
			continue;
		}

		map<string, int>::iterator iter2 = fileSizes.find( (filename) );
		if( iter2 != fileSizes.end() )
		{		
			file_size = iter2->second;	// get file length
		}


		idf = log( double(total_documents) / double(document_frequency) ); // calculate IDF

		double k1 = 1.2;
		double b = 0.75;

		double part2 = (double(tf)*(k1+1)) / ( k1*((1-b)+b*(double(file_size)/avg_file_size))+double(tf) );

		bm25_t = (idf * part2);

		double scoreA = 0.0;

		map<string, double>::iterator iterA = aspects_score.find( term );
		if( iterA != aspects_score.end() )
		{		
			scoreA = iterA->second;	// get term percent
		}

		bm25 = bm25 + ( bm25_t * scoreA );	
	}

	return bm25;
}

// calculate sentiBM25A of review file to query 
double InvertedIndexer::getSentiBM25A(string filename, string entity, vector<string> query, map<string, double> aspects_score)
{
	double senti_bm25 = 0.0;

	map<string, double> sentis;
	double aspect_senti=0.0;
	double aspect_counter=0.0;

	string sentiment_file = "sentimentsOffline/car_2009sentiments/" + entity;

										// load sentiment values to memory
	map<string,double> my_sentiments = FileMapIO::fileToSentiMap(sentiment_file);
	for (map<string,double>::iterator it=my_sentiments.begin(); it!=my_sentiments.end(); ++it)
	{
		sentis.insert( pair<string, double>( it->first, it->second ) );
		aspect_counter++;
	}
				// for every query term
	for(int i=0; i<query.size()-1; i++)
	{
		string term = query.at(i);

		int document_frequency = 0.0;
		double idf = 0.0;
		int tf = 0.0;
		double tf_idf = 0.0;	
		int file_size = 0;
		double senti_bm25_t = 0.0;
		double bm25_t = 0.0;
									// if query term is in inverted index 
		map< string, vector<boost::tuple<std::string, int> > >::iterator iter = invertedIndex.find(term);
		if( iter != invertedIndex.end() )
		{
			vector<boost::tuple<std::string, int> > *uptfs;
			uptfs = &iter->second;
			document_frequency = uptfs->size();	// get df_t

			for(int j=0; j < uptfs->size(); j++)
			{
				if( (filename) == uptfs->at(j).get<0>() )
				{
					tf = uptfs->at(j).get<1>();	// get tf_t
					break;
				}
			}
		}
		else
		{
			continue;
		}

		map<string, int>::iterator iter2 = fileSizes.find( (filename) );
		if( iter2 != fileSizes.end() )
		{		
			file_size = iter2->second;	// get file length
		}
								// calculate TF-IDF
		idf = log( double(total_documents) / double(document_frequency) );

		double k1 = 1.2;
		double b = 0.75;
						// if query term has a sentiment value
		map< string, double>::iterator iter_se = sentis.find(term);
		if( iter_se != sentis.end() )
		{
			aspect_senti = iter_se->second;	// get it
		}

		double scoreA = 0.0;

		map<string, double>::iterator iterA = aspects_score.find( term );
		if( iterA != aspects_score.end() )
		{		
			scoreA = iterA->second;	// get term percent
		}

		double part2 = (double(tf)*(k1+1)) / ( k1*((1-b)+b*(double(file_size)/avg_file_size))+double(tf) );

		bm25_t = (idf * part2);

		senti_bm25_t = ( bm25_t * scoreA * aspect_senti );

		senti_bm25 = senti_bm25 + senti_bm25_t;
	}

	return senti_bm25;
}

// calculate sentiBM25 of review file to query 
double InvertedIndexer::getSentiBM25(string filename, vector<string> query, string sentiment_file)
{
	double senti_bm25 = 0.0;

	map<string, double> sentis;
	double aspect_senti=0.0;
	double aspect_counter=0.0;
										// load sentiment values to memory
	map<string,double> my_sentiments = FileMapIO::fileToSentiMap(sentiment_file);
	for (map<string,double>::iterator it=my_sentiments.begin(); it!=my_sentiments.end(); ++it)
	{
		sentis.insert( pair<string, double>( it->first, it->second ) );
		aspect_counter++;
	}
				// for every query term
	for(int i=0; i<query.size()-1; i++)
	{
		string term = query.at(i);

		int document_frequency = 0.0;
		double idf = 0.0;
		int tf = 0.0;
		double tf_idf = 0.0;	
		int file_size = 0;
		double senti_bm25_t = 0.0;
									// if query term is in inverted index 
		map< string, vector<boost::tuple<std::string, int> > >::iterator iter = invertedIndex.find(term);
		if( iter != invertedIndex.end() )
		{
			vector<boost::tuple<std::string, int> > *uptfs;
			uptfs = &iter->second;
			document_frequency = uptfs->size();	// get df_t

			for(int j=0; j < uptfs->size(); j++)
			{
				if( (filename) == uptfs->at(j).get<0>() )
				{
					tf = uptfs->at(j).get<1>();	// get tf_t
					break;
				}
			}
		}
		else
		{
		//	cout << "query term " <<  term << " can not found" << endl;
			continue;
		}

		map<string, int>::iterator iter2 = fileSizes.find( (filename) );
		if( iter2 != fileSizes.end() )
		{		
			file_size = iter2->second;	// get file length
		}
								// calculate TF-IDF
		idf = log( double(total_documents) / double(document_frequency) );

		double k1 = 1.2;
		double b = 0.75;
						// if query term has a sentiment value
		map< string, double>::iterator iter_se = sentis.find(term);
		if( iter_se != sentis.end() )
		{
			aspect_senti = iter_se->second;	// get it
		}

		tf = aspect_senti;	// normalize sentiment value with tf_t

		double part2 = (double(tf)*(k1+1)) / ( k1*((1-b)+b*(double(file_size)/avg_file_size))+double(tf) );
		senti_bm25_t = (idf * part2 );

		senti_bm25 = senti_bm25 + senti_bm25_t;
	}

	return senti_bm25;
}


map<string,vector< boost::tuple<string, double> > > InvertedIndexer::getP_ri_ds(vector<string> CL, vector< boost::tuple<string, string> > judge, vector<string> aspects)
{
	map<string, vector< boost::tuple<string, double> > > P_ri_d;

	map<string, vector< boost::tuple<string, double> > > score_d_ris;
	for(int i=0; i<CL.size(); i++)
	{
		string doc = CL.at(i);

		vector< boost::tuple<string, double> > score_d_ris_vec;
		for(vector< boost::tuple<string, string> >::const_iterator iter = judge.begin(); iter != judge.end(); ++iter)
		{
			string entity = iter->get<0>();

			double score_d_ri = 0.0;
			score_d_ri = calculate_score_d_ri(doc, entity, aspects);

			score_d_ris_vec.push_back( boost::tuple<std::string, double>( entity, score_d_ri ) );
		}

		score_d_ris.insert( pair<string, vector< boost::tuple<string, double> > >( doc, score_d_ris_vec ) );		
	}

	for(vector< boost::tuple<string, string> >::const_iterator iter2 = judge.begin(); iter2 != judge.end(); ++iter2)
	{
		string ri = iter2->get<0>();

		vector< boost::tuple<string, double> > P_ri_ds;
		for(int i=0; i<CL.size(); i++)
		{
			string d = CL.at(i);	

			double p_ri_d = 0.0;

			map< string, vector<boost::tuple<std::string, double> > >::iterator iter_f = score_d_ris.find(d);
			if( iter_f != score_d_ris.end() )
			{
				vector< boost::tuple<string, double> > score_d_ris_vec = iter_f->second;
				p_ri_d = calculate_P_ri_d(ri, &score_d_ris_vec); 
			}

			P_ri_ds.push_back( boost::tuple<std::string, double>( d, p_ri_d ) );
		}

		P_ri_d.insert( pair<string, vector< boost::tuple<string, double> > >( ri, P_ri_ds ) );
	}

	return P_ri_d;
}

double InvertedIndexer::calculate_P_ri_d(string ri, vector< boost::tuple<string, double> > *score_d_ris)
{
	double p_ri_d = 0.0;

	double score_d_ri = 0.0;
	double sum_score_d_ris = 0.0;

	for(int i=0; i<score_d_ris->size(); i++)
	{
		if( score_d_ris->at(i).get<0>() == ri )
		{
			score_d_ri = score_d_ris->at(i).get<1>();
		}
		else
		{
			sum_score_d_ris = sum_score_d_ris + score_d_ris->at(i).get<1>();		
		}
	}
	
	if(sum_score_d_ris != 0)
	{
		p_ri_d = ( score_d_ri / sum_score_d_ris );
	}

	return p_ri_d;
}

double InvertedIndexer::calculate_score_d_ri(string d, string entity, vector<string> aspects)
{
	double score_d_ri = 0.0;

	string sentiment_file = "sentimentsOffline/car_2008sentiments/" + entity;
	map<string,double> my_sentiments = FileMapIO::fileToSentiMap(sentiment_file);

					// for every query term
	for(int i=0; i<aspects.size(); i++)
	{
		string term = aspects.at(i);		

		int document_frequency = 0.0;
		double idf = 0.0;
		int tf = 0.0;
		double tf_idf = 0.0;	
		int file_size = 0;
		double bm25_t = 0.0;
		double sa = 0.0;

		map<string,double>::iterator it=my_sentiments.find(term);
		if( it != my_sentiments.end() )
		{	
			sa = it->second;	

			if(sa == 0.0)
			{
				continue;
			}	
		}

								// if query term is in inverted index 
		map< string, vector<boost::tuple<std::string, int> > >::iterator iter = invertedIndex.find(term);
		if( iter != invertedIndex.end() )
		{
			vector<boost::tuple<std::string, int> > *uptfs;
			uptfs = &iter->second;
			document_frequency = uptfs->size();	// get df_t

			for(int j=0; j < uptfs->size(); j++)
			{
				if( (d) == uptfs->at(j).get<0>() )
				{
					tf = uptfs->at(j).get<1>();	// get tf_t
					break;
				}
			}
		}
		else
		{
			continue;
		}

		map<string, int>::iterator iter2 = fileSizes.find( (d) );
		if( iter2 != fileSizes.end() )
		{		
			file_size = iter2->second;	// get file length
		}


		idf = log( double(total_documents) / double(document_frequency) ); // calculate IDF

		double k1 = 0.1;
		double b = 0.75;

		double part2 = (double(tf)*(k1+1)) / ( k1*((1-b)+b*(double(file_size)/avg_file_size))+double(tf) );

		bm25_t = (idf * part2);

		score_d_ri = score_d_ri + ( bm25_t * sa);	
	}

	return score_d_ri;
}

double InvertedIndexer::Log2( double n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2 );  
}








