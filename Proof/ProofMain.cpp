#include "Proof.hpp"
#include "ProofReader.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

//Runs the program
int main(int nargs, char** args)
{
  if(nargs != 2)
  {
    cerr << "Usage: " << args[0] << " <input filename>\n";
    return 0;
  }
  
  Proof p;
  ProofReader r;
  r.setTarget(&p);
  if(!r.readFile(args[1]))
  {
    cerr << "Program terminated: errors encountered while reading file(s)\n";
    return 0;
  }
  
  p.printProof();
  p.verifyProof();
  return 0;
}

