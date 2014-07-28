#include "scorer.h"

using namespace std;

Scorer::Scorer()
{
	utils = new Utils();
}

Scorer::~Scorer()
{
	//
}

bool mycompare(const boost::tuple<string,double> &lhs, const boost::tuple<string,double> &rhs)
{
	return lhs.get<1>() > rhs.get<1>();
}

// calculates the nDCG value of the ranking list, on the top 10 results
double Scorer::getNDCG(vector< boost::tuple<string, double > > c_scores, vector< boost::tuple<string, double > > i_scores)
{
	double DCG = 0.0;
	double nDCG = 0.0;
	double iDCG = 0.0;

	// sorting of ranklist based on our scores
	sort(c_scores.begin(),c_scores.end(),mycompare); 

	// get the top 10
	vector< boost::tuple<string, double > > scores;
	for(int i=0; i<10; i++)
	{
		scores.push_back( c_scores.at(i) );
	}

	// get the ideal scores for our ranklist
	for(int i=0; i<scores.size(); i++)
	{
		for(int j=0; j<i_scores.size(); j++)
		{
			if( scores.at(i).get<0>() == i_scores.at(j).get<0>() )
			{
				scores.at(i).get<1>() = i_scores.at(j).get<1>();
				break;
			}
		}
	}

	// get the DCG
	DCG = calculateDCG(scores);

	// best score to all ideal score entries, else you get extremely very high nDCG
	for(int i=0; i<scores.size(); i++)
	{
		scores.at(i).get<1>() = 10.0;
	}

	// get the iDCG
	iDCG = calculateDCG(scores);

	// calculate the nDCG
	nDCG = DCG / iDCG;

	return nDCG;
}

// calculates the DCG
double Scorer::calculateDCG(vector< boost::tuple<string, double > > scores)
{
	double DCG = 0.0;

	DCG = scores.at(0).get<1>();
	for(int i = 1; i < scores.size(); i++)
	{
		DCG = DCG + ( scores.at(i).get<1>() / utils->Log2(i+1) );
	}

	return DCG;
}



