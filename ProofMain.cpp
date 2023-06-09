#include "Proof.hpp"
#include "ProofReader.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

/// <summary>
/// Runs the program. Expects one argument after the executable name, which the
/// name of an input file to read into a Proof object. That Proof will then be
/// verified.
/// </summary>
int main(int nargs, char** args)
{
  if(nargs != 2)
  {
    //Input file not specified, or additional arguments are present
    cerr << "Usage: " << args[0] << " <input filename>\n";
    return 0;
  }
  
  Proof p;
  ProofReader r;
  r.setTarget(&p);
  if(!r.readFile(args[1]))
  {
    //IO error
    cerr << "Program terminated: errors encountered while reading file(s)\n";
    return 0;
  }
  
  p.printProof();
  p.verifyProof();
  return 0;
}

