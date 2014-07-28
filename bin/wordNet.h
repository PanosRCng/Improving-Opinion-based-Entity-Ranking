#ifndef _WORDNET_H_
#define _WORDNET_H_

#include "wordnet.hh"

class WordNet{

	public:
		WordNet(std::string);
		/*
			loads WordNet

			1 - the wordnet path
		*/

		~WordNet();

		float getSimilarity(std::string, std::string);

		std::vector<std::string> getGlosses(std::string);


		std::vector<std::string> getSynonyms(std::string term);
		/*
			returns a string vector with synonyms terms for a given term

			1 - the term to find its synonyms
		*/

		std::vector<std::string> synonymExpansion(std::vector<std::string>);
		/*
			returns a vector with synonym terms for a given vector with terms

			1 - the vector with the terms to expand
		*/


	private:
		wnb::wordnet* wn;
};


#endif
