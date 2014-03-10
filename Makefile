all:
	g++ -Wall -I. -o bin/proof_verifier Justifications/*.cpp Statements/*.cpp Proof/*.cpp

debug:
	g++ -g -Wall -I. -o bin/proof_debug Justifications/*.cpp Statements/*.cpp Proof/*.cpp