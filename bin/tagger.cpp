#include "tagger.h"
#include "boost/tuple/tuple.hpp"

using namespace std;

bool PERFORM_TOKENIZATION = false;
bool OUTPUT_TAG_PROBS = false;
bool STANDOFF = false;
bool UIMA     = false;
bool ENJU     = false;
int  NBEST = 0;

string MODEL_DIR = "."; // the default directory for saving the models

CRF_Model crfm; // model

const double PROB_OUTPUT_THRESHOLD = 0.001; // suppress output of tags with a very low probability

void
tokenize(const string & s, vector<Token> & vt, const bool use_upenn_tokenizer);

void
crf_decode_lookahead(Sentence & s, CRF_Model & m, vector< map<string, double> > & tagp);

void read_WordNet(const string & wordnetdir);


ParenConverter paren_converter;


struct TagProb
{
  string tag;
  double prob;
  TagProb(const string & t_, const double & p_) : tag(t_), prob(p_) {}
  bool operator<(const TagProb & x) const { return prob > x.prob; }
};

Tagger::Tagger()
{
	MODEL_DIR = "model_wsj02-21";

	if (!ENJU)
	{
    	//	cerr << "loading tagger model \"" << MODEL_DIR << "\" to memory .....";
  	}

	if (!crfm.load_from_file(MODEL_DIR + "/model.la", ENJU ? false : true)) exit(1);
	if (!ENJU)
	{
    	//	cerr << "[ok]" << endl;
  	}	
}

Tagger::~Tagger()
{
	//
}

vector<string> Tagger::do_tagging(string ifilename)
{  
	vector<string> nn_vector;

	PERFORM_TOKENIZATION = true; 

	istream *is(&std::cin);	
	ifstream ifile;
	if (ifilename != "")
	{
		ifile.open(ifilename.c_str());
    
		if (!ifile) { cerr << "error: cannot open " << ifilename << endl; exit(1); }
		is = &ifile;
	}

	string line;
	int nlines = 0;
	while (getline(*is, line))
	{
		nlines++;
		vector<Token> vt;
		tokenize(line, vt, PERFORM_TOKENIZATION);

		if (vt.size() > 990)
		{
			cerr << "warning: the sentence is too long. it has been truncated." << endl;
      			while (vt.size() > 990) vt.pop_back();
    		}

		// convert parantheses
		vector<string> org_strs;
		for (vector<Token>::iterator i = vt.begin(); i != vt.end(); i++)
		{
	        	org_strs.push_back(i->str);
      			i->str = paren_converter.Ptb2Pos(i->str);
      			i->prd = "?";
    		}

    		// tag the words
		vector< map<string, double> > tagp0, tagp1;

		crf_decode_lookahead(vt, crfm, tagp0);
		if (false)
		{

		        assert(0); exit(1);
    		}
		else
		{

      			for (vector<Token>::const_iterator i = vt.begin(); i != vt.end(); i++)
			{
				map<string, double> dummy;
				tagp1.push_back(dummy);
	        	}
    		}

    	// merge the outputs (simple interpolation of probabilities)
    	vector< map<string, double> > tagp; // merged

    	for (size_t i = 0; i < vt.size(); i++)
	{
        	const map<string, double> & crf = tagp0[i];
        	const map<string, double> & ef  = tagp1[i];
        	map<string, double> m, m2; // merged

      		double sum = 0;
      		for (map<string, double>::const_iterator j = crf.begin(); j != crf.end(); j++)
		{
			m.insert(pair<string, double>(j->first, j->second));
			sum += j->second;
	        }

      		for (map<string, double>::const_iterator j = ef.begin(); j != ef.end(); j++)
		{
			sum += j->second;
			if (m.find(j->first) == m.end())
			{
	        		m.insert(pair<string, double>(j->first, j->second));
			}
			else
			{
	  			m[j->first] += j->second;
			}
      		}

      		const double th = PROB_OUTPUT_THRESHOLD * sum;
      		for (map<string, double>::iterator j = m.begin(); j != m.end(); j++)
		{
			if (j->second >= th) m2.insert(*j);
      		}

      		double maxp = -1;
	        string maxtag;
      		for (map<string, double>::iterator j = m2.begin(); j != m2.end(); j++)
		{
			const double p = j->second;

			if (p > maxp) { maxp = p; maxtag = j->first; }
	        }
      		tagp.push_back(m2);
      		vt[i].prd = maxtag;
    	}

    	for (size_t i = 0; i < vt.size(); i++)
	{
		const string s = org_strs[i];
      		const string p = vt[i].prd;

		string term = "";

		if( (p == "NN") || (p=="JJ") || (p=="JJR") || (p=="JJS") || (p=="NNS") || (p=="NNP") || (p=="NNPS") )
		{
			int dot_pos = s.find(".");

			if(  dot_pos!= string::npos )
			{
				term = s.substr(0,dot_pos);
			}
			else
			{
				term = s;
			}

			nn_vector.push_back(term);
		}

    	}

  }

  return nn_vector;
}

vector<string> Tagger::do_sentence_tagging(string line)
{
	vector<string> nn_vector;

	PERFORM_TOKENIZATION = true; 

	vector<Token> vt;
	tokenize(line, vt, PERFORM_TOKENIZATION);

	if (vt.size() > 990)
	{
		cerr << "warning: the sentence is too long. it has been truncated." << endl;
      		while (vt.size() > 990) vt.pop_back();
    	}

	// convert parantheses
	vector<string> org_strs;
	for (vector<Token>::iterator i = vt.begin(); i != vt.end(); i++)
	{
	       	org_strs.push_back(i->str);
      		i->str = paren_converter.Ptb2Pos(i->str);
      		i->prd = "?";
    	}

    	// tag the words
	vector< map<string, double> > tagp0, tagp1;

	crf_decode_lookahead(vt, crfm, tagp0);
	if (false)
	{
	        assert(0); exit(1);
    	}
	else
	{

      		for (vector<Token>::const_iterator i = vt.begin(); i != vt.end(); i++)
		{
			map<string, double> dummy;
			tagp1.push_back(dummy);
	       	}
    	}

    	// merge the outputs (simple interpolation of probabilities)
    	vector< map<string, double> > tagp; // merged

    	for (size_t i = 0; i < vt.size(); i++)
	{
        	const map<string, double> & crf = tagp0[i];
        	const map<string, double> & ef  = tagp1[i];
        	map<string, double> m, m2; // merged

      		double sum = 0;
      		for (map<string, double>::const_iterator j = crf.begin(); j != crf.end(); j++)
		{
			m.insert(pair<string, double>(j->first, j->second));
			sum += j->second;
	        }

      		for (map<string, double>::const_iterator j = ef.begin(); j != ef.end(); j++)
		{
			sum += j->second;
			if (m.find(j->first) == m.end())
			{
	        		m.insert(pair<string, double>(j->first, j->second));
			}
			else
			{
	  			m[j->first] += j->second;
			}
      		}

      		const double th = PROB_OUTPUT_THRESHOLD * sum;
      		for (map<string, double>::iterator j = m.begin(); j != m.end(); j++)
		{
			if (j->second >= th) m2.insert(*j);
      		}

      		double maxp = -1;
	        string maxtag;
      		for (map<string, double>::iterator j = m2.begin(); j != m2.end(); j++)
		{
			const double p = j->second;

			if (p > maxp) { maxp = p; maxtag = j->first; }
	        }
      		tagp.push_back(m2);
      		vt[i].prd = maxtag;
    	}

    	for (size_t i = 0; i < vt.size(); i++)
	{
		const string s = org_strs[i];
      		const string p = vt[i].prd;

		string term = "";

		if( (p=="RB") || (p=="RBR") || (p=="RBS") || (p=="RBG") || (p=="VBG") || (p=="JJ") || (p=="JJR") || (p=="JJS") )
		{
			int dot_pos = s.find(".");

			if(  dot_pos!= string::npos )
			{
				term = s.substr(0,dot_pos);
			}
			else
			{
				term = s;
			}

			nn_vector.push_back(term);
		}

    	}

	return nn_vector;

}

// use the pos tagger to locate only the possible aspect terms (NN, JJ, JJR, JJS, NNS, NNP, NNPS) 
vector<string> Tagger::do_sentence_tagging_for_aspects(string line)
{
	vector<string> nn_vector;

	PERFORM_TOKENIZATION = true; 

	vector<Token> vt;
	tokenize(line, vt, PERFORM_TOKENIZATION);

	if (vt.size() > 990)
	{
		cerr << "warning: the sentence is too long. it has been truncated." << endl;
      		while (vt.size() > 990) vt.pop_back();
    	}

	// convert parantheses
	vector<string> org_strs;
	for (vector<Token>::iterator i = vt.begin(); i != vt.end(); i++)
	{
	       	org_strs.push_back(i->str);
      		i->str = paren_converter.Ptb2Pos(i->str);
      		i->prd = "?";
    	}

    	// tag the words
	vector< map<string, double> > tagp0, tagp1;

	crf_decode_lookahead(vt, crfm, tagp0);
	if (false)
	{
	        assert(0); exit(1);
    	}
	else
	{

      		for (vector<Token>::const_iterator i = vt.begin(); i != vt.end(); i++)
		{
			map<string, double> dummy;
			tagp1.push_back(dummy);
	       	}
    	}

    	// merge the outputs (simple interpolation of probabilities)
    	vector< map<string, double> > tagp; // merged

    	for (size_t i = 0; i < vt.size(); i++)
	{
        	const map<string, double> & crf = tagp0[i];
        	const map<string, double> & ef  = tagp1[i];
        	map<string, double> m, m2; // merged

      		double sum = 0;
      		for (map<string, double>::const_iterator j = crf.begin(); j != crf.end(); j++)
		{
			m.insert(pair<string, double>(j->first, j->second));
			sum += j->second;
	        }

      		for (map<string, double>::const_iterator j = ef.begin(); j != ef.end(); j++)
		{
			sum += j->second;
			if (m.find(j->first) == m.end())
			{
	        		m.insert(pair<string, double>(j->first, j->second));
			}
			else
			{
	  			m[j->first] += j->second;
			}
      		}

      		const double th = PROB_OUTPUT_THRESHOLD * sum;
      		for (map<string, double>::iterator j = m.begin(); j != m.end(); j++)
		{
			if (j->second >= th) m2.insert(*j);
      		}

      		double maxp = -1;
	        string maxtag;
      		for (map<string, double>::iterator j = m2.begin(); j != m2.end(); j++)
		{
			const double p = j->second;

			if (p > maxp) { maxp = p; maxtag = j->first; }
	        }
      		tagp.push_back(m2);
      		vt[i].prd = maxtag;
    	}

    	for (size_t i = 0; i < vt.size(); i++)
	{
		const string s = org_strs[i];
      		const string p = vt[i].prd;

		string term = "";

		if( (p == "NN") || (p=="JJ") || (p=="JJR") || (p=="JJS") || (p=="NNS") || (p=="NNP") || (p=="NNPS") )
		{
			int dot_pos = s.find(".");

			if(  dot_pos!= string::npos )
			{
				term = s.substr(0,dot_pos);
			}
			else
			{
				term = s;
			}

			nn_vector.push_back(term);
		}

    	}

	return nn_vector;

}


vector< boost::tuple<string, string> > Tagger::do_sentence_tagging_map(string line)
{
	vector< boost::tuple<string, string> > tags;

	PERFORM_TOKENIZATION = true; 

	vector<Token> vt;
	tokenize(line, vt, PERFORM_TOKENIZATION);

	if (vt.size() > 990)
	{
		cerr << "warning: the sentence is too long. it has been truncated." << endl;
      		while (vt.size() > 990) vt.pop_back();
    	}

	// convert parantheses
	vector<string> org_strs;
	for (vector<Token>::iterator i = vt.begin(); i != vt.end(); i++)
	{
	       	org_strs.push_back(i->str);
      		i->str = paren_converter.Ptb2Pos(i->str);
      		i->prd = "?";
    	}

    	// tag the words
	vector< map<string, double> > tagp0, tagp1;

	crf_decode_lookahead(vt, crfm, tagp0);
	if (false)
	{
	        assert(0); exit(1);
    	}
	else
	{

      		for (vector<Token>::const_iterator i = vt.begin(); i != vt.end(); i++)
		{
			map<string, double> dummy;
			tagp1.push_back(dummy);
	       	}
    	}

    	// merge the outputs (simple interpolation of probabilities)
    	vector< map<string, double> > tagp; // merged

    	for (size_t i = 0; i < vt.size(); i++)
	{
        	const map<string, double> & crf = tagp0[i];
        	const map<string, double> & ef  = tagp1[i];
        	map<string, double> m, m2; // merged

      		double sum = 0;
      		for (map<string, double>::const_iterator j = crf.begin(); j != crf.end(); j++)
		{
			m.insert(pair<string, double>(j->first, j->second));
			sum += j->second;
	        }

      		for (map<string, double>::const_iterator j = ef.begin(); j != ef.end(); j++)
		{
			sum += j->second;
			if (m.find(j->first) == m.end())
			{
	        		m.insert(pair<string, double>(j->first, j->second));
			}
			else
			{
	  			m[j->first] += j->second;
			}
      		}

      		const double th = PROB_OUTPUT_THRESHOLD * sum;
      		for (map<string, double>::iterator j = m.begin(); j != m.end(); j++)
		{
			if (j->second >= th) m2.insert(*j);
      		}

      		double maxp = -1;
	        string maxtag;
      		for (map<string, double>::iterator j = m2.begin(); j != m2.end(); j++)
		{
			const double p = j->second;

			if (p > maxp) { maxp = p; maxtag = j->first; }
	        }
      		tagp.push_back(m2);
      		vt[i].prd = maxtag;
    	}

    	for (size_t i = 0; i < vt.size(); i++)
	{
		const string s = org_strs[i];
      		const string p = vt[i].prd;

		string term = "";

		int dot_pos = s.find(".");

		if(  dot_pos!= string::npos )
		{
			term = s.substr(0,dot_pos);
		}
		else
		{
			term = s;
		}

		tags.push_back( boost::tuple<string, string>( term, p) );
    	}

	return tags;
}


