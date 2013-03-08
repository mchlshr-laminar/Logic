#include "Statements/StatementTree.hpp"
#include "Justifications/EquivalenceRules.hpp"
#include <iostream>

#define stripParens StatementTree::stripParens

using std::cout;
using std::endl;

int main(int nargs, char** args)
{
  if(nargs < 2) return 0;
  EquivalenceRule DeM("!(a&b)", "!a|!b");
  StatementTree steve(args[1]);
  StatementTree bob(args[2]);
  if(DeM.areEquivalent(&steve, &bob)) cout << "equiv\n";
  else cout << "not equiv\n";
  /*cout << steve.createDisplayString() << endl;
  StatementTree evets(steve, false);
  cout << evets.createDisplayString() << endl;*/
  
  return 0;
}
