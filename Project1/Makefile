all: compile run
compile:
	gcc project1.c -o project1
	
run: compile
	for number in 1 2 3 4 5 6 7 8 9 10; do \
		echo "~~~~~~$$number~~~~~~"; \
		./project1; \
	done
	
run_once: compile
	./project1