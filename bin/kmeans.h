#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <vector>
#include <map>

class Kmeans{

	public:
		Kmeans();
		~Kmeans();
		std::map<int, std::vector<std::string> > do_kmeans(std::string, int );

	private:
		int getdir(std::string dir, std::vector<std::string> &files);

		std::vector<std::string> getSeedCentroids( std::vector<std::string> *, int );

		std::map<int, std::vector<double> > getCentroids( std::map<int, std::vector<double> >,
								  std::map<std::string, std::vector<double> > *);

		std::map<int, std::vector<std::string> > getClusters( std::map<int, std::vector<double> >,
								  std::map<std::string, std::vector<double> > *);

		double cosineSimilarity( std::vector<double>, std::vector<double> );

		double dot_product(std::vector<double>, std::vector<double>);

		double eucl_norm(std::vector<double>);
};


#endif
