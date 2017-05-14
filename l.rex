/* Project:  COCKTAIL training
 * Descr:    a simple scanner generated with rex
 * Kind:     REX scanner specification (solution)
 * Author:   Dr. Juergen Vollmer <juergen.vollmer@informatik-vollmer.de>
 * $Id: l.rex.in,v 1.13 2016/11/11 10:38:11 vollmer Exp $
 */

SCANNER l_scan

EXPORT {
/* code to be put into Scanner.h */

# include "Position.h"

/* Token Attributes.
 * For each token with user defined attributes, we need a typedef for the
 * token attributes.
 * The first struct-field must be of type tPosition!
 */
typedef struct {tPosition Pos; char* Value;} tint_const;
typedef struct {tPosition Pos; char* Value;} tfloat_const;
typedef struct {tPosition Pos; char* Value;} tstring_const;
typedef struct {tPosition Pos; char* Value;} tidentifier_const;
typedef struct {tPosition Pos; char* Value;} toperator_const;
typedef struct {tPosition Pos; char* Value;} tcomment_const;
typedef struct {tPosition Pos; char* Value;} tbegin_const;

/* There is only one "actual" token, during scanning. Therfore
 * we use a UNION of all token-attributes as data type for that unique
 * token-attribute variable.
 * All token (with and without user defined attributes) have one
 * attribute: the source position:
 *     tPosition     Position;
 */
typedef union {
  tPosition	Position;
  tint_const	int_const;
  tfloat_const	float_const;
  tstring_const string_const;
  tidentifier_const identifier_const;
  toperator_const operator_const;
} l_scan_tScanAttribute;

/* Tokens are coded as int's, with values >=0
 * The value 0 is reserved for the EofToken, which is defined automatically
 */
# define tok_int_const		1
# define tok_float_const	2
# define tok_string_const	3
# define tok_identifier_const	4
# define tok_operator_const	5
# define tok_comment_const	6
# define tok_begin_const	7
}// EXPORT

GLOBAL {
  # include <stdlib.h>
  # include "rString.h"
} // GLOBAL

LOCAL {
 /* user-defined local variables of the generated GetToken routine */
 # define MAX_STRING_LEN 2048
  char string [MAX_STRING_LEN+1]; 
  int len;
  int nestingCount; /* comments in comments */
}  // LOCAL

DEFAULT {
  /* What happens if no scanner rule matches the input */
  MessageI ("Panic! Illegal character", xxError, l_scan_Attribute.Position, xxCharacter, (char*)*l_scan_TokenPtr);
} // DEFAULT

EOF {
  /* What should be done if the end-of-input-file has been reached? */

  /* E.g.: check hat strings and comments are closed. */
  switch (yyStartState) {
  case STD:
    /* ok */
    break;
  case STR: 
      Message ("Panic! You opend a String but never closed it!", xxFatal, l_scan_Attribute.Position);
  case COM: 
      Message ("Panic! You opend a Comment but never closed it!", xxFatal, l_scan_Attribute.Position);
  default:
    Message ("Panic! OOPS: that should not happen!!", xxFatal, l_scan_Attribute.Position);
    break;
  }

  /* implicit: return the EofToken */
} // EOF

DEFINE  /* some abbreviations */
  digit		= {0-9}       .
  letter	= {a-zA-Z}    .
  ourWord	= (letter|digit)* . 
  B		= {Bb} .
  E		= {Ee} .
  G		= {Gg} .
  I		= {Ii} .
  N		= {Nn} .
  
/* define start states, note STD is defined by default, separate several states by a comma */
/* START STRING */
START STR, COM

RULE
/*Keyword Begin  */
#STD# B E G I N : { return tok_begin_const;}

/* Integers */
#STD# digit+ :
	{
	 l_scan_Attribute.int_const.Value = malloc (l_scan_TokenLength+1);
	 l_scan_GetWord (l_scan_Attribute.int_const.Value);
	 return tok_int_const;
	}
/* Float */
#STD# (digit*\.)?digit+("E"("+"|"-")?digit+)? :
	{
	 l_scan_Attribute.float_const.Value = malloc (l_scan_TokenLength+1);
	 l_scan_GetWord (l_scan_Attribute.float_const.Value);
	 return tok_float_const;
	}
/*comment */
#STD# "#"ANY* : 
	  {
	    return tok_comment_const;
	  }

#STD,COM# "(#" : 
	{
	   yyStart (COM);
	   nestingCount++;
	}
	
#COM# "#)" : 
	{
	    nestingCount--;
	    if(nestingCount == 0) yyStart(STD);
	}
	
#COM# ourword :{}	     

/* String */
#STD# \" :
	 {
	   yyStart (STR);
	   len = 0;
	 }

#STR# \" : { 
	    yyStart(STD);
	    string[len] = '\0';
	    l_scan_Attribute.string_const.Value = malloc (len+1);
	    strcpy (l_scan_Attribute.string_const.Value, string);
	    return tok_string_const;
	    }
	    
#STR# ourWord :
	{ /* we're inside the string */
          if (len + l_scan_TokenLength+1 >= MAX_STRING_LEN) {
	    Message ("String zu lang", xxError, l_scan_Attribute.Position);
	    len = 0;
	  } else {
  	    len += l_scan_GetWord (&string[len]);
	  }
        }
        
#STR# \\  : {string[len++] = '\\';} /* " allow \\ */ 

#STR# \\ \" : {string[len++] = '"';} /* " is ok if / before */ 

#STR# \n : { Message ("Panic! no linebreaks within a string...", xxFatal, l_scan_Attribute.Position); }

/* Identifier */
#STD# (letter)(letter|digit)* :
	 {
	  l_scan_Attribute.identifier_const.Value = malloc (l_scan_TokenLength+1);
	  l_scan_GetWord (l_scan_Attribute.identifier_const.Value);
	  return tok_identifier_const;
	 }
/* Operator */
#STD# (\+|\*|\/|\-) :
	{
	  l_scan_Attribute.operator_const.Value = malloc (l_scan_TokenLength+1);
	  l_scan_GetWord (l_scan_Attribute.operator_const.Value);
	  return tok_operator_const;
	}

	
/* Please add rules for: (don't forget to adapt main()) */
/* Float numbers */

/* case insensitive keywords: BEGIN PROCEDURE END CASE */

/* identifiers */

/* comment up to end of line */

/* C-style comment */

/* Modula2-style nested comment */

/* double quote delimited strings */
/**********************************************************************/
