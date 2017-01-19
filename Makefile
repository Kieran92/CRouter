all: pktgen router

pktgen.o:
	gcc -Wall -c -o pktgen.o pktgen.c
router.o:
	gcc -Wall -c -o router.o router.c
pktgen:
	gcc -Wall  -o pktgen pktgen.c
router:
	gcc -Wall  -o router router.c
clean:
	-rm -rf *.o pktgen router all