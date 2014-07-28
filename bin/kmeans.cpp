#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>   
#include <time.h> 
#include <cmath>
#include "kmeans.h"
#include "fileMapIO.h"

using namespace std;

int Kmeans::getdir(string dir, vector<string> &files)
{
	DIR *dp;
	struct dirent *dirp;

	if((dp  = opendir(dir.c_str())) == NULL)
	{
	        cout << "Error(" << errno << ") opening " << dir << endl;
	        return errno;
    	}

	while((dirp = readdir(dp)) != NULL)
	{
        	files.push_back(string(dirp->d_name));
    	}

    	closedir(dp);

    	return 0;
}

Kmeans::Kmeans()
{
	//
}

Kmeans::~Kmeans()
{
	//
}

map<int, vector<string> > Kmeans::do_kmeans(string features_path, int k)
{
	int iterations = 3;

	map<int, vector<string> > clusters;
	map<string, vector<double> > features;

	string dir = string(features_path);
	vector<string> files = vector<string>();
	getdir(dir,files);

	vector<string> points; 

	for(int it=0; it<files.size(); it++)
	{
		if( (files[it]!= ".") && (files[it]!="..") )
		{
			string entity = string(files[it]);

			points.push_back(entity);

			map<string,double> feature;
			feature = FileMapIO::fileToSentiMap(features_path + entity);
			vector<double> feature_vector;
			for (map< string,double >::iterator it_fv=feature.begin(); it_fv!=feature.end(); ++it_fv)
			{
				feature_vector.push_back(it_fv->second);
			}

			features.insert( pair< string,vector<double> >(entity, feature_vector) );
		}
	}

	vector<string> seeds = getSeedCentroids(&points, k);

	map<int, vector<double> > centroids; 
	for(int i=0; i<seeds.size(); i++)
	{
		vector<double> feature_vector = features[seeds.at(i)];
		centroids.insert( pair< int,vector<double> >(i, feature_vector) );
	}	
	
	do
	{
		centroids = getCentroids(centroids, &features);

		iterations--;

	}while( iterations > 0 );

	clusters = getClusters(centroids, &features);

	return clusters;
}

map<int, vector<string> > Kmeans::getClusters( map<int, vector<double> > centroids, map<string, vector<double> > *features )
{
	map<int, vector<string> > clusters;

	for (map< string,vector<double> >::iterator it=features->begin(); it!=features->end(); ++it)
	{
		string d = it->first;
		vector<double> vector_d = it->second;

		int cluster_id;
		double max_sim = 0.0; 

		for (map< int,vector<double> >::iterator it_c=centroids.begin(); it_c!=centroids.end(); ++it_c)
		{
			vector<double> vector_c = it_c->second; 

			double sim = cosineSimilarity(vector_d, vector_c);
	
			if( sim > max_sim )
			{
				max_sim = sim;
				cluster_id = it_c->first;
			}
		}

		map<int,vector<string> >::iterator it_i = clusters.find(cluster_id);
		if(it_i != clusters.end())
		{
			vector<string>* c_ds = &it_i->second;

			c_ds->push_back(d);
		}
		else
		{
			vector<string> c_ds;
			c_ds.push_back(d);

			clusters.insert( pair< int,vector<string> >(cluster_id, c_ds) );
		}				
	}

	return clusters;
}

map<int, vector<double> > Kmeans::getCentroids( map<int, vector<double> > centroids, map<string, vector<double> > *features )
{
	map<int, vector<double> > new_centroids;
	map<int, vector<string> > clusters;
	
	int feature_size;

	for (map< string,vector<double> >::iterator it=features->begin(); it!=features->end(); ++it)
	{
		string d = it->first;
		vector<double> vector_d = it->second;

		feature_size = vector_d.size();

		int cluster_id;
		double max_sim = 0.0; 

		for (map< int,vector<double> >::iterator it_c=centroids.begin(); it_c!=centroids.end(); ++it_c)
		{
			vector<double> vector_c = it_c->second; 

			double sim = cosineSimilarity(vector_d, vector_c);
	
			if( sim > max_sim )
			{
				max_sim = sim;
				cluster_id = it_c->first;
			}
		}

		map<int,vector<string> >::iterator it_i = clusters.find(cluster_id);
		if(it_i != clusters.end())
		{
			vector<string>* c_ds = &it_i->second;

			c_ds->push_back(d);
		}
		else
		{
			vector<string> c_ds;
			c_ds.push_back(d);

			clusters.insert( pair< int,vector<string> >(cluster_id, c_ds) );
		}				
	}

	for (map< int,vector<string> >::iterator it_ce=clusters.begin(); it_ce!=clusters.end(); ++it_ce)
	{
		vector<double> centroid_feature;

		for(int i=0; i<feature_size; i++)
		{
			centroid_feature.push_back(0.0);
		}

		vector<string> ds = it_ce->second;

		if( ds.size() > 0 )
		{
			for(int i=0; i<ds.size(); i++)
			{
//				cout << it_ce->first << " " << ds.at(i) << endl;

				string d_name = ds.at(i);
				vector<double> d_feature = features->at(d_name);

				for(int i=0; i<d_feature.size(); i++)
				{
					centroid_feature[i] = centroid_feature[i] + d_feature[i];
				}
			}

			for(int i=0; i<centroid_feature.size(); i++)
			{
				centroid_feature[i] = ( centroid_feature[i] / ds.size() );
			}
		}	
		
		new_centroids.insert( pair< int,vector<double> >(it_ce->first, centroid_feature) );
	}

	return new_centroids;
}

vector<string> Kmeans::getSeedCentroids( vector<string> *points, int k)
{
	vector<string> seeds;

	srand (time(NULL));

	do
	{
		int seed = rand() % points->size();

		if(seeds.size() == 0)
		{
			seeds.push_back( points->at(seed) );
			k--;
		}
		else
		{
			int flag_insert = 1;
			for(int j=0; j<seeds.size(); j++)
			{
				if( seeds.at(j) == points->at(seed) )
				{
					flag_insert = 0;
				}
			}

			if(flag_insert)
			{
				seeds.push_back( points->at(seed) );
				k--;	
			}
		}
	}
	while( k > 0 );

	return seeds;
}

double Kmeans::cosineSimilarity( vector<double> a, vector<double> b)
{
	double similarity=0.0;
	double product = 0.0;

	if( (eucl_norm(a)!=0) && (eucl_norm(b)!=0) )
	{
		similarity = dot_product(a, b) / ( eucl_norm(a) * eucl_norm(b) );
	}	

	return similarity;
}

double Kmeans::dot_product(vector<double> a, vector<double> b)
{
	double product = 0.0;

	if( a.size() != b.size() )
	{
		cout << "vectors do not have the same size" << endl;

		return 0.0;
	}

	for(int i=0; i<a.size(); i++)
	{
		product += a[i] * b[i];
	}	

	return product;
}

double Kmeans::eucl_norm(vector<double> a)
{
	double norm_v = 0.0;

	for(int i=0; i<a.size(); i++)
	{
		norm_v += pow(a[i],2);
	}

	norm_v = sqrt(norm_v);

	return norm_v;
}




