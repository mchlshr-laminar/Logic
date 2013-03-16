#include "Statements/StatementTree.hpp"
#include "Justifications/EquivalenceRules.hpp"
#include "Justifications/InferenceRules.hpp"
#include <iostream>

#define stripParens StatementTree::stripParens

using std::cout;
using std::endl;

int main(int nargs, char** args)
{
  if(nargs < 2) return 0;
  InferenceRule MP("b", "Modus Ponens");
  MP.addRequiredForm("a>b");
  MP.addRequiredForm("a");
  ProofStatement result(args[1]);
  if(result.getStatementData()->isValid()) cout << "Valid formulation\n";
  else cout << "Invalid formulation\n";
  result.setJustification(&MP);
  for(int i = 2; i < nargs; i++)
  {
    ProofStatement* temp = new ProofStatement(args[i]);
    result.toggleAntecedent(temp);
  }
  if(result.isJustified()) cout << "Justified\n";
  else cout << "Not justified\n";
  return 0;
}
