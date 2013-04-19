/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
/* Line 349 of yacc.c  */
#line 27 "osc_expr_parser.y"


#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef WIN_VERSION
#include <Carbon.h>
#include <CoreServices.h>
#endif
#include "osc_expr.h"
#include "osc_expr_rec.h"
#include "osc_expr_rec.r"
#include "osc_expr_func.h"
#include "osc_error.h"
#include "osc_expr_parser.h"
#include "osc_expr_scanner.h"
#include "osc_util.h"
#include "osc_atom_u.r"

	//#define OSC_EXPR_PARSER_DEBUG
#ifdef OSC_EXPR_PARSER_DEBUG
#define PP(fmt, ...)printf(fmt, ##__VA_ARGS__)
#else
#define PP(fmt, ...)
#endif




/* Line 349 of yacc.c  */
#line 98 "osc_expr_parser.c"

/* Substitute the variable and function names.  */
#define yyparse         osc_expr_parser_parse
#define yylex           osc_expr_parser_lex
#define yyerror         osc_expr_parser_error
#define yylval          osc_expr_parser_lval
#define yychar          osc_expr_parser_char
#define yydebug         osc_expr_parser_debug
#define yynerrs         osc_expr_parser_nerrs
#define yylloc          osc_expr_parser_lloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 73 "osc_expr_parser.y"


// this is a dummy so that the compiler won't complain.  we pass the hard-coded
// value of 1 to osc_expr_scanner_lex() inside of osc_expr_parser_lex() down below.
int alloc_atom = 1;


int osc_expr_parser_lex(YYSTYPE *yylval_param, YYLTYPE *llocp, yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started){
	return osc_expr_scanner_lex(yylval_param, llocp, yyscanner, 1, buflen, buf, startcond, started);
}

static t_osc_atom_ar_u *osc_expr_parser_foldConstants_impl(t_osc_expr *expr, t_osc_expr_lexenv *lexenv)
{
	printf("%s: %s\n", __func__, osc_expr_rec_getName(osc_expr_getRec(expr)));
	t_osc_expr *e = expr;
	while(e){
		t_osc_expr_arg *a = osc_expr_getArgs(e);
		int eval = 1;
		while(a){
			int type = osc_expr_arg_getType(a);
			switch(type){
			case OSC_EXPR_ARG_TYPE_OSCADDRESS:
				// we can't eval this expression, but we want to continue to see if we can 
				// reduce any of the other args
				printf("address\n");
				eval = 0;
				break;
			case OSC_EXPR_ARG_TYPE_EXPR:
				printf("expr\n");
				{
					t_osc_expr *ee = osc_expr_arg_getExpr(a);
					t_osc_atom_ar_u *ar = osc_expr_parser_foldConstants_impl(ee,
												 lexenv);
					if(ar){
						osc_expr_free(ee);
						printf("reduced\n");
						if(osc_atom_array_u_getLen(ar) == 1){
							osc_expr_arg_setOSCAtom(a, osc_atom_array_u_get(ar, 0));
						}else{
							osc_expr_arg_setList(a, ar);
						}
					}
				}
			default:
				printf("default\n");
			}
			a = osc_expr_arg_next(a);
		}
		if(eval > 0){
			printf("eval = %d\n", eval);
			t_osc_atom_ar_u *res = NULL;
			int ret = osc_expr_eval(e, NULL, NULL, &res);
			if(ret){
				return NULL;
			}else{
				return res;
			}
		}else{
			return NULL;
		}
		e = osc_expr_next(e);
	}
}

static void osc_expr_parser_foldConstants(t_osc_expr *expr)
{
	osc_expr_parser_foldConstants_impl(expr, NULL);
}

t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f)
{
	//printf("parsing %s\n", ptr);
	int len = strlen(ptr);
	int alloc = 0;

	// expressions really have to end with a semicolon, but it's nice to write single
	// expressions without one (or to leave it off the last one), so we add one to the
	// end of the string here just in case.
	if(ptr[len - 1] != ';'){
		char *tmp = osc_mem_alloc(len + 2);
		memcpy(tmp, ptr, len + 1);
		tmp[len] = ';';
		tmp[len + 1] = '\0';
		ptr = tmp;
		alloc = 1;
	}

	yyscan_t scanner;
	osc_expr_scanner_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
	osc_expr_scanner_set_out(NULL, scanner);
	t_osc_expr *exprstack = NULL;
	t_osc_expr *tmp_exprstack = NULL;
	long buflen = 0;
	char *buf = NULL;
	int startcond = START_EXPNS;
	int started = 0;
	t_osc_err ret = osc_expr_parser_parse(&exprstack, &tmp_exprstack, NULL, scanner, ptr, &buflen, &buf, startcond, &started);
	osc_expr_scanner__delete_buffer(buf_state, scanner);
	osc_expr_scanner_lex_destroy(scanner);
	if(tmp_exprstack){
		if(exprstack){
			osc_expr_appendExpr(exprstack, tmp_exprstack);
		}else{
			exprstack = tmp_exprstack;
		}
	}
	//osc_expr_parser_foldConstants(exprstack);
	*f = exprstack;
	if(alloc){
		osc_mem_free(ptr);
	}
	return ret;
}

t_osc_err osc_expr_parser_parseFunction(char *ptr, t_osc_expr_rec **f)
{
	yyscan_t scanner;
	osc_expr_scanner_lex_init(&scanner);
	YY_BUFFER_STATE buf_state = osc_expr_scanner__scan_string(ptr, scanner);
	osc_expr_scanner_set_out(NULL, scanner);
	t_osc_expr *exprstack = NULL;
	t_osc_expr *tmp_exprstack = NULL;
	long buflen = 0;
	char *buf = NULL;
	int startcond = START_FUNCTION;
	int started = 0;
	t_osc_err ret = osc_expr_parser_parse(&exprstack, &tmp_exprstack, f, scanner, ptr, &buflen, &buf, startcond, &started);

	osc_expr_scanner__delete_buffer(buf_state, scanner);
	osc_expr_scanner_lex_destroy(scanner);
	return ret;
}

/*
t_osc_err osc_expr_parser_parseString(char *ptr, t_osc_expr **f)
{
	return osc_expr_parser_parseExpr(ptr, f);
}
*/

void osc_expr_error_formatLocation(YYLTYPE *llocp, char *input_string, char **buf)
{
	int len = strlen(input_string);
	if(llocp->first_column >= len || llocp->last_column >= len){
		*buf = osc_mem_alloc(len + 1);
		strncpy(*buf, input_string, len + 1);
		return;
	}
	char s1[len * 2];
	char s2[len * 2];
	char s3[len * 2];
	memcpy(s1, input_string, llocp->first_column);
	s1[llocp->first_column] = '\0';
	memcpy(s2, input_string + llocp->first_column, llocp->last_column - llocp->first_column);
	s2[llocp->last_column - llocp->first_column] = '\0';
	memcpy(s3, input_string + llocp->last_column, len - llocp->last_column);
	s3[len - llocp->last_column] = '\0';
	*buf = osc_mem_alloc(len * 3 + 24); // way too much
	sprintf(*buf, "%s\n-->                %s\n%s\n", s1, s2, s3);
}

void osc_expr_error(YYLTYPE *llocp,
		    char *input_string,
		    t_osc_err errorcode,
		    const char * const moreinfo_fmt,
		    ...)
{
	char *loc = NULL;
	osc_expr_error_formatLocation(llocp, input_string, &loc);
	int loclen = 0;
	if(loc){
		loclen = strlen(loc);
	}
	va_list ap;
	va_start(ap, moreinfo_fmt);
	char more[256];
	memset(more, '\0', sizeof(more));
	int more_len = 0;
	if(ap){
		more_len += vsnprintf(more, 256, moreinfo_fmt, ap);
		va_end(ap);
	}
	if(loclen || more_len){
		char buf[loclen + more_len];
		char *ptr = buf;
		if(loclen){
			ptr += sprintf(ptr, "%s\n", loc);
		}
		if(more_len){
			ptr += sprintf(ptr, "%s\n", more);
		}
		osc_error_handler(basename(__FILE__),
				  NULL,
				  -1,
				  errorcode,
				  buf);
	}else{
		osc_error_handler(basename(__FILE__),
				  NULL,
				  -1,
				  errorcode,
				  "");
	}
	if(loc){
		osc_mem_free(loc);
	}
}

int osc_expr_parser_checkArity(YYLTYPE *llocp, char *input_string, t_osc_expr_rec *r, t_osc_expr_arg *arglist)
{
	if(!r){
		return 1;
	}
	/*
	if(r->arity < 0){
		// variable number of arguments
		return 0;
	}
	*/
	int i = 0;
	t_osc_expr_arg *a = arglist;
	while(a){
		i++;
		a = osc_expr_arg_next(a);
	}
	if(i == r->num_required_args){
		return 0;
	}
	if(i < r->num_required_args){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "expected %d %s for function %s but found %d.",
			       r->num_required_args,
			       r->num_required_args == 1 ? "argument" : "arguments",
			       r->name,
			       i);
		return 1;
	}

	// i is more than the num of required args.
 	if(r->num_optional_args < 0){
		return 0;
	}
	if(r->num_optional_args == 0 || (i - r->num_required_args) > r->num_optional_args){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "expected %d %s for function %s but found %d.",
			       r->num_required_args + r->num_optional_args,
			       (r->num_required_args + r->num_optional_args) == 1 ? "argument" : "arguments",
			       r->name,
			       i);
		return 1;
	}
	return 0;
}

 void yyerror(YYLTYPE *llocp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started, char const *e)
{
	osc_expr_error(llocp, input_string, OSC_ERR_EXPPARSE, e);
}

void osc_expr_parser_reportUnknownFunctionError(YYLTYPE *llocp,
						 char *input_string,
						 char *function_name)
{
	osc_expr_error(llocp,
		       input_string,
		       OSC_ERR_EXPPARSE,
		       "unknown function \"%s\"",
		       function_name);
}

void osc_expr_parser_reportInvalidLvalError(YYLTYPE *llocp,
					    char *input_string,
					    char *lvalue)
{
	osc_expr_error(llocp,
		       input_string,
		       OSC_ERR_EXPPARSE,
		       "\"%s\" is not a valid target for assignment (invalid lvalue)\n",
		       lvalue);
}

t_osc_expr *osc_expr_parser_reduce_PrefixFunction(YYLTYPE *llocp,
						  char *input_string,
						  char *function_name,
						  t_osc_expr_arg *arglist);

t_osc_expr *osc_expr_parser_reduce_InfixOperator(YYLTYPE *llocp,
						char *input_string,
						char *function_name,
						t_osc_expr_arg *left,
						t_osc_expr_arg *right)
{
	t_osc_expr_rec *r = osc_expr_lookupFunction(function_name);
	if(!r){
		osc_expr_parser_reportUnknownFunctionError(llocp, input_string, function_name);
		return NULL;
	}
	t_osc_expr *e = osc_expr_alloc();
	osc_expr_setRec(e, r);
	osc_expr_prependArg(e, right);
	osc_expr_prependArg(e, left);
	return e;
}

t_osc_expr *osc_expr_parser_reduce_InfixAssignmentOperator(YYLTYPE *llocp,
							  char *input_string,
							  char *function_name,
							  t_osc_expr_arg *left,
							  t_osc_expr_arg *right)
{
	t_osc_expr *infix = osc_expr_parser_reduce_InfixOperator(llocp, input_string, function_name, left, right);
	t_osc_expr_arg *assign_target = NULL;
	osc_expr_arg_copy(&assign_target, left);
	t_osc_expr_arg *assign_arg = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(assign_arg, infix);
	osc_expr_arg_setNext(assign_target, assign_arg);
	t_osc_expr *assign = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "assign", assign_target);
	return assign;
}

t_osc_expr *osc_expr_parser_reduce_PrefixFunction(YYLTYPE *llocp,
						 char *input_string,
						 char *function_name,
						 t_osc_expr_arg *arglist)
{
	t_osc_expr_rec *r = osc_expr_lookupFunction(function_name);
	if(!r){
		osc_expr_parser_reportUnknownFunctionError(llocp, input_string, function_name);
		return NULL;
	}
	if(osc_expr_parser_checkArity(llocp, input_string, r, arglist)){
		return NULL;
	}
	t_osc_expr *e = osc_expr_alloc();
	osc_expr_setRec(e, r);
	if(arglist){
		osc_expr_setArg(e, arglist);
	}
	return e;
}

t_osc_expr *osc_expr_parser_reduce_PrefixUnaryOperator(YYLTYPE *llocp,
						      char *input_string,
						      char *oscaddress,
						      char *op)
{
	char *ptr = oscaddress;
	if(*ptr != '/'){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "\"%s\" is not a valid target for assignment\nexpected \"%s\" in \"%s%s\" to be an OSC address\n",
			       oscaddress,
			       oscaddress,
			       op,
			       oscaddress);
		return NULL;
	}
	t_osc_expr_arg *arg = osc_expr_arg_alloc();
	osc_expr_arg_setOSCAddress(arg, ptr);
	t_osc_expr *pfu = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, op, arg);

	t_osc_expr_arg *assign_target = NULL;
	osc_expr_arg_copy(&assign_target, arg);
	t_osc_expr_arg *assign_arg = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(assign_arg, pfu);
	osc_expr_arg_setNext(assign_target, assign_arg);
	t_osc_expr *assign = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "assign", assign_target);
	return assign;
}

t_osc_expr *osc_expr_parser_reduce_PostfixUnaryOperator(YYLTYPE *llocp,
						       char *input_string,
						       char *oscaddress,
						       char *op)
{
	t_osc_expr *incdec = osc_expr_parser_reduce_PrefixUnaryOperator(llocp, input_string, oscaddress, op);
	if(!incdec){
		return NULL;
	}
	t_osc_expr_arg *arg1 = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(arg1, incdec);
	char *oscaddress_copy = NULL;
	osc_util_strdup(&oscaddress_copy, oscaddress);
	t_osc_expr_arg *arg2 = osc_expr_arg_alloc();
	osc_expr_arg_setOSCAddress(arg2, oscaddress_copy);
	osc_expr_arg_setNext(arg2, arg1);
	t_osc_expr *prog1 = osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "prog1", arg2);
	return prog1;
}

t_osc_expr *osc_expr_parser_reduce_NullCoalescingOperator(YYLTYPE *llocp,
							 char *input_string,
							 t_osc_atom_u *address_to_check,
							 t_osc_expr_arg *arg_if_null)
{
	char *address = NULL;
	osc_atom_u_getString(address_to_check, 0, &address);
	if(*address != '/'){
		osc_expr_error(llocp,
			       input_string,
			       OSC_ERR_EXPPARSE,
			       "\"%s\" is not a valid target for assignment\nexpected \"%s\" to be an OSC address\n",
			       address,
			       address);
		osc_mem_free(address);
		return NULL;
	}
	t_osc_expr *expr_def = osc_expr_alloc();
	osc_expr_setRec(expr_def, osc_expr_lookupFunction("bound"));
	t_osc_expr_arg *def_arg = osc_expr_arg_alloc();

	osc_expr_arg_setOSCAddress(def_arg, address);
	osc_expr_setArg(expr_def, def_arg);
	t_osc_expr_arg *arg1 = osc_expr_arg_alloc();
	osc_expr_arg_setExpr(arg1, expr_def);
	t_osc_expr_arg *arg2 = NULL;
	osc_expr_arg_copy(&arg2, def_arg);
	t_osc_expr_arg *arg3 = arg_if_null;
	osc_expr_arg_setNext(arg1, arg2);
	osc_expr_arg_setNext(arg2, arg3);
	return osc_expr_parser_reduce_PrefixFunction(llocp, input_string, "if", arg1);
}


/* Line 371 of yacc.c  */
#line 544 "osc_expr_parser.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "osc_expr_parser.h".  */
#ifndef YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED
# define YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int osc_expr_parser_debug;
#endif
/* "%code requires" blocks.  */
/* Line 387 of yacc.c  */
#line 58 "osc_expr_parser.y"

#include "osc.h"
#include "osc_mem.h"
#include "osc_atom_u.h"
#include "osc_expr.h"


#define YY_DECL int osc_expr_scanner_lex \
		(YYSTYPE * yylval_param, YYLTYPE * yylloc_param , yyscan_t yyscanner, int alloc_atom, long *buflen, char **buf, int startcond, int *started)
	//t_osc_err osc_expr_parser_parseString(char *ptr, t_osc_expr **f);
t_osc_err osc_expr_parser_parseExpr(char *ptr, t_osc_expr **f);
t_osc_err osc_expr_parser_parseFunction(char *ptr, t_osc_expr_rec **f);



/* Line 387 of yacc.c  */
#line 592 "osc_expr_parser.c"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OSC_EXPR_LAMBDA = 258,
     OSC_EXPR_OSCADDRESS = 259,
     OSC_EXPR_STRING = 260,
     OSC_EXPR_NUM = 261,
     OSC_EXPR_POWEQ = 262,
     OSC_EXPR_MODEQ = 263,
     OSC_EXPR_DIVEQ = 264,
     OSC_EXPR_MULTEQ = 265,
     OSC_EXPR_MINUSEQ = 266,
     OSC_EXPR_PLUSEQ = 267,
     OSC_EXPR_DBLQMARKEQ = 268,
     OSC_EXPR_DBLQMARK = 269,
     OSC_EXPR_TERNARY_COND = 270,
     OSC_EXPR_OR = 271,
     OSC_EXPR_AND = 272,
     OSC_EXPR_NEQ = 273,
     OSC_EXPR_EQ = 274,
     OSC_EXPR_GTE = 275,
     OSC_EXPR_LTE = 276,
     OSC_EXPR_UMINUS = 277,
     OSC_EXPR_UPLUS = 278,
     OSC_EXPR_PREFIX_DEC = 279,
     OSC_EXPR_PREFIX_INC = 280,
     CLOSE_DBL_BRKTS = 281,
     OPEN_DBL_BRKTS = 282,
     OSC_EXPR_QUOTED_EXPR = 283,
     OSC_EXPR_FUNC_CALL = 284,
     OSC_EXPR_DEC = 285,
     OSC_EXPR_INC = 286,
     START_EXPNS = 287,
     START_FUNCTION = 288
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 526 "osc_expr_parser.y"

	t_osc_atom_u *atom;
	t_osc_expr *expr;
	t_osc_expr_rec *func;
	t_osc_expr_arg *arg;


/* Line 387 of yacc.c  */
#line 648 "osc_expr_parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int osc_expr_parser_parse (void *YYPARSE_PARAM);
#else
int osc_expr_parser_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int osc_expr_parser_parse (t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started);
#else
int osc_expr_parser_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_OSC_EXPR_PARSER_OSC_EXPR_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 688 "osc_expr_parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   500

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  9
/* YYNRULES -- Number of rules.  */
#define YYNRULES  63
/* YYNRULES -- Number of states.  */
#define YYNSTATES  141

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   288

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    33,     2,     2,     2,    30,     2,     2,
      48,    49,    31,    27,    47,    28,     2,    29,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    15,    46,
      23,     7,    24,    14,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    32,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,     2,    51,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     8,     9,    10,    11,    12,    13,    16,    17,
      18,    19,    20,    21,    22,    25,    26,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    13,    17,    19,    23,
      25,    27,    29,    31,    33,    41,    43,    47,    49,    53,
      58,    62,    64,    68,    72,    76,    80,    84,    88,    92,
      96,   100,   104,   108,   112,   116,   120,   124,   128,   132,
     136,   140,   144,   147,   150,   153,   156,   159,   163,   170,
     179,   190,   196,   204,   210,   214,   218,   222,   227,   232,
     238,   243,   248,   253
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    44,    56,    -1,    45,    59,    -1,    -1,
      56,    46,    -1,    56,    62,    46,    -1,    58,    -1,    57,
      47,    58,    -1,     6,    -1,     5,    -1,     4,    -1,    62,
      -1,    59,    -1,     3,    48,    60,    49,    50,    56,    51,
      -1,    61,    -1,    60,    47,    61,    -1,     5,    -1,    48,
      62,    49,    -1,     5,    48,    57,    49,    -1,     5,    48,
      49,    -1,    40,    -1,    58,    27,    58,    -1,    58,    28,
      58,    -1,    58,    31,    58,    -1,    58,    29,    58,    -1,
      58,    30,    58,    -1,    58,    32,    58,    -1,    58,    22,
      58,    -1,    58,    21,    58,    -1,    58,    23,    58,    -1,
      58,    26,    58,    -1,    58,    24,    58,    -1,    58,    25,
      58,    -1,    58,    20,    58,    -1,    58,    19,    58,    -1,
      58,    13,    58,    -1,    58,    12,    58,    -1,    58,    11,
      58,    -1,    58,    10,    58,    -1,    58,     9,    58,    -1,
      58,     8,    58,    -1,    33,    58,    -1,    43,     4,    -1,
      42,     4,    -1,     4,    43,    -1,     4,    42,    -1,     4,
       7,    58,    -1,     4,    39,    57,    38,     7,    58,    -1,
       4,    39,    58,    15,    58,    38,     7,    58,    -1,     4,
      39,    58,    15,    58,    15,    58,    38,     7,    58,    -1,
      52,    58,    15,    58,    53,    -1,    52,    58,    15,    58,
      15,    58,    53,    -1,    58,    14,    58,    15,    58,    -1,
       4,    17,    58,    -1,     4,    16,    58,    -1,    52,    57,
      53,    -1,    58,    39,    57,    38,    -1,     4,    39,    57,
      38,    -1,    48,    59,    47,    57,    49,    -1,     5,    43,
       7,    58,    -1,     5,    42,     7,    58,    -1,    43,     5,
       7,    58,    -1,    42,     5,     7,    58,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   579,   579,   580,   582,   592,   593,   602,   603,   608,
     612,   616,   625,   656,   663,   729,   730,   736,   756,   760,
     772,   781,   787,   790,   793,   796,   799,   802,   805,   808,
     811,   814,   817,   820,   823,   826,   829,   832,   835,   838,
     841,   844,   849,   855,   867,   880,   892,   905,   918,   941,
     962,   999,  1004,  1010,  1016,  1022,  1038,  1042,  1046,  1056,
    1063,  1069,  1075,  1081
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OSC_EXPR_LAMBDA", "OSC_EXPR_OSCADDRESS",
  "OSC_EXPR_STRING", "OSC_EXPR_NUM", "'='", "OSC_EXPR_POWEQ",
  "OSC_EXPR_MODEQ", "OSC_EXPR_DIVEQ", "OSC_EXPR_MULTEQ",
  "OSC_EXPR_MINUSEQ", "OSC_EXPR_PLUSEQ", "'?'", "':'",
  "OSC_EXPR_DBLQMARKEQ", "OSC_EXPR_DBLQMARK", "OSC_EXPR_TERNARY_COND",
  "OSC_EXPR_OR", "OSC_EXPR_AND", "OSC_EXPR_NEQ", "OSC_EXPR_EQ", "'<'",
  "'>'", "OSC_EXPR_GTE", "OSC_EXPR_LTE", "'+'", "'-'", "'/'", "'%'", "'*'",
  "'^'", "'!'", "OSC_EXPR_UMINUS", "OSC_EXPR_UPLUS", "OSC_EXPR_PREFIX_DEC",
  "OSC_EXPR_PREFIX_INC", "CLOSE_DBL_BRKTS", "OPEN_DBL_BRKTS",
  "OSC_EXPR_QUOTED_EXPR", "OSC_EXPR_FUNC_CALL", "OSC_EXPR_DEC",
  "OSC_EXPR_INC", "START_EXPNS", "START_FUNCTION", "';'", "','", "'('",
  "')'", "'{'", "'}'", "'['", "']'", "$accept", "start", "expns", "args",
  "arg", "function", "parameters", "parameter", "expr", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,    61,   262,   263,
     264,   265,   266,   267,    63,    58,   268,   269,   270,   271,
     272,   273,   274,    60,    62,   275,   276,    43,    45,    47,
      37,    42,    94,    33,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,    59,    44,    40,    41,
     123,   125,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    55,    56,    56,    56,    57,    57,    58,
      58,    58,    58,    58,    59,    60,    60,    61,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     2,     3,     1,     3,     1,
       1,     1,     1,     1,     7,     1,     3,     1,     3,     4,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     2,     3,     6,     8,
      10,     5,     7,     5,     3,     3,     3,     4,     4,     5,
       4,     4,     4,     4
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     4,     0,     0,     2,     0,     3,     1,    11,    10,
       9,     0,    21,     0,     0,     5,     0,     0,     0,    13,
      12,     0,     0,     0,     0,     0,    46,    45,     0,     0,
       0,    42,    12,    44,     0,    43,     0,    13,    12,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     6,    17,     0,    15,    47,    55,    54,
       0,     7,     0,     0,    20,     0,     7,     0,     0,     0,
      18,     0,    56,     0,    41,    40,    39,    38,    37,    36,
       0,    35,    34,    29,    28,    30,    32,    33,    31,    22,
      23,    25,    26,    24,    27,     0,     0,     0,    58,     0,
      61,    60,    19,    63,    62,     0,     8,     0,     0,    57,
      16,     4,     0,     0,    59,     0,    51,    53,     0,    48,
       0,     0,     0,    14,     0,     0,    52,     0,    49,     0,
      50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,    39,    18,    19,    65,    66,    32
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -43
static const yytype_int16 yypact[] =
{
      64,   -43,     1,     8,    70,   -27,   -43,   -43,    48,   -19,
     -43,   137,   -43,   116,   121,   -43,   137,   137,   380,   -43,
      38,    20,   137,   137,   137,   137,   -43,   -43,    81,    85,
     126,    55,   -43,   -43,    90,   -43,    93,    59,    66,    16,
     284,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   -43,   -43,   -42,   -43,   380,   401,   401,
     -37,   316,   137,   137,   -43,   -21,   380,   137,   137,   137,
     -43,   137,   -43,   137,   380,   380,   380,   380,   380,   380,
     348,   421,   440,   457,   457,    50,    50,    50,    50,   461,
     461,    21,    21,   -12,    55,   -29,    20,    52,   110,   137,
     380,   380,   -43,    55,    55,    36,   380,   182,   137,   -43,
     -43,   -43,   137,   252,   -43,   137,   -43,   401,    53,   380,
     137,   151,   217,   -43,   125,   137,   -43,   153,   380,   137,
     380
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -43,   -43,     2,    -8,   -11,     0,   -43,    56,    -1
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      31,   108,     6,    20,     5,   106,    40,   107,     7,   119,
      81,    67,    68,    69,    71,    38,    37,    70,    81,    76,
      61,    21,    75,    28,    29,    64,    81,    62,   112,    30,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,    76,    60,    61,   105,    22,     5,     8,     9,    10,
      62,   110,   111,    81,    23,    24,   113,   114,    76,    82,
     116,   115,   117,     5,     8,     9,    10,    56,    57,    58,
      59,    60,    61,    81,    63,   124,    11,    25,    72,    62,
      26,    27,    73,    12,    62,    13,    14,    77,   123,    15,
      78,    16,   121,    11,   133,    17,    79,   127,     1,     2,
      12,   129,    13,    14,   132,    80,    15,   122,    16,   134,
      33,    34,    17,   128,   138,    35,    36,    20,   140,     5,
       8,     9,    10,    41,    42,    43,    44,    45,    46,    47,
       5,     8,     9,    10,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,   135,    11,
     139,     0,   120,   137,    62,     0,    12,     0,    13,    14,
      11,     0,     0,     0,    16,    74,     0,    12,    17,    13,
      14,     0,     0,     0,     0,    16,     0,     0,     0,    17,
      41,    42,    43,    44,    45,    46,    47,   125,     0,     0,
       0,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,     0,     0,     0,     0,     0,
       0,    62,     0,     0,     0,    41,    42,    43,    44,    45,
      46,    47,     0,     0,     0,   126,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
       0,     0,     0,     0,     0,     0,    62,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,   130,     0,     0,
     136,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,     0,     0,     0,     0,     0,
     131,    62,    41,    42,    43,    44,    45,    46,    47,    83,
       0,     0,     0,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,     0,     0,     0,
       0,     0,     0,    62,    41,    42,    43,    44,    45,    46,
      47,   109,     0,     0,     0,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,     0,
       0,     0,     0,     0,     0,    62,    41,    42,    43,    44,
      45,    46,    47,   118,     0,     0,     0,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     0,     0,     0,     0,     0,     0,    62,    41,    42,
      43,    44,    45,    46,    47,     0,     0,     0,     0,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,     0,     0,    47,     0,     0,     0,    62,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,     0,     0,     0,     0,     0,     0,
      62,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,     0,     0,     0,     0,     0,     0,
      62,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,     0,     0,     0,     0,     0,     0,    62,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      58,    59,    60,    61,     0,     0,    62,     0,     0,     0,
      62
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-43)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      11,    38,     2,     4,     3,    47,    17,    49,     0,    38,
      47,    22,    23,    24,    25,    16,    16,    25,    47,    30,
      32,    48,    30,    42,    43,     5,    47,    39,    49,    48,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    31,    32,    62,     7,     3,     4,     5,     6,
      39,    72,    73,    47,    16,    17,    77,    78,    79,    53,
      81,    79,    83,     3,     4,     5,     6,    27,    28,    29,
      30,    31,    32,    47,    46,    49,    33,    39,     7,    39,
      42,    43,     7,    40,    39,    42,    43,     7,   109,    46,
       7,    48,    50,    33,    51,    52,    47,   118,    44,    45,
      40,   122,    42,    43,   125,    49,    46,     7,    48,   130,
       4,     5,    52,   121,   135,     4,     5,   128,   139,     3,
       4,     5,     6,     8,     9,    10,    11,    12,    13,    14,
       3,     4,     5,     6,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,     7,    33,
       7,    -1,   106,    38,    39,    -1,    40,    -1,    42,    43,
      33,    -1,    -1,    -1,    48,    49,    -1,    40,    52,    42,
      43,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    52,
       8,     9,    10,    11,    12,    13,    14,    15,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    -1,    -1,    -1,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    53,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    -1,
       8,     9,    10,    11,    12,    13,    14,    15,    -1,    -1,
      53,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,
      38,    39,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    39,     8,     9,    10,    11,    12,    13,
      14,    15,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    39,     8,     9,    10,    11,
      12,    13,    14,    15,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    39,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    14,    -1,    -1,    -1,    39,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      29,    30,    31,    32,    -1,    -1,    39,    -1,    -1,    -1,
      39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    44,    45,    55,    56,     3,    59,     0,     4,     5,
       6,    33,    40,    42,    43,    46,    48,    52,    58,    59,
      62,    48,     7,    16,    17,    39,    42,    43,    42,    43,
      48,    58,    62,     4,     5,     4,     5,    59,    62,    57,
      58,     8,     9,    10,    11,    12,    13,    14,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    39,    46,     5,    60,    61,    58,    58,    58,
      57,    58,     7,     7,    49,    57,    58,     7,     7,    47,
      49,    47,    53,    15,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    57,    47,    49,    38,    15,
      58,    58,    49,    58,    58,    57,    58,    58,    15,    38,
      61,    50,     7,    58,    49,    15,    53,    58,    56,    58,
      15,    38,    58,    51,    58,     7,    53,    38,    58,     7,
      58
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner, alloc_atom, buflen, buf, startcond, started)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (exprstack);
  YYUSE (tmp_exprstack);
  YYUSE (rec);
  YYUSE (scanner);
  YYUSE (input_string);
  YYUSE (buflen);
  YYUSE (buf);
  YYUSE (startcond);
  YYUSE (started);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (exprstack);
  YYUSE (tmp_exprstack);
  YYUSE (rec);
  YYUSE (scanner);
  YYUSE (input_string);
  YYUSE (buflen);
  YYUSE (buf);
  YYUSE (startcond);
  YYUSE (started);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (t_osc_expr **exprstack, t_osc_expr **tmp_exprstack, t_osc_expr_rec **rec, void *scanner, char *input_string, long *buflen, char **buf, int startcond, int *started)
#else
int
yyparse (exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started)
    t_osc_expr **exprstack;
    t_osc_expr **tmp_exprstack;
    t_osc_expr_rec **rec;
    void *scanner;
    char *input_string;
    long *buflen;
    char **buf;
    int startcond;
    int *started;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;


#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
static YYSTYPE yyval_default;
# define YY_INITIAL_VALUE(Value) = Value
#endif
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc = yyloc_default;


    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
/* Line 1807 of yacc.c  */
#line 582 "osc_expr_parser.y"
    {
		if(*tmp_exprstack){
			if(*exprstack){
				osc_expr_appendExpr(*exprstack, *tmp_exprstack);
			}else{
				*exprstack = *tmp_exprstack;
			}
			*tmp_exprstack = NULL;
		}
 	}
    break;

  case 5:
/* Line 1807 of yacc.c  */
#line 592 "osc_expr_parser.y"
    {;}
    break;

  case 6:
/* Line 1807 of yacc.c  */
#line 593 "osc_expr_parser.y"
    {
		if(*tmp_exprstack){
			osc_expr_appendExpr(*tmp_exprstack, (yyvsp[(2) - (3)].expr));
		}else{
			*tmp_exprstack = (yyvsp[(2) - (3)].expr);
		}
 	}
    break;

  case 8:
/* Line 1807 of yacc.c  */
#line 603 "osc_expr_parser.y"
    {
		osc_expr_arg_append((yyval.arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 9:
/* Line 1807 of yacc.c  */
#line 608 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom((yyval.arg), (yyvsp[(1) - (1)].atom));
 	}
    break;

  case 10:
/* Line 1807 of yacc.c  */
#line 612 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom((yyval.arg), (yyvsp[(1) - (1)].atom));
 	}
    break;

  case 11:
/* Line 1807 of yacc.c  */
#line 616 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		char *st = osc_atom_u_getStringPtr((yyvsp[(1) - (1)].atom));
		int len = strlen(st) + 1;
		char *buf = osc_mem_alloc(len);
		memcpy(buf, st, len);
		osc_expr_arg_setOSCAddress((yyval.arg), buf);
		osc_atom_u_free((yyvsp[(1) - (1)].atom));
	}
    break;

  case 12:
/* Line 1807 of yacc.c  */
#line 625 "osc_expr_parser.y"
    {
		t_osc_expr *e = (yyvsp[(1) - (1)].expr);
		t_osc_expr_arg *a = osc_expr_getArgs(e);
		(yyval.arg) = osc_expr_arg_alloc();
		/*
		int eval = 1;
		while(a){
			int type = osc_expr_arg_getType(a);
			if(type == OSC_EXPR_ARG_TYPE_OSCADDRESS ||
			   type == OSC_EXPR_ARG_TYPE_EXPR ||
			   type == OSC_EXPR_ARG_TYPE_FUNCTION){
				eval = 0;
				break;
			}
			a = osc_expr_arg_next(a);
		}
		if(eval){
			t_osc_atom_ar_u *res = NULL;
			int ret = osc_expr_eval(e, NULL, NULL, &res);
			if(ret){
				osc_expr_arg_setExpr($$, e);
			}else{
				// assume that this is a special function like value() or bound() that
				// needs an OSC bundle to return a value
				osc_expr_arg_setList($$, res);
			}
		}else{
		*/
			osc_expr_arg_setExpr((yyval.arg), e);
			//}
  	}
    break;

  case 13:
/* Line 1807 of yacc.c  */
#line 656 "osc_expr_parser.y"
    {
		(yyval.arg) = osc_expr_arg_alloc();
		osc_expr_arg_setFunction((yyval.arg), (yyvsp[(1) - (1)].func));
	}
    break;

  case 14:
/* Line 1807 of yacc.c  */
#line 663 "osc_expr_parser.y"
    {
		int n = 0;
		t_osc_atom_u *a = (yyvsp[(3) - (7)].atom);
		while(a){
			n++;
			a = a->next;
		}
		char *params[n];
		a = (yyvsp[(3) - (7)].atom);
		for(int i = n - 1; i >= 0; i--){
			char *st = osc_atom_u_getStringPtr(a);
			int len = strlen(st) + 1;
			params[i] = (char *)osc_mem_alloc(len);
			strncpy(params[i], st, len);
			t_osc_atom_u *killme = a;
			a = a->next;
			osc_atom_u_free(killme);
		}
		t_osc_expr_rec *func = osc_expr_rec_alloc();
		osc_expr_rec_setName(func, "lambda");
		osc_expr_rec_setRequiredArgs(func, n, params, NULL);
		for(int i = 0; i < n; i++){
			if(params[i]){
				osc_mem_free(params[i]);
			}
		}
		/*
		t_osc_expr *e = *tmp_exprstack;
		while(e){
			e = osc_expr_next(e);
		}
		*/
		osc_expr_rec_setFunction(func, osc_expr_lambda);
		osc_expr_rec_setExtra(func, *tmp_exprstack);
		(yyval.func) = func;
		if(startcond == START_EXPNS){
			*tmp_exprstack = NULL;
		}else if(startcond == START_FUNCTION){
			*rec = func;
		}
// go through and make sure the parameters are unique
/*
		t_osc_expr_rec *r = osc_expr_lookupFunction("lambda");
		t_osc_expr *e = osc_expr_alloc();
		osc_expr_setRec(e, r);
		t_osc_expr *expns_copy = NULL;
		t_osc_expr *ee = $<expr>6;
		while(ee){
			t_osc_expr *copy = osc_expr_copy(ee);
			if(expns_copy){
				osc_expr_appendExpr(expns_copy, copy);
			}else{
				expns_copy = copy;
			}
			ee = osc_expr_next(ee);
		}
		t_osc_expr_arg *expns = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(expns, expns_copy);
		osc_expr_arg_append($3, expns);
		osc_expr_setArg(e, $3);
*/
//osc_expr_parser_bindParameters(&yylloc, input_string, e, $3, expns_copy);
//$$ = e;
	}
    break;

  case 16:
/* Line 1807 of yacc.c  */
#line 730 "osc_expr_parser.y"
    {
		(yyvsp[(3) - (3)].atom)->next = (yyvsp[(1) - (3)].atom);
		(yyval.atom) = (yyvsp[(3) - (3)].atom);
 	}
    break;

  case 17:
/* Line 1807 of yacc.c  */
#line 736 "osc_expr_parser.y"
    {
		if(osc_atom_u_getTypetag((yyvsp[(1) - (1)].atom)) == 's'){
			char *st = osc_atom_u_getStringPtr((yyvsp[(1) - (1)].atom));
			if(st){
				if(*st == '/' && st[1] != '\0'){
					// this is an OSC address
					//error
				}else{
					(yyval.atom) = (yyvsp[(1) - (1)].atom);
				}
			}else{
				//error
			}
		}else{
			//error
		}
	}
    break;

  case 18:
/* Line 1807 of yacc.c  */
#line 756 "osc_expr_parser.y"
    {
		(yyval.expr) = (yyvsp[(2) - (3)].expr);
  	}
    break;

  case 19:
/* Line 1807 of yacc.c  */
#line 760 "osc_expr_parser.y"
    {
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc,
								     input_string,
								     osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom)),
								     (yyvsp[(3) - (4)].arg));
		if(!e){
			osc_atom_u_free((yyvsp[(1) - (4)].atom));
			return 1;
		}
		(yyval.expr) = e;
		osc_atom_u_free((yyvsp[(1) - (4)].atom));
  	}
    break;

  case 20:
/* Line 1807 of yacc.c  */
#line 772 "osc_expr_parser.y"
    {
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, osc_atom_u_getStringPtr((yyvsp[(1) - (3)].atom)), NULL);
		if(!e){
			osc_atom_u_free((yyvsp[(1) - (3)].atom));
			return 1;
		}
		(yyval.expr) = e;
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
  	}
    break;

  case 21:
/* Line 1807 of yacc.c  */
#line 781 "osc_expr_parser.y"
    {
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAtom(arg, (yyvsp[(1) - (1)].atom));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "quote", arg);
	}
    break;

  case 22:
/* Line 1807 of yacc.c  */
#line 787 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "+", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 23:
/* Line 1807 of yacc.c  */
#line 790 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "-", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 24:
/* Line 1807 of yacc.c  */
#line 793 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "*", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 25:
/* Line 1807 of yacc.c  */
#line 796 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "/", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 26:
/* Line 1807 of yacc.c  */
#line 799 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "%", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 27:
/* Line 1807 of yacc.c  */
#line 802 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "^", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 28:
/* Line 1807 of yacc.c  */
#line 805 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "==", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 29:
/* Line 1807 of yacc.c  */
#line 808 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "!=", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 30:
/* Line 1807 of yacc.c  */
#line 811 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "<", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 31:
/* Line 1807 of yacc.c  */
#line 814 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "<=", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 32:
/* Line 1807 of yacc.c  */
#line 817 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, ">", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 33:
/* Line 1807 of yacc.c  */
#line 820 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, ">=", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 34:
/* Line 1807 of yacc.c  */
#line 823 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "&&", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 35:
/* Line 1807 of yacc.c  */
#line 826 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "||", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 36:
/* Line 1807 of yacc.c  */
#line 829 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "+", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 37:
/* Line 1807 of yacc.c  */
#line 832 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "-", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 38:
/* Line 1807 of yacc.c  */
#line 835 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "*", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 39:
/* Line 1807 of yacc.c  */
#line 838 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "/", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 40:
/* Line 1807 of yacc.c  */
#line 841 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "%", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 41:
/* Line 1807 of yacc.c  */
#line 844 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_InfixAssignmentOperator(&yylloc, input_string, "^", (yyvsp[(1) - (3)].arg), (yyvsp[(3) - (3)].arg));
 	}
    break;

  case 42:
/* Line 1807 of yacc.c  */
#line 849 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_alloc();
		osc_expr_setRec((yyval.expr), osc_expr_lookupFunction("!"));
		osc_expr_setArg((yyval.expr), (yyvsp[(2) - (2)].arg));
	}
    break;

  case 43:
/* Line 1807 of yacc.c  */
#line 855 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(2) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PrefixUnaryOperator(&yylloc, input_string, copy, "plus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(2) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(2) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 44:
/* Line 1807 of yacc.c  */
#line 867 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(2) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PrefixUnaryOperator(&yylloc, input_string, copy, "minus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(2) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(2) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 45:
/* Line 1807 of yacc.c  */
#line 880 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(1) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PostfixUnaryOperator(&yylloc, input_string, copy, "plus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(1) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(1) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 46:
/* Line 1807 of yacc.c  */
#line 892 "osc_expr_parser.y"
    {
		char *copy = NULL;
		osc_atom_u_getString((yyvsp[(1) - (2)].atom), 0, &copy);
		t_osc_expr *e = osc_expr_parser_reduce_PostfixUnaryOperator(&yylloc, input_string, copy, "minus1");
		if(!e){
			osc_mem_free(copy);
			osc_atom_u_free((yyvsp[(1) - (2)].atom));
			return 1;
		}
		osc_atom_u_free((yyvsp[(1) - (2)].atom));
		(yyval.expr) = e;
	}
    break;

  case 47:
/* Line 1807 of yacc.c  */
#line 905 "osc_expr_parser.y"
    {
		// basic assignment 
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (3)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, ptr);
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "assign", arg, (yyvsp[(3) - (3)].arg));
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
 	}
    break;

  case 48:
/* Line 1807 of yacc.c  */
#line 918 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (6)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		t_osc_expr_arg *indexes = (yyvsp[(3) - (6)].arg);
		if(osc_expr_arg_next((yyvsp[(3) - (6)].arg))){
			// /foo[[1, 2, 3]] = ...
			t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "list", (yyvsp[(3) - (6)].arg));
			indexes = NULL;
			indexes = osc_expr_arg_alloc();
			osc_expr_arg_setExpr(indexes, e);
		}
		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_append(arg, indexes);
		osc_expr_arg_append(arg, (yyvsp[(6) - (6)].arg));
		//$$ = osc_expr_parser_infix("=", arg, $3);
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assign_to_index", arg);
		osc_atom_u_free((yyvsp[(1) - (6)].atom));
	}
    break;

  case 49:
/* Line 1807 of yacc.c  */
#line 941 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (8)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();

		osc_expr_arg_append((yyvsp[(3) - (8)].arg), (yyvsp[(5) - (8)].arg));
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "aseq", (yyvsp[(3) - (8)].arg));
		t_osc_expr_arg *indexes = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(indexes, e);

		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_append(arg, indexes);
		osc_expr_arg_append(arg, (yyvsp[(8) - (8)].arg));
		//$$ = osc_expr_parser_infix("=", arg, $3);
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assign_to_index", arg);
		osc_atom_u_free((yyvsp[(1) - (8)].atom));
	}
    break;

  case 50:
/* Line 1807 of yacc.c  */
#line 962 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (10)].atom), 0, &ptr);
		if(*ptr != '/'){
			osc_expr_error(&yylloc, input_string, OSC_ERR_EXPPARSE, "osc_expr_parser: expected \"%s\" in \"%s = ... to be an OSC address\n", ptr, ptr);
			return 1;
		}
		t_osc_expr_arg *arg = osc_expr_arg_alloc();

		osc_expr_arg_append((yyvsp[(3) - (10)].arg), (yyvsp[(7) - (10)].arg));
		osc_expr_arg_append((yyvsp[(3) - (10)].arg), (yyvsp[(5) - (10)].arg));
		t_osc_expr *e = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "aseq", (yyvsp[(3) - (10)].arg));
		t_osc_expr_arg *indexes = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(indexes, e);

		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_append(arg, indexes);
		osc_expr_arg_append(arg, (yyvsp[(10) - (10)].arg));
		//$$ = osc_expr_parser_infix("=", arg, $3);
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "assign_to_index", arg);
		osc_atom_u_free((yyvsp[(1) - (10)].atom));
	}
    break;

  case 51:
/* Line 1807 of yacc.c  */
#line 999 "osc_expr_parser.y"
    {
		// matlab-style range
		osc_expr_arg_append((yyvsp[(2) - (5)].arg), (yyvsp[(4) - (5)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "aseq", (yyvsp[(2) - (5)].arg));
 	}
    break;

  case 52:
/* Line 1807 of yacc.c  */
#line 1004 "osc_expr_parser.y"
    {
		// matlab-style range
		osc_expr_arg_append((yyvsp[(2) - (7)].arg), (yyvsp[(6) - (7)].arg));
		osc_expr_arg_append((yyvsp[(2) - (7)].arg), (yyvsp[(4) - (7)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "aseq", (yyvsp[(2) - (7)].arg));
 	}
    break;

  case 53:
/* Line 1807 of yacc.c  */
#line 1010 "osc_expr_parser.y"
    {
		// ternary conditional
		osc_expr_arg_append((yyvsp[(1) - (5)].arg), (yyvsp[(3) - (5)].arg));
		osc_expr_arg_append((yyvsp[(1) - (5)].arg), (yyvsp[(5) - (5)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "if", (yyvsp[(1) - (5)].arg));
  	}
    break;

  case 54:
/* Line 1807 of yacc.c  */
#line 1016 "osc_expr_parser.y"
    {
		// null coalescing operator from C#.  
		// /foo ?? 10 means /foo if /foo is in the bundle, otherwise 10
		(yyval.expr) = osc_expr_parser_reduce_NullCoalescingOperator(&yylloc, input_string, (yyvsp[(1) - (3)].atom), (yyvsp[(3) - (3)].arg));
		osc_atom_u_free((yyvsp[(1) - (3)].atom)); // the above function will copy that
	}
    break;

  case 55:
/* Line 1807 of yacc.c  */
#line 1022 "osc_expr_parser.y"
    {
		// null coalescing operator from C#.  
		// /foo ?? 10 means /foo if /foo is in the bundle, otherwise 10
		t_osc_expr *if_expr = osc_expr_parser_reduce_NullCoalescingOperator(&yylloc, input_string, (yyvsp[(1) - (3)].atom), (yyvsp[(3) - (3)].arg));
		if(!if_expr){
			return 1;
		}
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (3)].atom), 0, &ptr);
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, ptr);
		t_osc_expr_arg *arg2 = osc_expr_arg_alloc();
		osc_expr_arg_setExpr(arg2, if_expr);
		(yyval.expr) = osc_expr_parser_reduce_InfixOperator(&yylloc, input_string, "=", arg, arg2);
		osc_atom_u_free((yyvsp[(1) - (3)].atom));
	}
    break;

  case 56:
/* Line 1807 of yacc.c  */
#line 1038 "osc_expr_parser.y"
    {
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "list", (yyvsp[(2) - (3)].arg));
	}
    break;

  case 57:
/* Line 1807 of yacc.c  */
#line 1042 "osc_expr_parser.y"
    {
		osc_expr_arg_setNext((yyvsp[(1) - (4)].arg), (yyvsp[(3) - (4)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "nth", (yyvsp[(1) - (4)].arg));
	}
    break;

  case 58:
/* Line 1807 of yacc.c  */
#line 1046 "osc_expr_parser.y"
    {
		char *ptr = NULL;
		osc_atom_u_getString((yyvsp[(1) - (4)].atom), 0, &ptr);
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setOSCAddress(arg, ptr);
		osc_expr_arg_setNext(arg, (yyvsp[(3) - (4)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "nth", arg);
		osc_atom_u_free((yyvsp[(1) - (4)].atom));
	}
    break;

  case 59:
/* Line 1807 of yacc.c  */
#line 1056 "osc_expr_parser.y"
    {
		t_osc_expr_arg *arg = osc_expr_arg_alloc();
		osc_expr_arg_setFunction(arg, (yyvsp[(2) - (5)].func));
		osc_expr_arg_append(arg, (yyvsp[(4) - (5)].arg));
		(yyval.expr) = osc_expr_parser_reduce_PrefixFunction(&yylloc, input_string, "apply", arg);
	}
    break;

  case 60:
/* Line 1807 of yacc.c  */
#line 1063 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom))) + 3];
		sprintf(buf, "%s++", osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;

  case 61:
/* Line 1807 of yacc.c  */
#line 1069 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom))) + 3];
		sprintf(buf, "%s--", osc_atom_u_getStringPtr((yyvsp[(1) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;

  case 62:
/* Line 1807 of yacc.c  */
#line 1075 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom))) + 3];
		sprintf(buf, "++%s", osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;

  case 63:
/* Line 1807 of yacc.c  */
#line 1081 "osc_expr_parser.y"
    {
		char buf[strlen(osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom))) + 3];
		sprintf(buf, "--%s", osc_atom_u_getStringPtr((yyvsp[(2) - (4)].atom)));
		osc_expr_parser_reportInvalidLvalError(&yylloc, input_string, buf);
		return 1;
	}
    break;


/* Line 1807 of yacc.c  */
#line 3012 "osc_expr_parser.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, exprstack, tmp_exprstack, rec, scanner, input_string, buflen, buf, startcond, started);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


