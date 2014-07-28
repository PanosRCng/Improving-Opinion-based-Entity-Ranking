#ifndef _TAGGER_H_
#define _TAGGER_H_

#include <stdio.h>
#include <fstream>
#include <map>
#include <list>
#include <set>
#include <iomanip>
#include <iostream>
#include <cfloat>
#include <sstream>
#include "crf.h"
#include "common.h"
#include <sys/time.h>
#include "boost/tuple/tuple.hpp"

//using namespace std;

class Tagger{

	public:
		Tagger();
		~Tagger();
		std::vector<std::string> do_tagging(std::string filename);
		std::vector<std::string> do_sentence_tagging(std::string line);


		std::vector<std::string> do_sentence_tagging_for_aspects(std::string line);
		/*
			use the pos tagger to locate only the possible aspect terms (NN, JJ, JJR, JJS, NNS, NNP, NNPS) 

			1 - the line as a string
		*/


		std::vector< boost::tuple<std::string, std::string> > do_sentence_tagging_map(std::string);
};


#endif
