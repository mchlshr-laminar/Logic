# Propositional Logic Verifier
Reads a proof from an input file, displays the proof, and determines whether or
not it is an allowable proof.


## Build & Run
Project builds with CMake. In the project's root directory, use commands:
```
mkdir Build
cmake -S . -B ./Build 
cmake --build ./Build --config Release
cmake --install ./Build
```

An executable will be generated in project directory/bin. Run with one command line argument 
to specify the name of the input file.


## Sentence Format
Sentences consist of atoms, and, or, not, implies/only if, iff, & parentheses. Atom names are
an alphanumeric string.

Recognized operator characters:  

**And:**
```
^
*
&
```
</br>

**Or:**
```
+
|
```
</br>
  
**Not:**
```
~
!
```
</br>
  
**Implies:**
```
>
```
</br>
  
**Iff:**
```
=
```
  
Only standard parentheses will be parsed as such (no brackets). Order of operations is: not, 
and, or, implies, iff, then left to right within an operator type. Generalized disjunction/
conjunction is not currently supported.

Any double negation will be removed and consolidated into a flag for each node in a 
sentence's tree stating whether or not it is negated (constructive logic is not supported).


## Justification Rules
Letters here are sentence variables. <a, b> as an antecedent means there must be a subproof 
antecedent with a as the assumption which includes sentence b. An equivalence rule can be 
used to justify multiple applications of the equivalence at once 
(e.g. a|(b&c) <==> (c&b)|a by Commutation).

The usable rules are read from the rules.xml file in the project directory.

### Equivalence Rules
Association
```
a&(b&c) <==> (a&b)&c
a|(b|c) <==> (a|b)|c
```

Commutation
```
a&b <==> b&a
a|b <==> b|a
```

DeMorgan
```
!(a&b) <==> !a|!b
```

Distribution
```
a&(b|c) <==> (a&b)|(a&c)
a|(b&c) <==> (a|b)&(a|c)
```

Idempotence
```
a&a <==> a
a|a <==> a
```

Absorption
```
a&(a|b) <==> a
a|(a&b) <==> a
```

Reduction
```
a&(!a|b) <==> a&b
a|(!a&b) <==> a|b
```

Adjacency
```
(a|b)&(a|!b) <==> a
(a&b)|(a&!b) <==> a
```

Implication
```
a>b <==> !a|b
```

Contraposition
```
a>b <==> !b>!a
```

Exportation
```
a>(b>c) <==> (a&b)>c
```

Equivalence
```
a=b <==> (a>b)&(b>a)
a=b <==> (a&b)|(!a&!b)
```

Conditional Distribution
```
a>(b&c) <==> (a>b)&(a>c)
a>(b|c) <==> (a>b)|(a>c)
(a|b)>c <==> (a>c)&(b>c)
(a&b)>c <==> (a>c)|(b>c)
```

Conditional Reduction
```
(a>b)&a <==> a&b
(a>b)&!b <==> !a&!b
```

### Inference Rules
Conjunction
```
a
b
---
a&b
```

Simplification
```
a&b       a&b
----  or  ----
a         b
```

Addition
```
a         b
----  or  ----
a|b       a|b
```

Proof by Cases
```
a|b
<a, c>
<b, c>
-------
c
```

Indirect Proof
```
<!a, b&!b>
-----------
a
```

Conditional Proof
```
<a, b>
------
a>b
```

Modus Ponens
```
a>b
a
----
b
```

Biconditional Proof
```
<a, b>
<b, a>
-------
a=b
```

Binconditional Elimination
```
a=b       a=b
b         a
----  or  ----
a         b
```

Modus Tollens
```
a>b
!b
----
!a
```

Hypothetical Syllogism
```
a>b
b>c
----
a>c
```

Reiteration
```
a
----
a
```

Excluded Middle
```
----
a|!a
```

## Input File Format
Each line of the input file is a 3-letter command generally followed by additional 
information. Whitespace at the beginning of a line will be ignored. Allowable lines are:

`pre <sentence>`</br>
Makes <sentence> into a premise in the proof. Note that all "pre" lines must appear before 
all "lin", "sub", or "end" lines.

`gol <sentence>`</br>
Sets the goal of the proof to <sentence>

`lin <sentence>:<rule name>:<antecedents>`</br>
Adds <sentence> as the next line in the proof. If any subproofs have been started but not 
ended, the line will be in those subproofs. <rule name> is the name of the equivalence/
inference rule used to justify the line, and <antecedents> is a space-delimited list of the 
line numbers of the antecedents. The line number of an antecedent refers to the line in the 
input file on which it appears (starting at 1). An antecedent that's a subproof should give 
the line number of the subproof's assumption.

`sub <sentence>`</br>
Opens a subproof with <sentence> as the assumption. If any subproofs are already open, the 
new subproof will be in them.

`end`</br>
Closes the most recently opened subproof which has not already been closed.

`inf <rule name>:<filename>`</br>
Creates a new inference rule that can be used elsewhere in the proof. The named file must 
contain a lemma proof where the goal statement is the consequent form of the new rule and 
each of the premises of the lemma proof is one of the antecedent forms of the rule. 
Subsequent lines in the top-level proof can use this rule as justification. The lemma proof 
must be justified for the rule to be added.

'equ <rule name>:<filename 1>:<filename 2>'</br>
Creates a new equivalence rule that can be used elsewhere in the proof. File 1 must contain 
a lemma proof with a single premise of the first equivalent form and a goal of the second 
equivalent form. File 2 is the reverse. Both proofs must be justified for the rule to 
be added.

### Example Input File
```
pre a|b
pre !b
lin !a>b:Implication:1
sub !a
  lin b:Modus Ponens:3 4
  lin b&!b:Conjunction:2 5
end
lin a:Indirect Proof:4
```

***

Copyright (c) 2013-2023 Michael Usher

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

rapidxml is copyright Marcin Kalicinski and redistributed under the Boost license or the MIT license
