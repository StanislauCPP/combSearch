PROJECT = combSearch

build :
	gcc -flto ${PROJECT}.c Subset.c -o ${PROJECT}.out
