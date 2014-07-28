#ifndef _KURLAND_H_
#define _KURLAND_H_

#include <vector>
#include <map>
#include "boost/tuple/tuple.hpp"

class Kurland{

	public:
		Kurland();
		~Kurland();

		std::vector< boost::tuple<std::string, double> > getP_q_ds(std::vector<std::string>,
						                              std::map<int,std::vector<std::string> >,
				 			                      std::map<std::string,std::vector<double> >,
										std::string);

	private:
		double calculate_P_d_q(std::string,
				       std::vector<std::string> *,
	   			       std::map<std::string,double> *);

		double calculate_P_c_q(int,
				       std::map<int,std::vector<std::string> > *,
				       std::map<std::string,double> *);

		double calculate_P_d_c(std::string,
				       int,
				       std::map<int,std::vector<std::string> > *,
				       std::vector<std::string> *,
				       std::map<std::string,double> *);

		double calculate_F_d_q(std::string, std::map<std::string,std::vector<double> > *);

		double getSimilarity(std::string, std::string);
		double cosineSimilarity( std::vector<double>, std::vector<double> );
		double dot_product(std::vector<double>, std::vector<double>);
		double eucl_norm(std::vector<double>);
		
};


#endif
