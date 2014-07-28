#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <list>

class Utils{

	public:
		Utils();
		~Utils();

		int deleteFiles(std::string);
		/*
			deletes all files of a directory

			1 - the path of the directory
		*/


		int getFilesList(std::string, std::vector<std::string> &, bool);
		/*
			get all files of a directory as a vector of strings

			1 - the path of the directory
			2 - a pointer to a vector of strings, where the list of files will be saved
			3 - false if you want just the filenames, true if you want the full paths
		*/

		std::vector<std::string> getCL(std::string);
		std::vector<std::string> getCL2(std::vector< boost::tuple<std::string, std::string> > *);


		std::string getQueryStr(std::vector<std::string>);
		/*
			makes a query string from a vector of string terms (term1_term2_term3_term4_term5)

			1 - the string that contains the terms
		*/

		std::vector<std::string> getQueryVec(std::string);
		/*
			breaks a query (term1 term2, term3, term4 term5) to terms and returns them as a vector of strings
		
			1 - the query as a string
		*/



		std::vector<std::string> getQueryAspects(std::string);
		std::vector<std::string> getQueryTerms(std::string);


		std::list<std::string> getSentences(std::string);
		/*
			get sentences from paragraph, splits sentences using the symbols "." and "!"

			1 - the paragraph as string
		*/


		int parse_reviews(std::string, std::string);
		/*
			reads reviews from a source directory, process them, and saves them to a destination directory

			1 - the reviews source directory path
			2 - the reviews destination directory path	
		*/

		int parse_review(std::string, std::string);
		/*
			parse a review

			1 - the review source path
			2 - the review destination path
		*/

		double getSimilarity(std::string, std::string);
		/*
			calculates the similarity between two documents

			1 - the document's a feature path
			2 - the document's b feature path
		*/

		double cosineSimilarity( std::vector<double>, std::vector<double> );
		/*
			calculates the cosine similarity of documents a and b

			1 - vector a as a vector of doubles
			2 - vector b as a vector of doubles
		*/

		double dot_product(std::vector<double>, std::vector<double>);
		/*
			calculates the dot product of vectors a and b

			1 - vector a as a vector of doubles
			2 - vector b as a vector of doubles
		*/

		double eucl_norm(std::vector<double>);
		/*
			calculates the euclidean norm of vector a

			1 - the vector as a vector of doubles
		*/

		std::vector<std::string> getLines(std::string);
		/*
			returns the lines of a text file as a vector of strings

			1 - the path of the text file
		*/

		double Log2( double n );
		/*
			base 2 logarithm of n

			(*)   log(n)/log(2) is log2 

			1 - the n number
		*/
		
};


#endif
