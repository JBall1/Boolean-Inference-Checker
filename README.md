# Boolean-Inference-Checker
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
