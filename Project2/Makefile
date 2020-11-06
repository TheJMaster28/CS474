all: compile run
compile:
	gcc p2.c -lpthread -lrt -o project2.out	
run: compile
	for number in 1 2 3 4 5; do \
		echo "~~~~~~$$number~~~~~~"; \
		./project2; \
	done 
run_once: compile
	./project2
