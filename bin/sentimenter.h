#ifndef _SENTIMENTER_H_
#define _SENTIMENTER_H_

#include <map>
#include <string>
#include "tagger.h"
#include <vector>
#include "boost/tuple/tuple.hpp"

class Sentimenter{

	public:
		Sentimenter();
		~Sentimenter();
		void getSentiment(std::string);
		std::map<std::string, double> do_sentimentAnalysis(std::string, 
							            std::map<std::string, std::vector<std::string> >,
								    Tagger *);
		/*
			performs sentiment analysis

			1 - the review file path as a string
			2 - a map with the aspects and a list of synonyms for every aspect
			3 - a pointer to the pos tagger object 
		*/


		std::map<std::string, double> do_sentimentAnalysis2(std::string, 
							            std::map<std::string, std::vector<std::string> >,
								    Tagger *,
								    std::map<std::string, double> *);
		/*
			performs sentiment analysis using syntactic patterns

			1 - the review file path as a string
			2 - a map with the aspects and a list of synonyms for every aspect
			3 - a pointer to the pos tagger object 
			4 - a pointer to the sentiment oreintations map
		*/	

		std::vector<std::string> getPhrases(std::string, 
						    std::vector<std::string>,
						    Tagger *);
		/*
			extracts opinion phrases using pos tag patterns

			1 - the file path as a string
			2 - the aspects as a vector of strings
			3 - a pointer to the pos tagger object
		*/		

		double getScore(std::string, std::vector<std::string>);

	private:
		std::map<std::string, double> dict;

		int loadSentiWordNet(void);
		/*
			load SentiWordNet

			- looking for file: SentiWordNet_3.0.0_20130122.txt
		*/

		std::map<std::string, int> loadSentiLexicon(void);


		void loadSentimentShifters(void);

		std::vector<std::string> shifters;

		std::vector<std::string> check_pos_patterns(std::vector< boost::tuple<std::string,std::string> > *);
		/*
			checks if a tagged phrase match any of the opinion patterns
			(!) from a 3-term phrase extracts the first and the second term, if there is a match

			1 - the tagged phrase as a vector of tupples (term, tag)
		*/
};


#endif
