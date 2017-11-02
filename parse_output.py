import sys

def main(argv) :

	f = open(argv[0], "r")

	output = ""

	l = f.readline()

	if len(l) == 0:
		print "Erro :("
	else:
		print l

	try:
		while "};" not in l:

			if "int32_t width =" in l:
				while "};" not in l:
					output += l
					l = f.next()
				output += l
				myfile = open("img_matrix_bmp.h", 'w')
				myfile.write(output)
				myfile.close()

			l = f.next()
	except StopIteration:
		print "Dados lidos e escritos com sucesso!"

if __name__ == "__main__" : main(sys.argv[1:])
