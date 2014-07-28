#ifndef _JUDGMENT_H_
#define _JUDGMENT_H_

#include <string>
#include "boost/tuple/tuple.hpp"
#include <vector>
#include <list>

class Judgment{

	typedef std::vector< boost::tuple<std::string, std::string> > judge_list;
	typedef std::vector< boost::tuple<std::string, std::string> > query_list;

	public:
		Judgment(const char* filename);
		~Judgment();

		std::vector<std::string> get_cat(void);
		/*
			returns the query category
		*/

		judge_list get_judge(void);
		/*
			returns the vector with all entities and their relevance to the query
		*/

		query_list get_query(void);
		/*
			returns the vector with all identical queries (representing a single query) and their ids
		*/


	// pending delete
	//	int parse_review(const char* entity);
	//	int parse_hotel_review(const char* entity);
		std::vector<boost::tuple<std::string, double > > getIdealScores();

	private:
		judge_list judge;
		/*
			the vector with all entities and their relevance to the query
		*/

		query_list query;
		/*
			the vector with all identical queries (representing a single query) and their ids
		*/

		std::string cat;
		/*
			the query category (it represents all the queries in the query list)
		*/

		int parse_judgment(const char* filename);
		/*
			parse a judgment file to memory

			1 - the path for the judgment file
		*/

		std::list<std::string> getSentences(std::string);

};


#endif
