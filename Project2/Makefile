all: compile run
compile:
	gcc p2.c -lpthread -lrt -o project2.out	
run: compile
	./project2.out; 
shared_memory:
	ipcs
	@echo "ipcrm -m"