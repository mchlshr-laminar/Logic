#include "Statements/StatementTree.hpp"
#include "Statements/ProofStatement.hpp"
#include "Statements/SubProof.hpp"
#include "Justifications/EquivalenceRules.hpp"
#include "Justifications/InferenceRules.hpp"
#include "Proof/Proof.hpp"
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
  Proof p;
  p.addPremiseLine();
  p.setStatement("a>b");
  
  p.addPremiseLine();
  p.setStatement("a>!b");
  
  p.addSubproofLine();
  p.setStatement("a");
  
  p.addLine();
  p.setStatement("b");
  p.setJustification("Modus Ponens");
  p.toggleAntecedent(1);
  p.toggleAntecedent(3);
  
  p.addLine();
  p.setStatement("!b");
  p.setJustification("Modus Ponens");
  p.toggleAntecedent(2);
  p.toggleAntecedent(3);
  
  p.addLine();
  p.setStatement("b&!b");
  p.setJustification("Conjunction");
  p.toggleAntecedent(4);
  p.toggleAntecedent(5);
  
  p.endSubproof();
  p.setStatement("!a");
  p.setJustification("Indirect Proof");
  p.toggleAntecedent(3);
  
  p.addLine();
  p.setStatement("!a");
  p.setJustification("Indirect Proof");
  p.toggleAntecedent(3);
  
  p.printProof();
  p.verifyProof();
  return 0;
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

