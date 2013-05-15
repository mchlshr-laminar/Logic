Reads a proof from an input file, displays the proof, and determines whether or
not it is an allowable proof.

In Ubuntu, run with ./ubuntu_verifier <input file path>
In windows, run windows_verifier <input file path>
The included windows executable must be in the same directory as cygwin1.dll

builder is a bash script to recompile the program. The resulting executable will
be called proof_checker.

============Sentence Format============
Sentences consist of atoms, and, or, not, implies/only if, iff, & parentheses.
Atom names are an alphanumeric string.

Allowable operator characters:
And:
^
*
&

Or:
+
|

Not:
~
!

Implies:
>

Iff:
=

Only standard parentheses will be parsed as such (no brackets). If parentheses
are not used, order of operations is:
not, and, or, implies, iff
& left to right within an operator type. Generalized disjunction/conjunction is
not currently supported.

Any double negation will be removed and consolidated into a flag for each node
in a sentence's tree stating whether or not it is negated.

===========Justification Rules===========
Letters here are sentence variables. <a, b> as an antecedent means there must be
a subproof antecedent with a as the assumption which includes sentence b. An
equivalence rule can be used to justify multiple applications of the equivalence
at once (e.g. a|(b&c) <==> (c&b)|a by Commutation).

====Equivalence Rules
Association
a&(b&c) <==> (a&b)&c
a|(b|c) <==> (a|b)|c

Commutation
a&b <==> b&a
a|b <==> b|a

DeMorgan
!(a&b) <==> !a|!b

Distribution
a&(b|c) <==> (a&b)|(a&c)
a|(b&c) <==> (a|b)&(a|c)

Idempotence
a&a <==> a
a|a <==> a

Absorption
a&(a|b) <==> a
a|(a&b) <==> a

Reduction
a&(!a|b) <==> a&b
a|(!a&b) <==> a|b

Adjacency
(a|b)&(a|!b) <==> a
(a&b)|(a&!b) <==> a

Implication
a>b <==> !a|b

Contraposition
a>b <==> !b>!a

Exportation
a>(b>c) <==> (a&b)>c

Equivalence
a=b <==> (a>b)&(b>a)
a=b <==> (a&b)|(!a&!b)

Conditional Distribution
a>(b&c) <==> (a>b)&(a>c)
a>(b|c) <==> (a>b)|(a>c)
(a|b)>c <==> (a>c)&(b>c)
(a&b)>c <==> (a>c)|(b>c)

Conditional Reduction
(a>b)&a <==> a&b
(a>b)&!b <==> !a&!b

====Inference Rules
Conjunction
a
b
----
a&b

Simplification
a&b    a&b
----   ----
a      b

Addition
a      b
----   ----
a|b    a|b

Proof by Cases
a|b
<a, c>
<b, c>
-------
c

Indirect Proof
<!a, b&!b>
-----------
a

Conditional Proof
<a, b>
------
a>b

Modus Ponens
a>b
a
----
b

Biconditional Proof
<a, b>
<b, a>
-------
a=b

Binconditional Elimination
a=b     a=b
b       a
----    ----
a       b

Modus Tollens
a>b
!b
----
!a

Hypothetical Syllogism
a>b
b>c
----
a>c

Reiteration
a
----
a

Excluded Middle
----
a|!a

============Input file format============
Each line of the input file is a 3-letter command possibly followed by additional
information. Whitespace at the beginning of a line will be ignored. Allowable
lines are:

pre <sentence>
Makes <sentence> into a premise in the proof. Note that all "pre" lines must
appear before all "lin", "sub", or "end" lines.

gol <sentence>
Sets the goal of the proof to <sentence>

lin <sentence>:<rule name>:<antecedents>
Adds <sentence> as the next line in the proof. If any subproofs have been
started but not ended, the line will be in those subproofs. <rule name> is the
name of the equivalence/inference rule used to justify the line, and
<antecedents> is a space-delimited list of the line numbers of the antecedents.
The line number of an antecedent refers to the line in the input file on which
it appears (starting at 1). A subproof antecedent should give the line number of
the subproof's assumption.

sub <sentence>
Opens a subproof with <sentence> as the assumption. If any subproofs are already
open, the new subproof will be in them.

end
Closes the most recently opened subproof which has not already been closed.

===========Example Input File===========
pre a|b
pre !b
lin !a>b:Implication:1
sub !a
  lin b:Modus Ponens:3 4
  lin b&!b:Conjunction:2 5
end
lin a:Indirect Proof:4

=========================================
Created by Michael Usher, spring 2013
