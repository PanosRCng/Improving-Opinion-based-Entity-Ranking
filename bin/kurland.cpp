#include <iostream>
#include "kurland.h"
#include "fileMapIO.h"
#include <math.h>

using namespace std;

Kurland::Kurland( )
{
	//	
}

Kurland::~Kurland()
{
	//
}

vector< boost::tuple<string, double> > Kurland::getP_q_ds(vector<string> docs,
			  map<int,vector<string> > clusters,
			  map<string,vector<double> > lists, string feature_dir)
{		
	vector< boost::tuple<string, double> > scores;

	// calculate first F_d_q for all documents 
	map<string, double> F_d_qs;
	for(int i=0; i<docs.size(); i++)
	{
		string d = docs[i];
		double f_d_q = calculate_F_d_q(d, &lists);
		F_d_qs.insert( pair<string,double>(d, f_d_q) );
	}
	cout << "calculate F_d_qs .... [ok]" << endl;

	// calculate first documents similarities matrix
	map<string,double> similarities;
	for(int i=0; i<docs.size(); i++)
	{
		string d = docs[i];

		for(int j=0; j<docs.size(); j++)
		{
			string dl = docs[j];

			if(d!=dl)
			{
				string ddl = d+dl;
				double similarity = getSimilarity(feature_dir+d, feature_dir+dl);

				similarities.insert( pair<string,double> (ddl, similarity) );
			}
		}
	}
	cout << "calculate similarities ...... [ok]" << endl;

	double l = 0.9;
	double P_q_d = 0.0;

	for(int i=0; i<docs.size(); i++)
	{
		string d = docs[i];
		double P_d_q = 0.0;
		double part2 = 0.0;

		P_d_q = calculate_P_d_q(d, &docs, &F_d_qs);

		map<int, vector<string> >::iterator it;
        	for (it = clusters.begin(); it != clusters.end(); ++it)
		{
			double P_c_q = 0.0;
			double P_d_c = 0.0;

			int cluster = it->first;

			P_c_q = calculate_P_c_q(cluster, &clusters, &F_d_qs); 
			
			P_d_c = calculate_P_d_c(d, cluster, &clusters, &docs, &similarities);

			part2 = part2 + (P_c_q + P_d_c); 
		}

		double P_q_d = ( (1-l) * P_d_q ) + ( l * part2 );

		scores.push_back( boost::tuple<string, double>( d, P_q_d ) );
	}

	return scores;
}

double Kurland::calculate_P_d_q(string d, vector<string>* docs, map<string,double>* F_d_qs)
{
	double P_d_q = 0.0;
	double F_d_q = 0.0;
	double sum_F_dcl_q = 0.0;

	for(int i=0; i<docs->size(); i++)
	{
		if( d == docs->at(i) )
		{
			map<string,double>::iterator it = F_d_qs->find(d);			
			F_d_q = it->second;	
		}
		else
		{
			string dcl = docs->at(i);
			double F_dcl_q = 0.0;
		
			map<string,double>::iterator it = F_d_qs->find(dcl);			
			F_dcl_q = it->second;

			sum_F_dcl_q = sum_F_dcl_q + F_dcl_q;
		}
	}

	if(sum_F_dcl_q != 0)
	{
		P_d_q = ( F_d_q / sum_F_dcl_q );
	}

	return P_d_q;
}

double Kurland::calculate_P_c_q(int cluster, map<int,vector<string> > *clusters, map<string,double> *F_d_qs)
{
	double P_c_q = 0.0;

	double pro_F_d_q = 1;
	double sum_pro_F_d_q = 0.0;

	map<int,vector<string> >::iterator it_f = clusters->find(cluster);
	if(it_f != clusters->end())
	{
		vector<string> c_ds = it_f->second;

		for(int i=0; i<c_ds.size(); i++)
		{
			map<string,double>::iterator it_fdq = F_d_qs->find(c_ds[i]);			
			double F_d_q = it_fdq->second;

			pro_F_d_q = pro_F_d_q * F_d_q;
		}
	}

	map<int, vector<string> >::iterator it;
        for (it = clusters->begin(); it != clusters->end(); ++it)
	{
		if(it->first != cluster)
		{
			double procl_F_d_q = 1;

			vector<string> cl_ds = it_f->second;
			for(int i=0; i<cl_ds.size(); i++)
			{
				// if d' != d

				map<string,double>::iterator it_fdq = F_d_qs->find(cl_ds[i]);			
				double F_d_q = it_fdq->second;

				procl_F_d_q = procl_F_d_q * F_d_q;
			}			

			sum_pro_F_d_q = sum_pro_F_d_q + procl_F_d_q;
		}
	}

	if( sum_pro_F_d_q != 0 )
	{
        	P_c_q = ( pro_F_d_q / sum_pro_F_d_q );
	}

	return P_c_q;
}

double Kurland::calculate_P_d_c(string d, int cluster, map<int,vector<string> > *clusters, vector<string>* docs, map<string,double>* similarities)
{
	double P_d_c = 0.0;
	double sum_sim = 0.0;
	double sum_sum_sim = 0.0;

	vector<string> c_ds;

	map<int,vector<string> >::iterator it = clusters->find(cluster);
	if(it != clusters->end())
	{
		c_ds = it->second;

		for(int i=0; i<c_ds.size(); i++)
		{
			string dc = c_ds.at(i);
			if( d != dc )
			{
				string ddl = d+dc;
				map<string,double>::iterator it_simi = similarities->find(ddl);	
				double similarity = it_simi->second;		

				sum_sim = sum_sim + similarity;
			}
		}
	}

	for(int i=0; i<docs->size(); i++)
	{
		string d_cl = docs->at(i);

		double sum_sim_cl = 0.0;

		for(int i=0; i<c_ds.size(); i++)
		{
			string d_c = c_ds.at(i);

			if( d_cl != d_c )
			{
				string ddl = d_cl+d_c;
				map<string,double>::iterator it_simi = similarities->find(ddl);	
				double similarity = it_simi->second;

				sum_sim_cl = sum_sim_cl + similarity;
			}			
		}

		sum_sum_sim = sum_sum_sim + sum_sim_cl;
	}

	if(sum_sum_sim!=0)
	{
		P_d_c = ( sum_sim / sum_sum_sim );
	}

	return P_d_c;
}

double Kurland::calculate_F_d_q(string entity, map<string,vector<double> >* lists)
{
	double F_d_q = 0.0;

	map<string,vector<double> >::iterator it = lists->find(entity);

	if(it != lists->end())
	{
		vector<double> SL_d = it->second;

		for(int i=0; i<SL_d.size(); i++)
		{
			F_d_q = F_d_q + SL_d.at(i);
		}
	}

	return F_d_q;
}

double Kurland::getSimilarity(string a, string b)
{
	double sim = 0.0;

	map<string,double> feature_a;
	feature_a = FileMapIO::fileToSentiMap(a);
	vector<double> feature_vector_a;
	for (map< string,double >::iterator it_a=feature_a.begin(); it_a!=feature_a.end(); ++it_a)
	{
		feature_vector_a.push_back(it_a->second);
	}

	map<string,double> feature_b;
	feature_b = FileMapIO::fileToSentiMap(b);
	vector<double> feature_vector_b;
	for (map< string,double >::iterator it_b=feature_b.begin(); it_b!=feature_b.end(); ++it_b)
	{
		feature_vector_b.push_back(it_b->second);
	}

	sim = cosineSimilarity(feature_vector_a, feature_vector_b);

	return sim;
}

double Kurland::cosineSimilarity( vector<double> a, vector<double> b)
{
	double similarity=0.0;
	double product = 0.0;

	if( (eucl_norm(a)!=0) && (eucl_norm(b)!=0) )
	{
		similarity = dot_product(a, b) / ( eucl_norm(a) * eucl_norm(b) );
	}	

	return similarity;
}

double Kurland::dot_product(vector<double> a, vector<double> b)
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

double Kurland::eucl_norm(vector<double> a)
{
	double norm_v = 0.0;

	for(int i=0; i<a.size(); i++)
	{
		norm_v += pow(a[i],2);
	}

	norm_v = sqrt(norm_v);

	return norm_v;
}






