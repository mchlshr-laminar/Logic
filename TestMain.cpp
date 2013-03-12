#include "Statements/StatementTree.hpp"
#include "Justifications/EquivalenceRules.hpp"
#include <iostream>

#define stripParens StatementTree::stripParens

using std::cout;
using std::endl;

int main(int nargs, char** args)
{
  EquivalenceRule DeM("!(a&b)", "!a|!b", "DeMorgan"); 
  ProofStatement result(args[1]);
  result.setJustification(&DeM);
  for(int i = 2; i < nargs; i++)
  {
    ProofStatement* temp = new ProofStatement(args[i]);
    result.toggleAntecedent(temp);
  }
  if(result.isJustified()) cout << "Justified\n";
  else cout << "Not justified\n";
  return 0;
}
