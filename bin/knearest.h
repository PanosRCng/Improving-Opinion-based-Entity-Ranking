#ifndef _KNEAREST_H_
#define _KNEAREST_H_

#include <vector>
#include <map>
#include <string>
#include "utils.h"

class Knearest{

	public:
		Knearest();
		~Knearest();

		std::map<int, std::vector<std::string> > do_knearest(std::string, int);
		/*
			construct clusters on the documents using the knearest algorithm (modified to construct overlapping clusters - Kurland paper)

			1 - the path for the directory that contains the documents' features
			2 - the number of the nearest documents
		*/

	private:

		std::map<int, std::vector<std::string> > getKnearests(std::vector<std::string> *, 
								std::map<std::string, double> *, int, Utils*);
		/*
			constructs the clusters

			1 - pointer to the feature vector
			2 - pointer to the document similarity matrix
			3 - pointer to the Utils's instance
		*/
};


#endif
