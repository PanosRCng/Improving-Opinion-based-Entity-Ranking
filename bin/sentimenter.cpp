#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include "sentimenter.h"
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include "wordNet.h"
#include "fileMapIO.h"
#include <cmath>

using namespace std;
using namespace wnb;

// used for switch statement using string values
enum PosTagValue { JJ,
                   RB,
                   RBR,
                   RBS,
                   NN,
		   NNS };

Sentimenter::Sentimenter()
{
	loadSentiWordNet();

	loadSentimentShifters();
}

Sentimenter::~Sentimenter()
{
	//
}

void Sentimenter::loadSentimentShifters(void)
{
	shifters.push_back("not");
	shifters.push_back("don't");
	shifters.push_back("none");
	shifters.push_back("nobody");
	shifters.push_back("nowhere");
	shifters.push_back("neither");
	shifters.push_back("cannot");
}

void Sentimenter::getSentiment(string review_file)
{
	cout << review_file << endl;
}

int Sentimenter::loadSentiWordNet(void)
{
	ifstream myfile ("SentiWordNet_3.0.0_20130122.txt");
	
	if(myfile.is_open())
	{
		map<string,vector<double> > _temp;
		string line;

    		while( myfile.good() )
    		{
      			getline (myfile,line);

			if(line.size()>0)
			{
				vector<string> data;
				boost::split(data, line, boost::is_any_of("\t"));

				double score = 0;

				score = strtod(data[2].c_str(), NULL) - strtod(data[3].c_str(), NULL);
				
				vector<string> words;
				boost::split(words, data[4], boost::is_any_of(" "));

				for(vector<string>::iterator it = words.begin(); it!= words.end(); ++it)
				{
					vector<string> w_n;
					boost::split(w_n, *it, boost::is_any_of("#"));					

								// change term format // no tagging information use
					w_n[0] += "#" + data[0];

					int index = atoi(w_n[1].c_str()) - 1;

					map<string,vector<double> >::iterator it2 = _temp.find(w_n[0]);

					if(it2 != _temp.end())
					{
						string term = it2->first;
						vector<double> v = it2->second;

						if(index > v.size())
						{
							for(int i = v.size(); i < index; i++)
							{
								v.push_back(0.0);
							}
						}

						v.insert(v.begin()+index, score);
						_temp.insert( pair<string,vector<double> >( term, v ) );

					}
					else
					{
						vector<double> v;
						for(int i = 0; i < index; i++)
						{
							v.push_back(0.0);
						}

						v.insert(v.begin()+index, score);

						_temp.insert( pair<string,vector<double> >( w_n[0], v ) );
					}
				}

			}
    		}

	        map<string, vector<double> >::iterator iter;

        	for (iter = _temp.begin(); iter != _temp.end(); ++iter)
		{
			string word = iter->first;

			vector<double> v = iter->second;

			double score = 0.0;
			double sum = 0.0;

		/*	for (int i = 0; i < v.size(); i++)
			{
				score += ((double) 1 / (double) (i + 1)) * v.at(i);
			}

			for (int i = 1; i <= v.size(); i++)
			{
				sum += (double) 1 / (double) i;
			}

			score /= sum;
		*/
			score = v.at(0);

			dict.insert( pair<string, double>( word, score ) );		
		}

		myfile.close();

		return 1;
	}

	return 0;
}

map<string,double> Sentimenter::do_sentimentAnalysis(string reviewPath, map< string,vector<string> > aspects, Tagger* pos_tagger)
{
	map<string,double> sentiments;

	map<string,double> deviders;

	map<string,int> sentiLexicon;
	sentiLexicon = loadSentiLexicon();

	ifstream myfile(reviewPath.c_str());
	
	if (myfile.is_open())
	{
		string line;

		while ( myfile.good() )
    		{
      		    getline (myfile,line);		
		    vector<string> sentences;
	
		    if(line.size()>0)
		    {
			boost::split(sentences, line, boost::is_any_of(","));
		    }
		    else
		    {
			continue;
		    }

		    for(int i=0; i<sentences.size(); i++)
		    {
			string sentence = sentences[i];

			bool negation = false;

			for(int i=0; i<shifters.size(); i++)
			{
				if( sentence.find(shifters[i]) != std::string::npos )
				{
					negation = true;
					break;
				}	
			}

			for(map< string,vector<string> >::iterator it_m = aspects.begin(); it_m!= aspects.end(); ++it_m)
			{
				string aspect_key = it_m->first;
				vector<string> query = it_m->second;

				for(int i=0; i<query.size(); i++)
				{
					string aspect = query.at(i);

					double senti_sentence_sum = 0.0;
					double senti_sentence_counter = 0.0;

					if( sentence.find(aspect) != std::string::npos )
					{
						pair<map<string,double>::iterator,bool> ret2;
						ret2 = deviders.insert( pair<string,double>( aspect_key, 1 ) );
		  				if (ret2.second==false)
						{
							ret2.first->second = ret2.first->second + 1;
						}

						vector<string> terms = pos_tagger->do_sentence_tagging(sentence);

						for(vector<string>::iterator it_v = terms.begin(); it_v!= terms.end(); ++it_v)
						{						
							string test = *it_v;

							map<string,int>::iterator it_d = sentiLexicon.find(test);

							if(it_d != sentiLexicon.end())
							{
								string term = it_d->first;
								double polarity = double(it_d->second);
							
								senti_sentence_sum = senti_sentence_sum + polarity;
							}
						}

						if(senti_sentence_sum > 0)
						{
							senti_sentence_sum = 1;
						}
						else if(senti_sentence_sum < 0)
						{
							senti_sentence_sum = -1;
						}

						if( (negation) && (senti_sentence_sum!=0) )
						{
							senti_sentence_sum = senti_sentence_sum * (-1);
						}
					}

					pair<map<string,double>::iterator,bool> ret;
					ret = sentiments.insert( pair<string,double>( aspect_key, senti_sentence_sum ) );
		  			if (ret.second==false)
					{
						ret.first->second = ret.first->second + senti_sentence_sum;
					}
				}
			}
		    }		
    		}

	        map<string, double>::iterator iter_d;
        	for (iter_d = sentiments.begin(); iter_d != sentiments.end(); ++iter_d)
		{
			map<string,double>::iterator it_divi = deviders.find(iter_d->first);
			if(it_divi != deviders.end())
			{
				iter_d->second = iter_d->second / it_divi->second;			
			}
		}

    		myfile.close();
  	}

  	else cout << "Unable to open file " << reviewPath << endl; 

	return sentiments;
}

map<string,double> Sentimenter::do_sentimentAnalysis2(string reviewPath, map< string,vector<string> > aspects, Tagger* pos_tagger, map<string,double>* sos)
{
	map<string,double> sentiments;
	map<string,double> deviders;

	map<string,int> sentiLexicon;
	sentiLexicon = loadSentiLexicon();

	ifstream myfile(reviewPath.c_str());
	
	if (myfile.is_open())
	{
		string line;

		while ( myfile.good() )
    		{
      		    getline (myfile,line);		
		    vector<string> sentences;
	
		    if(line.size()>0)
		    {
			boost::split(sentences, line, boost::is_any_of(","));
		    }
		    else
		    {
			continue;
		    }

		    for(int i=0; i<sentences.size(); i++)
		    {
			string sentence = sentences[i];

			for(map< string,vector<string> >::iterator it_m = aspects.begin(); it_m!= aspects.end(); ++it_m)
			{
				string aspect_key = it_m->first;
				vector<string> query = it_m->second;

				for(int i=0; i<query.size(); i++)
				{
					string aspect = query.at(i);
					double so_sum = 0.0;

					if( sentence.find(aspect) != std::string::npos )
					{
						pair<map<string,double>::iterator,bool> ret2;
						ret2 = deviders.insert( pair<string,double>( aspect_key, 1 ) );
		  				if (ret2.second==false)
						{
							ret2.first->second = ret2.first->second + 1;
						}

						vector< boost::tuple<string,string> > tags;
						tags = pos_tagger->do_sentence_tagging_map(sentence);

						vector<string> phrases = check_pos_patterns(&tags);
			
						for(int i=0; i<phrases.size(); i++)
						{
							map<string,double>::iterator iter_sos = sos->find(phrases.at(i));
							if( iter_sos != sos->end() )
							{
								so_sum = so_sum + iter_sos->second;
							}
						}
					}

					pair<map<string,double>::iterator,bool> ret;
					ret = sentiments.insert( pair<string,double>( aspect_key, so_sum ) );
		  			if (ret.second==false)
					{
						ret.first->second = ret.first->second + so_sum;
					}
				}
			}
		    }		
    		}

	        map<string, double>::iterator iter_d;
        	for (iter_d = sentiments.begin(); iter_d != sentiments.end(); ++iter_d)
		{
			map<string,double>::iterator it_divi = deviders.find(iter_d->first);
			if(it_divi != deviders.end())
			{
				iter_d->second = iter_d->second / it_divi->second;			
			}
		}

    		myfile.close();
  	}

  	else cout << "Unable to open file"; 

	return sentiments;
}

// extracts opinion phrases using pos tag patterns
vector<string> Sentimenter::getPhrases(string index_file, vector<string> aspects, Tagger* pos_tagger)
{
	vector<string> phrases;

	map<string,int> sentiLexicon;
	sentiLexicon = loadSentiLexicon();

	string index_path = "reviews/" + index_file;
	ifstream myfile(index_path.c_str());
	
	if (myfile.is_open())
	{
		string line;

		while ( myfile.good() )
    		{
      		    getline (myfile,line);		
		    vector<string> sentences;
	
		    if(line.size()>0)
		    {
			boost::split(sentences, line, boost::is_any_of(","));
		    }
		    else
		    {
			continue;
		    }

		    for(int i=0; i<sentences.size(); i++)
		    {
		    	string sentence = sentences[i];

			for(int i=0; i<aspects.size(); i++)
			{
				string aspect = aspects.at(i);

				if( sentence.find(aspect) != std::string::npos )
				{
					vector< boost::tuple<string,string> > tags;
					tags = pos_tagger->do_sentence_tagging_map(sentence);

					vector<string> phrases_vector = check_pos_patterns(&tags);

					if( phrases_vector.size() > 0 )
					{
						for(int i=0; i<phrases_vector.size(); i++)
						{
							phrases.push_back( phrases_vector.at(i) );
						}
					}						
				}
			}
		    }		
    		}

    		myfile.close();
  	}

  	else cout << "Unable to open file"; 

	return phrases;
}

// checks if a tagged phrase match any of the opinion patterns
vector<string> Sentimenter::check_pos_patterns(vector< boost::tuple<string, string> >* tags)
{
	vector<string> phrases;
	
	map<string,PosTagValue> mapPosTagValues;

	mapPosTagValues["JJ"] = JJ;
	mapPosTagValues["RB"] = RB;
	mapPosTagValues["RBR"] = RBR;
	mapPosTagValues["RBS"] = RBS;
	mapPosTagValues["NN"] = NN;
	mapPosTagValues["NNS"] = NNS;

	if(tags->size() < 3 )
	{
		return phrases;
	}

	for(vector< boost::tuple<string, string> >::const_iterator iter = tags->begin(); iter != tags->end(); ++iter)
	{
		vector< boost::tuple<string, string> >::const_iterator iter_temp = iter+3;

		if( iter_temp > tags->end() )
		{
			break;
		}
		else
		{
			vector< boost::tuple<string, string> >::const_iterator iter1 = iter;
			vector< boost::tuple<string, string> >::const_iterator iter2 = iter+1;			
			vector< boost::tuple<string, string> >::const_iterator iter3 = iter+2;

			string term1 = iter1->get<0>();
			string term2 = iter2->get<0>();

			string tag1 = iter1->get<1>();
			string tag2 = iter2->get<1>();
			string tag3 = iter3->get<1>();

			bool result = false;

			if(tag1=="JJ")
			{
				if( (tag2 =="NN") || (tag2=="NNS") )
				{
					result=true;
				}
				else if( (tag2=="JJ") && !( (tag3=="NN") || (tag3=="NNS") ) )
				{
					result=true;
				}
			}	
			else if( tag1=="RB")
			{
				if( (tag2 == "JJ") && !( (tag3=="NN") || (tag3=="NNS") ) )
				{
					result=true;
				}
				else if( (tag2=="VB") || (tag2=="VBD") || (tag2=="VBN") || (tag2=="VBG") )
				{
					result=true;
				}
			}
			else if( tag1=="RBR")
			{
				if( (tag2 == "JJ") && !( (tag3=="NN") || (tag3=="NNS") ) )
				{
					result=true;
				}
				else if( (tag2=="VB") || (tag2=="VBD") || (tag2=="VBN") || (tag2=="VBG") )
				{
					result=true;
				}
			}
			else if( tag1=="RBS" )
			{
				if( (tag2 == "JJ") && !( (tag3=="NN") || (tag3=="NNS") ) )
				{
					result=true;
				}
				else if( (tag2=="VB") || (tag2=="VBD") || (tag2=="VBN") || (tag2=="VBG") )
				{
					result=true;
				}
			}
			else if( tag1=="NN")
			{
				if( (tag2=="JJ") && !( (tag3=="NN") || (tag3=="NNS") ) )
				{
					result=true;
				}
			}
			else if( tag1=="NNS")
			{
				if( (tag2=="JJ") && !( (tag3=="NN") || (tag3=="NNS" ) ) )
				{
					result=true;
				}
			}

			if( result )
			{
				string phrase = term1 + " " + term2;
				phrases.push_back(phrase);
			}
		}
	}

	return phrases;
}

// load senti lexicon (positive and negative words) to map
map<string,int> Sentimenter::loadSentiLexicon(void)
{
	map<string,int> sentiLexicon;

	string line;
	
	ifstream myfile ("positive-words.txt");

	if (myfile.is_open())
	{
    		while ( myfile.good() )
    		{
     			getline (myfile,line);

			pair<map<string,int>::iterator,bool> ret;

			ret = sentiLexicon.insert( pair<string,int>( line, 1 ) );
		  	if (ret.second==false)
			{
				//
			}
    		}

    		myfile.close();
	}
	else
	{
		cout << "Unable to open file"; 
	}

	ifstream myfile2("negative-words.txt");

	if (myfile2.is_open())
	{
    		while ( myfile2.good() )
    		{
     			getline (myfile2,line);

			pair<map<string,int>::iterator,bool> ret;

			ret = sentiLexicon.insert( pair<string,int>( line, -1 ) );
		  	if (ret.second==false)
			{
				ret.second = 0;
			}
    		}

    		myfile2.close();
	}
	else
	{
		cout << "Unable to open file"; 
	}

	return sentiLexicon;
}

// get score based only to sentiments on aspects
double Sentimenter::getScore(string filename, vector<string> aspects)
{
	double entity_score=0.0;

	double aspect_counter=0.0;

	map<string,double> my_sentiments = FileMapIO::fileToSentiMap(filename);

	for (map<string,double>::iterator it=my_sentiments.begin(); it!=my_sentiments.end(); ++it)
	{
		string aspect = it->first;

		for(int i=0; i<aspects.size(); i++)
		{
			if( aspect == aspects.at(i) )
			{	
				entity_score = entity_score + it->second;
				aspect_counter++;
				break;
			}
		}
	}

	entity_score = entity_score / aspect_counter;

	return entity_score;
}




