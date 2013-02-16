CFLAGS = -O3 -Wall -fstrict-aliasing -std=c99

test: main
	@echo
	@echo "Example 1: search for pattern in target (partial subgraph isomorphism)"
	./main -v -p pattern -t target
	@echo
	@echo "Example 2: search for pattern in target (induced subgraph isomorphism)"
	./main -v -p pattern -t target -i
	@echo
	@echo "Example 3: search for pattern in target with additional constraints on node matching (partial subgraph isomorphism)"
	./main -v -p pattern -t target -d nodeComp

main: main.c allDiff.c graph.c lad.c domains.c
	$(CC) $(CFLAGS) main.c -o $@
