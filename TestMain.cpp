#include "Statements/StatementTree.hpp"
#include "Statements/ProofStatement.hpp"
#include "Statements/SubProof.hpp"
#include "Justifications/EquivalenceRules.hpp"
#include "Justifications/InferenceRules.hpp"
#include <iostream>

#define stripParens StatementTree::stripParens

using std::cout;
using std::endl;

/*int main(int nargs, char** args)
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
}*/

int main(int nargs, char** args)
{
  if(nargs < 3) return 0;
  InferenceRule CP("a>b", "Conditional Proof");
  CP.addRequiredForm("b", "a");
  ProofStatement result(args[1]);
  if(result.getStatementData()->isValid()) cout << "Valid formulation\n";
  else cout << "Invalid formulation\n";
  result.setJustification(&CP);
  
  SubProof ant(args[2]);
  for(int i = 3; i < nargs; i++)
  {
    ProofStatement* temp = new ProofStatement(args[i]);
    ant.toggleChild(temp);
  }
  result.toggleAntecedent(&ant);
  
  if(result.isJustified()) cout << "Justified\n";
  else cout << "Not justified\n";
}

/*int main(int nargs, char** args)
{
  if(nargs < 2) return 0;
  EquivalenceRule DeM("DeMorgan");
  DeM.addEquivalentPair("(a&b)&c", "a&(b&c)");
  DeM.addEquivalentPair("(a|b)|c", "a|(b|c)");
  ProofStatement result(args[1]);
  if(result.getStatementData()->isValid()) cout << "Valid formulation\n";
  else cout << "Invalid formulation\n";
  result.setJustification(&DeM);
  for(int i = 2; i < nargs; i++)
  {
    ProofStatement* temp = new ProofStatement(args[i]);
    result.toggleAntecedent(temp);
  }
  if(result.isJustified()) cout << "Justified\n";
  else cout << "Not justified\n";
  return 0;
}*/

