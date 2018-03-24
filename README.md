# Boolean-Inference-Checker
This project is very similar to my Boolean-Expression-Evaluator-C, in that they are both based on the Backus–Naur form(BNF), which you can read more about here: https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form. What makes this one different is that,essentially, this program also allows variables such as "foo" instead of only T,F, etc. This program will then take an input, for example
"p => q, ~q :. ~p", and evaluate it with every possible combonation of T and F in the place of the logical variables(varaibles that aren't explicity defined as T or F) and ultimately evaluate it to one of four things: symbol error, grammar error, valid or invalid.
Most of the challeneges were overcome with the use of recursive descent and lots of caffine.

Examples of these outputs:
"=> u F *" is "symbol error"
"u => :. :. F pr" is "grammar error"
"u => p, u :. p" is "valid"
"pa v q, ~pa v ~q :. pa <=> q" is "invalid". 

There are still a few cases which are giving me some issues but in general the program is complete.

A logical variable is a string of one or more lowercase letters a,...,z, not beginning with v. For example, the following are logical variables: "p", "foo", "qrs", "ivt".

A symbol is either a logical variable or one of the following eleven strings:  "T", "F", "^", "v", "~", "=>", "<=>", "(", ")", ":.", "," Note that, by comparison with the Boolean expression evaluator, the new symbols are logical variables, the comma, and the therefore symbol ":.".

A symbol string is the concatenation of zero or more symbols and/or spaces. For example, the following are symbol strings:

"T :.  "
"=> Tv~  F(( F"
"T foo  => F ^ (F pr :. ) v F)"
""
"  "
"Foo" --> ["F", "oo"] 

and the following are not symbols strings

"T X  "
"  : .F => T"
"TF p*^)"
"FOO"

For example, the string "foo ^ bar => T" should generate the tokens ["foo","^","bar","=>","T"]

The rules this program was made on were as follows: 

Const  →   "T" |  "F"
  U ::= Const |  "(" B ")" | Lvar
	N  →   U | "~" N
	C  →   N | C "^" N
	D  →   C | D "v" C     // note, this rule has been corrected
	I  →   D | D "=>" I
	B  →   I | I "<=>" B
  
  A Boolean inference consists of one or more Boolean expressions separated by commas, followed by a therefore sign ":.", followed by a Boolean expression. Formally,
  
	Ps  ::=  Bexp | Bexp "," Ps

	Inf  ::=  Ps ":." Bexp
  
For example, the following strings represent valid inferences:
"p => q, ~q :. ~p"
"p ^ ~q => r ^ ~r :. ~p v q"
"p => q, q => r, r => s v t, ~s, p :. t"

and the following strings represent invalid inferences:

"p => q, ~p :. ~q"
"p => q, q => r  :. q => p ^ r"


