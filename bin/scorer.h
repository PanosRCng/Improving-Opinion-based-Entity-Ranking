#ifndef _SCORER_H_
#define _SCORER_H_

#include <vector>
#include <boost/tuple/tuple.hpp>
#include "utils.h"

class Scorer{


	public:
		Scorer();
		~Scorer();
		double getScore(std::string);


		double getNDCG(std::vector< boost::tuple<std::string, double > >, std::vector< boost::tuple<std::string, double > >);
		/*
			calculates the nDCG value of the ranking list, on the top 10 results

			1 - the calculated ranking list
			2 - the ideal ranking list
		*/

	private:
		double calculateDCG(std::vector< boost::tuple<std::string, double > >);
		/*
			calculates the DCG

			1 - the ranking list
		*/

		
		Utils *utils;

};


#endif
