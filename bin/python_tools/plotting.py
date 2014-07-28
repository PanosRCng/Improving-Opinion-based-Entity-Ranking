try:
	import math
	from matplotlib.pyplot import *
except ImportError:
	print 'can not import modules'
	sys.exit(1) # exit on import module failed

def read_results(filename):

	y = []

	f = open(filename, 'rU')
	lines = f.readlines()
	f.close()

	temp_y = []
	counter_y = 0;

	for line in lines:
		result = line.split('\t')

		nDCG = float(result[1])

		if not math.isnan(nDCG):
			temp_y.append(nDCG)
		else:
			temp_y.append(0)

		counter_y = counter_y + 1

		if (counter_y%10==0) and (counter_y != 0):
			sum_y = 0;
			for i in temp_y:
				sum_y = sum_y + i

			avg_y = sum_y/10

			y.append(avg_y)
			counter_y = 0
			temp_y = []

	return y
		
def plot_results(y):

	x=[]

	for i in range(len(y)):
		x.append(i)

	plot(x, y, 'b-o')
	ylabel(' nDCG@10 ')
	xlabel('# query (x10)')
	xlim(0,len(x))
	ylim(0.8,0.95)
	show()

###################################################################

y=read_results("results.txt")
plot_results(y)

