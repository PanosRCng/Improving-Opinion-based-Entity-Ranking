BM25_SOURCE=bm25.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

OPINEXPBM25_SOURCE=opinExpBM25.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

BM25KURLAND_SOURCE=bm25kurland.cpp knearest.cpp kmeans.cpp kurland.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

BM25KURLANDSENTI_SOURCE=bm25kurlandsenti.cpp sentimenter.cpp knearest.cpp kmeans.cpp kurland.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

SENTI_SOURCE=senti.cpp scorer.cpp sentimenter.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

SENTIBM25_SOURCE=sentiBM25.cpp sentimenter.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

SCHEMA1_SOURCE=schema1.cpp scorer.cpp invertedIndexer.cpp judgment.cpp fileMapIO.cpp utils.cpp tagger.cpp sentimenter.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp

SCHEMA2_SOURCE=schema2.cpp scorer.cpp kmeans.cpp kurland.cpp invertedIndexer.cpp judgment.cpp tagger.cpp sentimenter.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

OFFLINEII_SOURCE=offlineInvertedIndex.cpp judgment.cpp invertedIndexer.cpp fileMapIO.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

OFFLINEREVIEWII_SOURCE=offlineReviewInvertedIndex.cpp judgment.cpp invertedIndexer.cpp fileMapIO.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

OFFLINESENTI_SOURCE=offlineSentiments.cpp sentimenter.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

OFFLINEPHRASESENTI_SOURCE=offlinePhraseSentiments.cpp sentimenter.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

OFFLINEPHRASES_SOURCE=offlinePhrases.cpp invertedIndexer.cpp sentimenter.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc fileMapIO.cpp

WEBA_SOURCE=webA.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc fileMapIO.cpp

WEBKURLAND_SOURCE=webKurland.cpp sentimenter.cpp knearest.cpp kmeans.cpp kurland.cpp scorer.cpp invertedIndexer.cpp judgment.cpp tagger.cpp lookahead.cpp tokenize.cpp crf.cpp crfpos.cpp fileMapIO.cpp utils.cpp wordNet.cpp wordnet.cc load_wordnet.cc info_helper.cc

BM25=bm25
OPINEXPBM25=opinExpBM25
BM25KURLAND=bm25kurland
BM25KURLANDSENTI=bm25kurlandsenti
SENTI=senti
SENTIBM25=sentiBM25
SCHEMA1=schema1
SCHEMA2=schema2
OFFLINEII=offlineII
OFFLINEREVIEWII=offlineReviewII
OFFLINESENTI=offlineSenti
OFFLINEPHRASESENTI=offlinePhraseSenti
OFFLINEPHRASES=offlinePhrases
WEBA=webA
WEBKURLAND=webKurland

#---------------- !!! change this !!! set the boost location here ------------------
INCLUDES=/home/panos/Desktop/LINUX_BACKUP/Opinion_MIning_and_Sentiment_Analysis/project/bin/boost_1_53_0/

CC=g++

#------------------------------------------------------------------------------


$(BM25): $(BM25_SOURCE)

	$(CC) -I $(INCLUDES) $(BM25_SOURCE) -o $(BM25) -w


$(OPINEXPBM25): $(OPINEXPBM25_SOURCE)

	$(CC) -I $(INCLUDES) $(OPINEXPBM25_SOURCE) -o $(OPINEXPBM25) -w


$(BM25KURLAND): $(BM25KURLAND_SOURCE)

	$(CC) -I $(INCLUDES) $(BM25KURLAND_SOURCE) -o $(BM25KURLAND) -w


$(BM25KURLANDSENTI): $(BM25KURLANDSENTI_SOURCE)

	$(CC) -I $(INCLUDES) $(BM25KURLANDSENTI_SOURCE) -o $(BM25KURLANDSENTI) -w


$(SENTI): $(SENTI_SOURCE)

	$(CC) -I $(INCLUDES) $(SENTI_SOURCE) -o $(SENTI) -w


$(SENTIBM25): $(SENTIBM25_SOURCE)

	$(CC) -I $(INCLUDES) $(SENTIBM25_SOURCE) -o $(SENTIBM25) -w


$(SCHEMA1): $(SCHEMA1_SOURCE)

	$(CC) -I $(INCLUDES) $(SCHEMA1_SOURCE) -o $(SCHEMA1) -w

$(SCHEMA2): $(SCHEMA2_SOURCE)

	$(CC) -I $(INCLUDES) $(SCHEMA2_SOURCE) -o $(SCHEMA2) -w


$(OFFLINEII): $(OFFLINEII_SOURCE)

	$(CC) -I $(INCLUDES) $(OFFLINEII_SOURCE) -o $(OFFLINEII) -w


$(OFFLINEREVIEWII): $(OFFLINEREVIEWII_SOURCE)

	$(CC) -I $(INCLUDES) $(OFFLINEREVIEWII_SOURCE) -o $(OFFLINEREVIEWII) -w


$(OFFLINESENTI): $(OFFLINESENTI_SOURCE)

	$(CC) -I $(INCLUDES) $(OFFLINESENTI_SOURCE) -o $(OFFLINESENTI) -w

$(OFFLINEPHRASESENTI): $(OFFLINEPHRASESENTI_SOURCE)

	$(CC) -I $(INCLUDES) $(OFFLINEPHRASESENTI_SOURCE) -o $(OFFLINEPHRASESENTI) -w

$(OFFLINEPHRASES): $(OFFLINEPHRASES_SOURCE)

	$(CC) -I $(INCLUDES) $(OFFLINEPHRASES_SOURCE) -o $(OFFLINEPHRASES) -w


$(WEBA): $(WEBA_SOURCE)

	$(CC) -I $(INCLUDES) $(WEBA_SOURCE) -o $(WEBA) -w


$(WEBKURLAND): $(WEBKURLAND_SOURCE)

	$(CC) -I $(INCLUDES) $(WEBKURLAND_SOURCE) -o $(WEBKURLAND) -w

clean:

	rm -f $(BM25) *.o *~
	rm -f $(OPINEXPBM25) *.o *~
	rm -f $(BM25KURLAND) *.o *~
	rm -f $(BM25KURLANDSENTI) *.o *~
	rm -f $(SENTI) *.o *~
	rm -f $(SENTIBM25) *.o *~
	rm -f $(SCHEMA1) *.o *~
	rm -f $(SCHEMA2) *.o *~
	rm -f $(BM25KURLANDSENTI) *.o *~	
	rm -f $(WEBA) *.o *~
	rm -f $(OFFLINEREVIEWII) *.o *~	
	rm -f $(OFFLINEII) *.o *~
	rm -f $(OFFLINESENTI) *.o *~
	rm -f $(OFFLINEPHRASESENTI) *.o *~
	rm -f $(OFFLINEPHRASES) *.o *~
	rm -f $(WEBKURLAND) *.o *~
	rm -f $(WEBA) *.o *~

