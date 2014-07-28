#include "knearest.h"
#include <iostream>
#include <boost/tuple/tuple.hpp>

using namespace std;

Knearest::Knearest()
{
	//
}

Knearest::~Knearest()
{
	//
}

bool mcompare(const boost::tuple<string,double> &lhs, const boost::tuple<string,double> &rhs)
{
	return lhs.get<1>() > rhs.get<1>();
}

// construct clusters on the documents using the knearest algorithm (modified to construct overlapping clusters - Kurland paper)
map<int, vector<string> > Knearest::do_knearest(string features_path, int k)
{
	map<int, vector<string> > clusters;

	Utils* utils = new Utils();

	vector<string> features = vector<string>();
	utils->getFilesList(features_path, features, false);

	// calculate first the documents similarity matrix 
	map<string,double> similarities;

	// (!) improve here --> (i,j) is the same as (j,i)  
	for(int i=0; i<features.size(); i++)
	{
		string d = features[i];

		for(int j=0; j<features.size(); j++)
		{
			string dl = features[j];

			if(d!=dl)
			{
				string ddl = d+dl;

				// calculate the similarity of document d with all the other documents in the set
				double similarity = utils->getSimilarity(features_path+d, features_path+dl);

				similarities.insert( pair<string,double> (ddl, similarity) );
			}
		}
	}

	// get the clusters
	clusters = getKnearests(&features, &similarities, k, utils);

	return clusters;
}


// constructs the clusters
map<int, vector<string> > Knearest::getKnearests(vector<string> *features, map<string, double> *similarities, int k, Utils* utils)
{
	map<int, vector<string> > clusters;

	int cluster_counter = 0;

	for(int i=0; i<features->size(); i++)
	{
		string d = features->at(i);
		vector<string> maxs;
		vector< boost::tuple<string, double> > scores;

		cluster_counter++;

		// get all similarities
		for(int j=0; j<features->size(); j++)
		{
			string dl = features->at(j);

			if(d!=dl)
			{
				string ddl = d+dl;
				map<string,double>::iterator it_simi = similarities->find(ddl);	
				double similarity = it_simi->second;

				scores.push_back( boost::tuple<string,double>(dl, similarity) );
			}
		}

		// get sorted similarities
		sort(scores.begin(),scores.end(),mcompare); 

		// get documents with max similarities
		for(int f=0; f<k; f++)
		{
			maxs.push_back( scores.at(f).get<0>() );
		}

		// get a cluster
		clusters.insert( pair<int, vector<string> > (cluster_counter, maxs) );
	}

	return clusters;
}

