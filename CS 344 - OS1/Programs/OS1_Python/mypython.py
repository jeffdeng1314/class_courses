import random
import sys

def random_gen(file_name):
	for i in range(10):
		x = random.randint(0,25)+97
		file_name.write(str(chr(x)))
		sys.stdout.write(str(chr(x)))
	file_name.write("\n")


def main():
	lmao = open("lmao", "w+")
	random_gen(lmao)
	sys.stdout.write("\n")
	lmao.close()

	lol = open("lol", "w+")
	random_gen(lol)
	sys.stdout.write("\n")
	lol.close()

	haha = open("haha", "w+")
	random_gen(haha)
	sys.stdout.write("\n")
	haha.close()

	num1 = random.randint(1,42)
	num2 = random.randint(1,42)
	product = num1 * num2
	print(str(num1))
	print(str(num2))
	print(str(product))

main()
