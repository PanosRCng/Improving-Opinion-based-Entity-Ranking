#ifndef _INVERTEDINDEXER_H_
#define _INVERTEDINDEXER_H_

#include <vector>
#include <boost/tuple/tuple.hpp>
#include "tagger.h"
#include "utils.h"

class InvertedIndexer{

	public:
		InvertedIndexer();

		~InvertedIndexer();

		int buildInvertedIndex(std::vector<std::string>*, Tagger*, std::map<std::string, int>*);
		/*
			builds an inverted index for the files in directory (the files must be parsed first -> use the utils->parse_reviews() )

			(*) inverted index entry-> 	[ term, [(doc1|tf1), (doc2|tf2), (doc3|tf3)] ]
			(*)   also calculates the average file size (the average terms number for the files)

			1 - a pointer to a vector of strings that contains the files of the directory
			2 - a pointer to the POS tagger instance
			3 - a pointer to a map of strings that contains all the possible terms			
		*/

		std::map<std::string, int> buildPhrasesInvertedIndex(std::vector<std::string>*,
					      Tagger*,
					      std::vector<std::string>*,
					      std::vector<std::string>*,
					      std::vector<std::string>*);

		double getBM25(std::string, std::vector<std::string>);
		/*
			calculates the BM25 value of the review file to the query vector

			1 - the review file path as a string
			2 - the query as a vector of strings
		*/

		double getBM25A(std::string, std::vector<std::string>, std::map<std::string, double>);

		double getSentiBM25A(std::string filename, std::string entity, std::vector<std::string>, std::map<std::string, double>);

		double getSentiBM25(std::string entity, std::vector<std::string>, std::string);
		/*
			calculates the sentiBM25 value of the review file to the query vector

			1 - the review file path as a string
			2 - the query as a vector of strings
			3 - the path to the pre-calculated sentiments as a string
		*/

		std::map<std::string, double> getFeature(std::string, std::vector<std::string>);
		/*
			calculates the review's feature -> [term, term's_bm25_value]

			1 - the review file path
			2 - the aspect terms as a vector of string -> these represent the feature space
		*/

		std::map<std::string, std::vector< boost::tuple<std::string, double> > > getP_ri_ds(std::vector<std::string>, 
									    std::vector< boost::tuple<std::string, std::string> >,
										std::vector<std::string>);

		int InvertedIndexToFile(std::string);
		/*
			saves the inverted index to file

			1 - the path to save the inverted index as string
		*/

		int fileToInvertedIndex(std::string);
		/*
			loads inverted index to memory from file

			1 - the inverted index's file path
		*/

		int InvertedIndexPhrasesToFile(std::map<std::string, int>, std::string);

		std::map<std::string,int> fileToInvertedIndexPhrases(std::string); 

		void getFeatures(std::string, Tagger*);

		std::map<std::string, double> calculateSOS(std::map<std::string, int>*); 

	private:
		int getdir(std::string dir, std::vector<std::string> &files);

		std::map< std::string, std::vector<boost::tuple<std::string, int> > > invertedIndex;

		double calculate_P_ri_d(std::string, std::vector< boost::tuple<std::string, double> >*);

		double calculate_score_d_ri(std::string, std::string, std::vector<std::string>);

		double Log2( double n );

		std::vector<std::string> getTerms(std::string, Tagger*);
		/*
			returns the terms of a line as a vector of strings

			(*) use the pos tagger to locate only the possible aspect terms (NN, JJ, JJR, JJS, NNS, NNP, NNPS) 

			1 - the line as string
			2 - pointer to the POS tagger isntance
		*/


		Utils *utils;

		int total_documents;

		std::map<std::string, int> fileSizes;

		double avg_file_size;

		std::vector<std::string> stopwords;

		void loadStopWords(void);

};


#endif
