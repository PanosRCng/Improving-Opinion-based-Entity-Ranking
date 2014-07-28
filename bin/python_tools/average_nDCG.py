
def avg_ndcg(filename):

	y = []

	f = open(filename, 'rU')
	lines = f.readlines()
	f.close()

	sum_y = 0
	counter_y = 0;

	for line in lines:
		result = line.split('\t')

		nDCG = float(result[1])

		counter_y = counter_y + 1
		sum_y = sum_y + nDCG

	avg_y = sum_y/counter_y

	return sum_y, counter_y, avg_y
		
###################################################################

sum_ndcg, counter, avg = avg_ndcg("results.txt")

print 'average nDCG: ' + str(avg)
