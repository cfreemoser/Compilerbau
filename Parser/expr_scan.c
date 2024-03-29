/* $Id: Scanner.c,v 2.67 2009/10/13 14:13:39 grosch Exp $ */

# define rbool		char
# define rtrue		1
# define rfalse		0
# define StdIn		0

# include "expr_scan.h"

# ifndef EXTERN_C_BEGIN
# define EXTERN_C_BEGIN
# define EXTERN_C_END
# endif

EXTERN_C_BEGIN
#    include "expr_scanSource.h"
#    include "rSystem.h"
#    include "General.h"
#    include "DynArray.h"
#    include "Position.h"
#    include "Errors.h"
EXTERN_C_END

# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>

# define yyStart(State)	{ yyPreviousStart = yyStartState; yyStartState = State;}
# define yyPrevious	{ yyStateRange s = yyStartState; \
			yyStartState = yyPreviousStart; yyPreviousStart = s; }
# define yyEcho		{ (void) fwrite (expr_scan_TokenPtr, sizeof (yytChar), \
			expr_scan_TokenLength, stdout); }
# define yyEol(Column)	{ yyLineCount ++; \
			yyLineStart = (yytusChar *) expr_scan_TokenPtr + \
			expr_scan_TokenLength - 1 - (Column); }
# if expr_scan_xxMaxCharacter < 256
#  define output(c)	(void) putchar ((int) c)
# else
#  define output(c)	(void) printf ("%lc", c)
# endif
# define yyColumn(Ptr)	((int) ((Ptr) - (yytChar *) yyLineStart))
# define yyOffset(Ptr)	(yyFileOffset + ((Ptr) - yyChBufferStart2))

# define yytChar	expr_scan_xxtChar
# define yytusChar	expr_scan_xxtusChar

# define yyDNoState		0
# define yyFirstCh	(yytusChar) '\0'
# define yyEolCh	(yytusChar) '\12'
# define yyEobCh	(yytusChar) '\177'
# define yyDStateCount	15
# define yyTableSize	453
# define yyEobState	7
# define yyDefaultState	8
# define yyToClassArraySize	0
# define STD	1
# define xxGetWord

static void yyExit ARGS ((void))
{ rExit (1); }

typedef unsigned short	yyStateRange;
typedef struct { yyStateRange yyCheck, yyNext; } yyCombType;

	yytChar *	expr_scan_TokenPtr	;
	int		expr_scan_TokenLength	;
	expr_scan_tScanAttribute	expr_scan_Attribute	;
	void		(* expr_scan_Exit)	ARGS ((void)) = yyExit;

static	void		yyInitialize	ARGS ((void));
static	void		yyErrorMessage	ARGS ((int yyErrorCode));
static	yytChar		input		ARGS ((void));
static	void		unput		ARGS ((yytChar));
static	void		yyLess		ARGS ((int));

static	yyCombType	yyComb		[yyTableSize   + 1] = {
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   1,   10}, 
{   1,    9}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   1,   11}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   1,   13}, {   1,   12}, {   1,   14}, {   1,   15}, {   0,    0}, 
{   2,    4}, {   4,    5}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   1,    3}, {   0,    0}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, {   1,    3}, 
{   1,    3}, {   1,    3}, {   1,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   6,    7}, {   0,    0}, 
{   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   0,    0}, {   0,    0}, {   0,    0}, {   0,    0}, 
{   3,    3}, {   0,    0}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, {   3,    3}, 
{   3,    3}, {   3,    3}, {   3,    3}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   0,    0}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   0,    0}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   0,    0}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
{   5,    5}, {   5,    5}, {   5,    5}, {   5,    5}, 
};
static	yyCombType *	yyBasePtr	[yyDStateCount + 1] = { 0,
& yyComb [   0], & yyComb [   0], & yyComb [  75], & yyComb [   1], 
& yyComb [ 198], & yyComb [   6], & yyComb [   0], & yyComb [   0], 
& yyComb [   0], & yyComb [   0], & yyComb [   0], & yyComb [   0], 
& yyComb [   0], & yyComb [   0], & yyComb [   0], 
};
static	yyStateRange	yyDefault	[yyDStateCount + 1] = { 0,
    6,     1,     6,     6,     4,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0, 
};
static	yyStateRange	yyEobTrans	[yyDStateCount + 1] = { 0,
    0,     0,     0,     0,     5,     0,     0,     0,     0,     0, 
    0,     0,     0,     0,     0, 
};

# if expr_scan_xxMaxCharacter < 256
#  define yyGetLine	expr_scan_GetLine
# else
#  define yyGetLine	expr_scan_GetWLine
# endif

# if yyToClassArraySize == 0
#  define yyToClass(x) (x)
# else
typedef	unsigned short	yytCharClass;

static	yytCharClass	yyToClassArray	[yyToClassArraySize] = {
};
#  if expr_scan_xxMaxCharacter < yyToClassArraySize
#   define yyToClass(x) (yyToClassArray [x])
#  else
#   define yyToClass(x) \
	((x) < yyToClassArraySize ? yyToClassArray [x] : yyToClassFunction (x))

static	yytusChar	yyToClassUpb	[yyToClassRangeSize] = {
};
static	yytCharClass	yyToClassClass	[yyToClassRangeSize] = {
};

static yytCharClass yyToClassFunction
# ifdef HAVE_ARGS
   (yytusChar yyCh)
# else
   (yyCh) yytusChar yyCh;
# endif
{
   register int yyLower = 1, yyUpper = yyToClassRangeSize;
   while (yyUpper - yyLower > 1) {			/* binary search */
      register int yyMiddle = (yyLower + yyUpper) / 2;
      if (yyToClassUpb [yyMiddle] < yyCh)
	 yyLower = yyMiddle;
      else
	 yyUpper = yyMiddle;
   }
   if (yyToClassUpb [yyLower - 1] < yyCh && yyCh <= yyToClassUpb [yyLower])
      return yyToClassClass [yyLower];
   else
      return yyToClassClass [yyUpper];
}

#  endif
# endif

static	yyStateRange	yyStartState	= STD;
static	yyStateRange	yyPreviousStart	= STD;
static	int		yySourceFile	= StdIn;
static	rbool		yyEof		= rfalse;
static	long		yyBytesRead	= 0;
static	long		yyFileOffset	= 0;
static	unsigned int	yyLineCount	= 1;
static	yytusChar *	yyLineStart	;
static	yytChar *	yyChBufferStart2;

					/* Start State Stack: StStSt	*/

# if defined xxyyPush | defined xxyyPop
# define		yyInitStStStackSize	16

static	yyStateRange *	yyStStStackPtr	;
static	unsigned long	yyStStStackSize	= 0;
static	unsigned int	yyStStStackIdx	= 0;
# endif

# ifdef xxyyPush
static void yyPush
# ifdef HAVE_ARGS
   (yyStateRange yyState)
# else
   (yyState) yyStateRange yyState;
# endif
{
   if (yyStStStackIdx == yyStStStackSize) {
      if (yyStStStackSize == 0) {
	 yyStStStackSize = yyInitStStStackSize;
	 MakeArray ((char * *) & yyStStStackPtr, & yyStStStackSize,
			(unsigned long) sizeof (yyStateRange));
      } else {
	 ExtendArray ((char * *) & yyStStStackPtr, & yyStStStackSize,
			(unsigned long) sizeof (yyStateRange));
      }
      if (yyStStStackPtr == NULL) yyErrorMessage (xxScannerOutOfMemory);
   }
   yyStStStackPtr [yyStStStackIdx ++] = yyStartState;
   yyStart (yyState);
}
# endif

# ifdef xxyyPop
static void yyPop ARGS ((void))
{
   yyPreviousStart = yyStartState;
   if (yyStStStackIdx > 0)
      yyStartState = yyStStStackPtr [-- yyStStStackIdx];
   else
      yyErrorMessage (xxStartStackUnderflow);
}
# endif

static void yyTab1	ARGS ((int yya));

# define yyTab		yyTab1 (0)
# define yyTab2(a,b)	yyTab1 (a)

void expr_scan_ErrorAttribute
# ifdef HAVE_ARGS
   (int Token, expr_scan_tScanAttribute * Attribute)
# else
   (Token, Attribute)
   int Token;
   expr_scan_tScanAttribute * Attribute;
# endif
   { }

# ifndef yySetPosition
# define yySetPosition expr_scan_Attribute.Position.Line = yyLineCount; \
expr_scan_Attribute.Position.Column = (int) ((yytusChar *) expr_scan_TokenPtr - yyLineStart);
# endif

# undef yyTab
# undef yyTab2

# ifndef yyInitBufferSize
# define yyInitBufferSize	1024 * 8 + 256
# endif
# ifndef yyInitFileStackSize
# define yyInitFileStackSize	8
# endif
# ifndef yyTabSpace
# define yyTabSpace		8
# endif

static void yyTab1
# ifdef HAVE_ARGS
   (int yya)
# else
   (yya) int yya;
# endif
   { yyLineStart -= (yyTabSpace - 1 - ((yytusChar *) expr_scan_TokenPtr -
	yyLineStart + yya - 1)) & (yyTabSpace - 1); }

# define yyTab		yyLineStart -= (yyTabSpace - 1 - \
((yytusChar *) expr_scan_TokenPtr - yyLineStart - 1)) & (yyTabSpace - 1)
# define yyTab1(a)	yyLineStart -= (yyTabSpace - 1 - \
((yytusChar *) expr_scan_TokenPtr - yyLineStart + (a) - 1)) & (yyTabSpace - 1)
# define yyTab2(a,b)	yyLineStart -= (yyTabSpace - 1 - \
((yytusChar *) expr_scan_TokenPtr - yyLineStart + (a) - 1)) & (yyTabSpace - 1)

# ifndef EBCDIC
#  if expr_scan_xxMaxCharacter < 256
#   include <ctype.h>
#   define yyToUpper(x)	toupper (x)
#   define yyToLower(x)	tolower (x)
#  else
#   include <wctype.h>
#   define yyToUpper(x)	towupper (x)
#   define yyToLower(x)	towlower (x)
#  endif
# else
#  define yyToLower(x)	yyToLowerArray [x]
#  define yyToUpper(x)	yyToUpperArray [x]

# ifdef xxGetLower
static	yytusChar	yyToLowerArray	[] = {
'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
'\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27',
'\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37',
'\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
'\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47',
'\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F',
'\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57',
'\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67',
'\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77',
'\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
'\x80',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
   'h',    'i', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
'\x90',    'j',    'k',    'l',    'm',    'n',    'o',    'p',
   'q',    'r', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
'\xA0', '\xA1',    's',    't',   'u',     'v',    'w',    'x',
   'y',    'z', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7',
'\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
'\xC0',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
   'h',    'i', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
'\xD0',    'j',    'k',    'l',    'm',    'n',    'o',    'p',
   'q',    'r', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
'\xE0', '\xE1',    's',    't',    'u',    'v',    'w',    'x',
   'y',    'z', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7',
'\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};
# endif

# ifdef xxGetUpper
static	yytusChar	yyToUpperArray	[] = {
'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
'\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
'\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27',
'\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37',
'\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
'\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47',
'\x48', '\x49', '\x4A', '\x4B', '\x4C', '\x4D', '\x4E', '\x4F',
'\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57',
'\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67',
'\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77',
'\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
'\x80',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
   'H',    'I', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
'\x90',    'J',    'K',    'L',    'M',    'N',    'O',    'P',
   'Q',    'R', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
'\xA0', '\xA1',    'S',    'T',    'U',    'V',    'W',    'X',
   'Y',    'Z', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7',
'\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
'\xC0',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
   'H',    'I', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
'\xD0',    'J',    'K',    'L',    'M',    'N',    'O',    'P',
   'Q',    'R', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
'\xE0', '\xE1',    'S',    'T',    'U',    'V',    'W',    'X',
   'Y',    'Z', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7',
'\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};
# endif

# endif

static	yyStateRange	yyInitStateStack [4] = { yyDefaultState };
static	yyStateRange *	yyStateStack	= yyInitStateStack;
static	unsigned long	yyStateStackSize= 0;

static	yytusChar	yyInitChBuffer [] = {
   '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
   '\0', '\0', '\0', '\0', '\0', '\0', '\0', yyEolCh, yyEobCh, '\0', };
static	yytusChar *	yyChBufferPtr	= yyInitChBuffer;
static	unsigned long	yyChBufferSize	= 0;
static	yytusChar *	yyChBufferStart	= & yyInitChBuffer [16];
static	yytusChar *	yyChBufferIndex	= & yyInitChBuffer [16];

# if yyInitFileStackSize != 0
typedef	struct {
	int		yySourceFile	;
	rbool		yyEof		;
	yytusChar *	yyChBufferPtr	;
	yytusChar *	yyChBufferStart	;
	unsigned long	yyChBufferSize	;
	yytusChar *	yyChBufferIndex	;
	long		yyBytesRead	;
	long		yyFileOffset	;
	unsigned int	yyLineCount	;
	yytusChar *	yyLineStart	;
	}		yytFileStack	;

static	yytFileStack *	yyFileStack	;
static	unsigned long	yyFileStackSize	= 0;
static	yytFileStack *	yyFileStackPtr	;
# endif

int expr_scan_GetToken ARGS ((void))
{
   register	yyStateRange	yyState;
   register	yyStateRange *	yyStatePtr;
   register	yytusChar *	yyChBufferIndexReg;
   register	yyCombType * *	yyBasePtrReg = yyBasePtr;

yyBegin:
   yyState		= yyStartState;		/* initialize */
   yyStatePtr		= & yyStateStack [1];
   yyChBufferIndexReg	= yyChBufferIndex;
   expr_scan_TokenPtr		= (yytChar *) yyChBufferIndexReg;
   if (yyChBufferIndexReg [-1] == yyEolCh) yyState ++;

   /* ASSERT yyChBuffer [yyChBufferIndex] == first character */

yyContinue:		/* continue after sentinel or skipping blanks */
   for (;;) {		/* execute as many state transitions as possible */
			/* determine next state and get next character */
      register yyCombType * yyTablePtr =
		      yyBasePtrReg [yyState] + yyToClass (* yyChBufferIndexReg);
      if (yyTablePtr->yyCheck == yyState) {
	 yyState = yyTablePtr->yyNext;		/* determine next state */
	 * yyStatePtr ++ = yyState;		/* push state */
	 yyChBufferIndexReg ++;			/* get next character */
	 goto yyContinue;
      }
      if ((yyState = yyDefault [yyState]) == yyDNoState) break;
   }

   for (;;) {				/* search for last final state */
      expr_scan_TokenLength =
	    (int) (yyChBufferIndexReg - (yytusChar *) expr_scan_TokenPtr);
      yyChBufferIndex = yyChBufferIndexReg;
switch (* -- yyStatePtr) {
case 3:;
yySetPosition
/* line 37 "expr.rex" */
{expr_scan_Attribute.identifier.Value =
			(char*) malloc (expr_scan_TokenLength + 1);
	 expr_scan_GetWord (expr_scan_Attribute.identifier.Value);
	 return tok_identifier;
	
/* line 501 "expr_scan.c" */
} goto yyBegin;
case 15:;
yySetPosition
/* line 43 "expr.rex" */
{return '+';
/* line 507 "expr_scan.c" */
} goto yyBegin;
case 14:;
yySetPosition
/* line 44 "expr.rex" */
{return '*';
/* line 513 "expr_scan.c" */
} goto yyBegin;
case 13:;
yySetPosition
/* line 45 "expr.rex" */
{return '(';
/* line 519 "expr_scan.c" */
} goto yyBegin;
case 12:;
yySetPosition
/* line 46 "expr.rex" */
{return ')';
/* line 525 "expr_scan.c" */
} goto yyBegin;
case 5:;
yySetPosition
/* line 49 "expr.rex" */
{ /* comment up to end of line, nothing to do */
        
/* line 532 "expr_scan.c" */
} goto yyBegin;
case 11:;
{/* BlankAction */
while (* yyChBufferIndexReg ++ == ' ') ;
expr_scan_TokenPtr = (yytChar *) -- yyChBufferIndexReg;
yyState = yyStartState;
yyStatePtr = & yyStateStack [1];
goto yyContinue;
/* line 541 "expr_scan.c" */
} goto yyBegin;
case 10:;
{/* TabAction */
yyTab;
/* line 546 "expr_scan.c" */
} goto yyBegin;
case 9:;
{/* EolAction */
yyEol (0);
/* line 551 "expr_scan.c" */
} goto yyBegin;
case 1:;
case 2:;
case 4:;
case 6:;
	 /* non final states */
	 yyChBufferIndexReg --;			/* return character */
	 break;

case 8:
	 yySetPosition
      expr_scan_TokenLength   = 1;
	 yyChBufferIndex = ++ yyChBufferIndexReg;
	 {
/* line 24 "expr.rex" */

  WritePosition (stderr, expr_scan_Attribute.Position);
  fprintf (stderr, " Illegal character [%c]\n", *expr_scan_TokenPtr);

/* line 571 "expr_scan.c" */
	 }
	 goto yyBegin;

      case yyDNoState:
	 goto yyBegin;

case 7:
	 yyChBufferIndex = -- yyChBufferIndexReg; /* undo last state transit */
	 if (-- expr_scan_TokenLength == 0) {		/* get previous state */
	    yyState = yyStartState;
  	    if (yyChBufferIndexReg [-1] == yyEolCh) yyState ++;
	 } else {
	    yyState = * (yyStatePtr - 1);
	 }

	 if (yyChBufferIndex != & yyChBufferStart [yyBytesRead]) {
					/* end of buffer sentinel in buffer */
	    if ((yyState = yyEobTrans [yyState]) == yyDNoState) continue;
	    yyChBufferIndexReg ++;
	    * yyStatePtr ++ = yyState;		/* push state */
	    goto yyContinue;
	 }
						/* end of buffer reached */
	 if (! yyEof) {
	    register yytChar * yySource;
	    register yytChar * yyTarget;
	    unsigned long yyChBufferFree;

	    if (yyChBufferSize == 0) {
	       yyStateRange * yyOldStateStack = yyStateStack;
	       yyInitialize ();
	       yyStatePtr += yyStateStack - yyOldStateStack;
	       yyChBufferIndexReg = yyChBufferIndex;
	    }
	    yySource = expr_scan_TokenPtr - 1;
	    yyTarget = (yytChar *) & yyChBufferPtr
		[(yyMaxAlign - 1 - expr_scan_TokenLength) & (yyMaxAlign - 1)];
	    yyChBufferFree = Exp2 (Log2 (yyChBufferSize - 4 -
		yyMaxAlign - expr_scan_TokenLength));
		/* copy initial part of token in front of the input buffer */
	    if (yySource > yyTarget) {
	       expr_scan_TokenPtr = yyTarget + 1;
	       do * yyTarget ++ = * yySource ++;
	       while (yySource < (yytChar *) yyChBufferIndexReg);
	       yyLineStart += (yytusChar *) yyTarget - yyChBufferStart -
				yyBytesRead;
	       yyChBufferStart = (yytusChar *) yyTarget;
	    } else {
	       yyChBufferStart = yyChBufferIndexReg;
	    }
	    yyChBufferStart2 = (yytChar *) yyChBufferStart;
						/* extend buffer if necessary */
	    if (yyChBufferFree < yyChBufferSize >> 3 /* / 8 */ ) {
	       register ptrdiff_t yyDelta;
	       register yytusChar * yyOldChBufferPtr = yyChBufferPtr;
	       ExtendArray ((char * *) & yyChBufferPtr, & yyChBufferSize,
				(unsigned long) sizeof (yytChar));
	       if (yyChBufferPtr == NULL) yyErrorMessage (xxScannerOutOfMemory);
	       yyDelta = yyChBufferPtr - yyOldChBufferPtr;
	       yyChBufferStart	+= yyDelta;
	       yyLineStart	+= yyDelta;
	       expr_scan_TokenPtr	+= yyDelta;
	       yyChBufferStart2	 = (yytChar *) yyChBufferStart;
	       yyChBufferFree = Exp2 (Log2 (yyChBufferSize - 4 -
			yyMaxAlign - expr_scan_TokenLength));
	       if (yyStateStackSize < yyChBufferSize) {
		  yyStateRange * yyOldStateStack = yyStateStack;
		  ExtendArray ((char * *) & yyStateStack, & yyStateStackSize,
				   (unsigned long) sizeof (yyStateRange));
		  if (yyStateStack == NULL)
		     yyErrorMessage (xxScannerOutOfMemory);
		  yyStatePtr	+= yyStateStack - yyOldStateStack;
	       }
	    }
						/* read buffer and restart */
	    yyChBufferIndex = yyChBufferIndexReg = yyChBufferStart;
	    yyFileOffset += yyBytesRead;
	    yyBytesRead = yyGetLine (yySourceFile, (yytChar *) yyChBufferIndex,
	       (int) yyChBufferFree);
	    if (yyBytesRead <= 0) { yyBytesRead = 0; yyEof = rtrue; }
	    yyChBufferStart [yyBytesRead    ] = yyEobCh;
	    yyChBufferStart [yyBytesRead + 1] = '\0';
	    goto yyContinue;
	 }

	 if (expr_scan_TokenLength == 0) {		/* end of file reached */
	    if (yyChBufferSize == 0) return expr_scan_EofToken;
	    yySetPosition
	    expr_scan_CloseFile ();
# if yyInitFileStackSize != 0
	    if (yyFileStackPtr == yyFileStack) {
	    }
	    if (yyFileStackPtr == yyFileStack) {
	       expr_scan_ResetScanner ();
	       return expr_scan_EofToken;
	    }
	    goto yyBegin;
# else
	    expr_scan_ResetScanner ();
	    return expr_scan_EofToken;
# endif
	 }
	 break;

      default:
	 yyErrorMessage (xxScannerInternalError);
      }
   }
}

static void yyInitialize ARGS ((void))
   {
      if (yyStateStackSize == 0) {
	 yyStateStackSize = yyInitBufferSize;
	 MakeArray ((char * *) & yyStateStack, & yyStateStackSize,
		       (unsigned long) sizeof (yyStateRange));
	 if (yyStateStack == NULL) yyErrorMessage (xxScannerOutOfMemory);
	 yyStateStack [0] = yyDefaultState;
      }
# if yyInitFileStackSize != 0
      if (yyFileStackSize == 0) {
	 yyFileStackSize = yyInitFileStackSize;
	 MakeArray ((char * *) & yyFileStack, & yyFileStackSize,
			(unsigned long) sizeof (yytFileStack));
	 if (yyFileStack == NULL) yyErrorMessage (xxScannerOutOfMemory);
	 yyFileStackPtr = yyFileStack;
      }

      if (yyFileStackPtr >= yyFileStack + yyFileStackSize - 1) {
	 ptrdiff_t yyyFileStackPtr = yyFileStackPtr - yyFileStack;
	 ExtendArray ((char * *) & yyFileStack, & yyFileStackSize,
			   (unsigned long) sizeof (yytFileStack));
	 if (yyFileStack == NULL) yyErrorMessage (xxScannerOutOfMemory);
	 yyFileStackPtr = yyFileStack + yyyFileStackPtr;
      }
      yyFileStackPtr ++;			/* push file */
      yyFileStackPtr->yySourceFile	= yySourceFile		;
      yyFileStackPtr->yyEof		= yyEof			;
      yyFileStackPtr->yyChBufferPtr	= yyChBufferPtr		;
      yyFileStackPtr->yyChBufferStart	= yyChBufferStart	;
      yyFileStackPtr->yyChBufferSize	= yyChBufferSize	;
      yyFileStackPtr->yyChBufferIndex	= yyChBufferIndex	;
      yyFileStackPtr->yyBytesRead	= yyBytesRead		;
      yyFileStackPtr->yyFileOffset	= yyFileOffset		;
      yyFileStackPtr->yyLineCount	= yyLineCount		;
      yyFileStackPtr->yyLineStart	= yyLineStart		;
# endif
						/* initialize file state */
      yyChBufferSize	   = yyInitBufferSize;
      MakeArray ((char * *) & yyChBufferPtr, & yyChBufferSize,
			(unsigned long) sizeof (yytChar));
      if (yyChBufferPtr == NULL) yyErrorMessage (xxScannerOutOfMemory);
      yyChBufferStart	   = & yyChBufferPtr [yyMaxAlign];
      yyChBufferStart2	   = (yytChar *) yyChBufferStart;
      yyChBufferStart [-1] = yyEolCh;		/* begin of line indicator */
      yyChBufferStart [ 0] = yyEobCh;		/* end of buffer sentinel */
      yyChBufferStart [ 1] = '\0';
      yyChBufferIndex	   = yyChBufferStart;
      expr_scan_TokenPtr	   = (yytChar *) yyChBufferStart;
      yyEof		   = rfalse;
      yyBytesRead	   = 0;
      yyFileOffset	   = 0;
      yyLineCount	   = 1;
      yyLineStart	   = & yyChBufferStart [-1];
# ifdef HAVE_FILE_NAME
      if (expr_scan_Attribute.Position.FileName == 0)
	 expr_scan_Attribute.Position.FileName = 1; /* NoIdent */
# endif
   }

void expr_scan_BeginFile
# ifdef HAVE_ARGS
   (char * yyFileName)
# else
   (yyFileName) char * yyFileName;
# endif
   {
      yyInitialize ();
      yySourceFile = yyFileName == NULL ? StdIn :
# ifdef SOURCE_VER
	 expr_scan_BeginSourceFile (yyFileName);
# else
	 expr_scan_BeginSource (yyFileName);
# endif
      if (yySourceFile < 0) yyErrorMessage (xxCannotOpenInputFile);
   }

# ifdef SOURCE_VER

# if HAVE_WCHAR_T

void expr_scan_BeginFileW
# ifdef HAVE_ARGS
   (wchar_t * yyFileName)
# else
   (yyFileName) wchar_t * yyFileName;
# endif
   {
      yyInitialize ();
      yySourceFile = yyFileName == NULL ? StdIn :
	 expr_scan_BeginSourceFileW (yyFileName);
      if (yySourceFile < 0) yyErrorMessage (xxCannotOpenInputFile);
   }

# endif

void expr_scan_BeginMemory
# ifdef HAVE_ARGS
   (void * yyInputPtr)
# else
   (yyInputPtr) void * yyInputPtr;
# endif
   {
      yyInitialize ();
      expr_scan_BeginSourceMemory (yyInputPtr);
   }

void expr_scan_BeginMemoryN
# ifdef HAVE_ARGS
   (void * yyInputPtr, int yyLength)
# else
   (yyInputPtr, yyLength) void * yyInputPtr; int yyLength;
# endif
   {
      yyInitialize ();
      expr_scan_BeginSourceMemoryN (yyInputPtr, yyLength);
   }

void expr_scan_BeginGeneric
# ifdef HAVE_ARGS
   (void * yyInputPtr)
# else
   (yyInputPtr) void * yyInputPtr;
# endif
   {
      yyInitialize ();
      expr_scan_BeginSourceGeneric (yyInputPtr);
   }

# endif

void expr_scan_CloseFile ARGS ((void))
   {
# if yyInitFileStackSize != 0
      if (yyFileStackPtr == yyFileStack) yyErrorMessage (xxFileStackUnderflow);
# endif
      expr_scan_CloseSource (yySourceFile);
      ReleaseArray ((char * *) & yyChBufferPtr, & yyChBufferSize,
			(unsigned long) sizeof (yytChar));
# if yyInitFileStackSize != 0
						/* pop file */
      yySourceFile	= yyFileStackPtr->yySourceFile		;
      yyEof		= yyFileStackPtr->yyEof			;
      yyChBufferPtr	= yyFileStackPtr->yyChBufferPtr		;
      yyChBufferStart	= yyFileStackPtr->yyChBufferStart	;
      yyChBufferStart2	= (yytChar *) yyChBufferStart		;
      yyChBufferSize	= yyFileStackPtr->yyChBufferSize	;
      yyChBufferIndex	= yyFileStackPtr->yyChBufferIndex	;
      yyBytesRead	= yyFileStackPtr->yyBytesRead		;
      yyFileOffset	= yyFileStackPtr->yyFileOffset		;
      yyLineCount	= yyFileStackPtr->yyLineCount		;
      yyLineStart	= yyFileStackPtr->yyLineStart		;
      yyFileStackPtr --;
# endif
   }

# ifdef xxGetWord
int expr_scan_GetWord
# ifdef HAVE_ARGS
   (yytChar * yyWord)
# else
   (yyWord) yytChar * yyWord;
# endif
   {
      register yytChar * yySource		= expr_scan_TokenPtr;
      register yytChar * yyTarget		= yyWord;
      register yytChar * yyChBufferIndexReg	= (yytChar *) yyChBufferIndex;

      while (yySource < yyChBufferIndexReg)
	 * yyTarget ++ = * yySource ++;
      * yyTarget = '\0';
      return (int) (yyChBufferIndexReg - expr_scan_TokenPtr);
   }
# endif

# ifdef xxGetLower
int expr_scan_GetLower
# ifdef HAVE_ARGS
   (yytChar * yyWord)
# else
   (yyWord) yytChar * yyWord;
# endif
   {
      register yytusChar * yySource	= (yytusChar *) expr_scan_TokenPtr;
      register yytusChar * yyTarget	= (yytusChar *) yyWord;
      register yytusChar * yyChBufferIndexReg = yyChBufferIndex;

      while (yySource < yyChBufferIndexReg)
	 * yyTarget ++ = yyToLower (* yySource ++);
      * yyTarget = '\0';
      return (int) (yyChBufferIndexReg - (yytusChar *) expr_scan_TokenPtr);
   }
# endif

# ifdef xxGetUpper
int expr_scan_GetUpper
# ifdef HAVE_ARGS
   (yytChar * yyWord)
# else
   (yyWord) yytChar * yyWord;
# endif
   {
      register yytusChar * yySource	= (yytusChar *) expr_scan_TokenPtr;
      register yytusChar * yyTarget	= (yytusChar *) yyWord;
      register yytusChar * yyChBufferIndexReg = yyChBufferIndex;

      while (yySource < yyChBufferIndexReg)
	 * yyTarget ++ = yyToUpper (* yySource ++);
      * yyTarget = '\0';
      return (int) (yyChBufferIndexReg - (yytusChar *) expr_scan_TokenPtr);
   }
# endif

# ifdef xxinput
static yytChar input ARGS ((void))
   {
      if (yyChBufferIndex == & yyChBufferStart [yyBytesRead]) {
	 if (! yyEof) {
	    yyLineStart -= yyBytesRead;
	    yyChBufferIndex = yyChBufferStart = & yyChBufferPtr [yyMaxAlign];
	    yyChBufferStart2 = (yytChar *) yyChBufferStart;
	    yyFileOffset += yyBytesRead;
	    yyBytesRead = yyGetLine (yySourceFile, (yytChar *) yyChBufferIndex,
	       (int) Exp2 (Log2 (yyChBufferSize)));
	    if (yyBytesRead <= 0) { yyBytesRead = 0; yyEof = rtrue; }
	    yyChBufferStart [yyBytesRead    ] = yyEobCh;
	    yyChBufferStart [yyBytesRead + 1] = '\0';
	 }
      }
      if (yyChBufferIndex == & yyChBufferStart [yyBytesRead]) return '\0';
      else return * yyChBufferIndex ++;
   }
# endif

static void unput
# ifdef HAVE_ARGS
   (yytChar yyc)
# else
   (yyc) yytChar yyc;
# endif
   { * (-- yyChBufferIndex) = yyc; }

static void yyLess
# ifdef HAVE_ARGS
   (int yyn)
# else
   (yyn) int yyn;
# endif
   { yyChBufferIndex -= expr_scan_TokenLength - yyn; expr_scan_TokenLength = yyn; }

void expr_scan_BeginScanner ARGS ((void))
   {
   }

void expr_scan_CloseScanner ARGS ((void))
   {
   }

void expr_scan_ResetScanner ARGS ((void))
   {
      yyChBufferPtr	= yyInitChBuffer;
      yyChBufferSize	= 0;
      yyChBufferStart	= & yyInitChBuffer [16];
      yyChBufferIndex	= & yyInitChBuffer [16];
      if (yyStateStackSize != 0)
	 ReleaseArray ((char * *) & yyStateStack, & yyStateStackSize,
			(unsigned long) sizeof (yyStateRange));
      yyStateStack	= yyInitStateStack;
      yyStateStackSize	= 0;
# if yyInitFileStackSize != 0
      if (yyFileStackSize != 0)
	 ReleaseArray ((char * *) & yyFileStack, & yyFileStackSize,
			(unsigned long) sizeof (yytFileStack));
      yyFileStackSize	= 0;
# endif
# if defined xxyyPush | defined xxyyPop
      if (yyStStStackSize != 0)
	 ReleaseArray ((char * *) & yyStStStackPtr, & yyStStStackSize,
			(unsigned long) sizeof (yyStateRange));
      yyStStStackSize	= 0;
      yyStStStackIdx	= 0;
# endif
      yyStartState	= STD;
      yyPreviousStart	= STD;
      yySourceFile	= StdIn;
   }

static void yyErrorMessage
# ifdef HAVE_ARGS
   (int yyErrorCode)
# else
   (yyErrorCode) int yyErrorCode;
# endif
   {
      ErrorMessageI (yyErrorCode, xxFatal, expr_scan_Attribute.Position,
	 xxString, "expr_scan");
      expr_scan_Exit ();
   }
