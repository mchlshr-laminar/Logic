#ifndef __STATEMENT_TREE_H_
#define __STATEMENT_TREE_H_

#include <list>

class StatementTree;

/// <summary>
/// Type for the child nodes of a StatementTree
/// </summary>
typedef std::list<StatementTree*> tree_list;
/// <summary>
/// Iterator for tree_list
/// </summary>
typedef std::list<StatementTree*>::iterator child_itr;

/// <summary>
/// Abstract syntax tree for a logical statement. Negation is consolidated as
/// a boolean flag on nodes, rather than a separate node. Thus all non-leaf
/// nodes are binary.
/// </summary>
class StatementTree
{
  public:
  //enum not used for node type for the sake of order of operations.
  const static int ATOM = 0, IFF = 1, IMPLIES = 2, OR = 3, AND = 4, NOT = 5;
  const static int OP_START = 1, OP_END = 5;
  const static int ONLY = 0, LEFT = 0, RIGHT = 1;
  const static int IS_INVALID = 0, IS_VALID = 1, VALIDITY_UNKNOWN = 2;
  
  private:
  int node_type;
  std::list<StatementTree*> children;
  char* atom_name;
  bool is_affirmed;
  int validity; //Caches well-formedness
  
  void consolidateNegation();
  void consolidateChildren();
  
  public:

  /// <summary>
  /// Construct a syntax tree by parsing an infix notation string.
  /// </summary>
  /// <param name="input">Logical sentence to parse into a tree</param>
  StatementTree(const char* input);
  
  /// <summary>
  /// Copy constructor
  /// </summary>
  /// <param name="other">Syntax tree to copy</param>
  /// <param name="dontNegate">
  ///	If false, the negation flag at the root of the new tree will be
  ///	inverted compared to the original tree.
  /// </param>
  StatementTree(StatementTree& other, bool dontNegate=true);
  ~StatementTree();
  
#pragma region Iteration
  child_itr begin();
  child_itr end();
#pragma endregion

#pragma region Properties

  /// <summary>
  /// Status of the negation flag. If a node is negated, it's equivalent to
  /// that node being the child of a unitary negation operator.
  ///                !
  ///                |
  ///   !^           ^
  ///  /  \   ==   /   \
  /// a    b      a     b
  /// </summary>
  /// <returns>False if this node is negated, true otherwise</returns>
  bool isAffirmed();

  /// <summary>
  /// Whether or not this node and its children are a well-formed sentence.
  /// With the negation flags in use, this means all leaf/atom nodes have no
  /// children and all other nodes have two. Also checks invalid characters
  /// such as operators in atom names.
  /// </summary>
  /// <returns>True if this statement (sub)tree is valid</returns>
  bool isValid();

  /// <summary>
  /// Which type of node is this? Options are:
  /// 0: Atomic statement (proposition)
  /// 1: Biconditional implication/equality
  /// 2: Implication
  /// 3: Disjunction/or
  /// 4: Conjunction/and
  /// 5: Negation. These nodes should be flattened into flags after parsing.
  /// </summary>
  /// <returns>Node type</returns>
  int nodeType();

  /// <summary>
  /// For an atomic statement, the string for this particular proposition.
  /// </summary>
  /// <returns>Proposition</returns>
  char* atomName();

  /// <summary>
  /// Does this node represent an associative operator?
  /// </summary>
  /// <returns>
  ///	True for biconditional implication, conjunction, and disjunction
  /// </returns>
  bool isAssociative();

  /// <summary>
  /// Does this node represent a commutative operator?
  /// </summary>
  /// <returns>
  ///	True for biconditional implication, conjunction, and disjunction
  /// </returns>
  bool isCommutative();
#pragma endregion

  /// <summary>
  /// Flip the negation flag on this node
  /// </summary>
  void negate();

  /// <summary>
  /// Is this statement tree equivalent to another one? For an atom, this is
  /// true if the other also is and the proposition is the same. For other
  /// nodes, the operator and negation flag must be the same, and the
  /// corresponding child nodes must be equivalent.
  /// </summary>
  /// <param name="other">Statement tree to compare with</param>
  /// <returns>True if the trees are equivalent</returns>
  bool equals(StatementTree& other);
  
#pragma region ParsingHelpers
  /// <summary>
  /// Removes matching parentheses from the start and end of a string, until
  /// there are no sets of parentheses that enclose the whole string.
  /// </summary>
  /// <param name="input">String to remove parens from</param>
  static void stripParens(char* input);

  /// <summary>
  /// Returns the position of an operator which is not enclosed by any
  /// parentheses in a string. If there are multiple such operators, the one
  /// returned will be the earliest one in the order of operations. If there's
  /// still multiple, will return the first (leftmost) one.
  /// </summary>
  /// <param name="input">String to check for operators</param>
  /// <returns>
  ///	Position of the relevant operator in the string. If none is found,
  ///	returns the end of the string.
  /// </returns>
  static int findOperator(char* input);

  /// <summary>
  /// Returns which operation a character represents
  /// </summary>
  /// <param name="input">Operator character to check</param>
  /// <returns>
  ///	= is IFF
  ///	> is IMPLIES
  ///	| and + are OR
  ///	& and ^ are AND
  ///	! and ~ are NOT
  /// If the character is anything else, returns ATOM.
  /// </returns>
  static int operatorType(char input);

  /// <summary>
  /// Returns the character for an operation.
  /// </summary>
  /// <param name="input">Operation</param>
  /// <returns>=, >, |, &, or !. Returns space for an ATOM</returns>
  static char typeOperator(int input);
#pragma endregion
  
#pragma region Display
  /// <summary>
  /// Constructs a string representing this syntax tree in infix notation.
  /// Will use parentheses rather than relying on order of operations.
  /// </summary>
  /// <returns>Display string</returns>
  char* createDisplayString();

  /// <summary>
  /// Constructs an ASCII graph of the tree structure. Used for debugging
  /// </summary>
  /// <param name="depth">
  ///	Horizontal alignment of this subtree in the graph
  /// </param>
  void DrawDebugGraph(int depth = 0);
#pragma endregion

};

#endif
