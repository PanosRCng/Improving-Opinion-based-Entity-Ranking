#include <iostream>
#include "wordNet.h"
#include "nltk_similarity.hh"

using namespace std;
using namespace wnb;

// loads WordNet
WordNet::WordNet(string wordnet_path)
{
	wn = new wnb::wordnet(wordnet_path);
}

WordNet::~WordNet()
{
	//
}

// returns a string vector with synonyms terms for a given term
vector<string> WordNet::getSynonyms(string term)
{
	vector<string> synonyms;

	vector<synset> synsets_term = wn->get_synsets(term);

	for(int i=0; i<synsets_term.size(); i++)
	{
		vector<string> words = synsets_term[i].words;

		for(int j=0; j<words.size(); j++)
		{
			int insert_flag = true;

			for(int k=0; k<synonyms.size(); k++)
			{
				if(synonyms[k] == words[j])
				{
					insert_flag = false;
					break;
				}
			}

			if(insert_flag)
			{				
				synonyms.push_back( words[j] );
			}
		}
	}

	return synonyms;
}

vector<string> WordNet::synonymExpansion(vector<string> terms)
{
	vector<string> exp_terms;

	map<string,int> exp_map;
	vector<string> exp_vector;

        for(int i=0; i<terms.size(); i++)
        {
		string term = terms.at(i);
                vector<string> synonyms = getSynonyms(term);

		exp_vector.push_back(term);
		for(int i=0; i<synonyms.size(); i++)
		{
			exp_vector.push_back(synonyms.at(i));
		}
        }

        for(int i=0; i<exp_vector.size(); i++)
        {
		pair<map<string,int>::iterator,bool> ret;
		ret = exp_map.insert( pair<string,int>( exp_vector.at(i), 1 ) );
		if (ret.second==false)
		{
			//
		}
        }

	for(map<string, int>::const_iterator iter_s= exp_map.begin(); iter_s!=exp_map.end(); ++iter_s)
	{
		exp_terms.push_back(iter_s->first);
	}

	return exp_terms;
}

vector<string> WordNet::getGlosses(string term)
{
	vector<string> glosses;

	vector<synset> synsets_term = wn->get_synsets(term);	

	for(int i=0; i<synsets_term.size(); i++)
	{
		glosses.push_back(synsets_term[i].gloss);
	}

	return glosses;
}

float WordNet::getSimilarity(string a, string b)
{
	float sim = 0.0;

	vector<synset> synsets1 = wn->get_synsets(a);	
	vector<synset> synsets2 = wn->get_synsets(b);

	if( (synsets1.size() > 0) && (synsets2.size() > 0) )
	{
        	nltk_similarity similarity(*wn);
		sim = similarity(synsets1[0], synsets2[0], 6);
	}

	return sim;
}
