#include "limo.h"

// chartokens: (, ), {, }, [, ], ',', ;, *, %, $
#define limpy_TOKEN_EQUALS 1
#define limpy_TOKEN_SETF   2
#define limpy_TOKEN_MULMUL 3

#define limpy_TOKEN_VAR    5

#define limpy_TOKEN_FOR    10
#define limpy_TOKEN_IN     11

#define limpy_TOKEN_IF     12
#define limpy_TOKEN_ELIF   13
#define limpy_TOKEN_ELSE   14

#define limpy_TOKEN_WHILE  15
#define limpy_TOKEN_DEF    16

#define limpy_TOKEN_SYMBOL 20
#define limpy_TOKEN_NUMBER 21

static struct {
  int full;
  int token;
  limo_data *data;
} putback_token = {0, 0, NULL};

void limpy_parser_error(char *msg, reader_stream *rs)
{
  limo_error("limpy parse error(%i, %i): %s", rs->line, rs->pos, msg);
}

char limpy_read_skip_space_comments(reader_stream *f)
{
  char c;
  c=limo_getc(f);
  while ((isspace(c) || c=='#') && !limo_eof(f)) {
    if (c=='#')
      while (c != '\n')
	c=limo_getc(f);
    else
      c=limo_getc(f);
  }
  return c;
}

void limpy_ungettoken(int token, limo_data *data)
{
  if (putback_token.full)
    limo_error("putback_token already full. implementation error");

  putback_token.full = 1;
  putback_token.token = token;
  putback_token.data = data;
}

int limpy_gettoken(reader_stream *rs, limo_data **ld)
{
  int c;
  char buf[256];
  int i;

  if (putback_token.full) {
    putback_token.full = 0;
    *ld = putback_token.data;
    return putback_token.token;
  }

  c = limpy_read_skip_space_comments(rs);

  if (strchr("[]{}()=+-$%.,;\"", c))
    return c;

  if (c==':') {
    c=limo_getc(rs);
    if (c=='=')
      return limpy_TOKEN_SETF;
    else {
      limo_ungetc(c, rs);
      c=':';
    }
  }

  if (isalnum(c) || c==':') {
    i=0;
    while ((isalnum(c) || c=='-' || (c==':' && i==0)) && i<255) {
      buf[i++] = c;
      c=limo_getc(rs);
    }
    limo_ungetc(c, rs);
    buf[i]='\0';

    if (isdigit(buf[0])) {
      *ld = make_number_from_str(buf);
      return limpy_TOKEN_NUMBER;
    }
    if (!(strcmp("for", buf)))
      return limpy_TOKEN_FOR;
    else if (!(strcmp("in", buf)))
      return limpy_TOKEN_IN;
    else if (!(strcmp("while", buf)))
      return limpy_TOKEN_WHILE;
    else if (!(strcmp("if", buf)))
      return limpy_TOKEN_IF;
    else if (!(strcmp("elif", buf)))
      return limpy_TOKEN_ELIF;
    else if (!(strcmp("else", buf)))
      return limpy_TOKEN_ELSE;
    else if (!(strcmp("def", buf)))
      return limpy_TOKEN_DEF;
    else {
      *ld = make_sym(buf);
      return limpy_TOKEN_SYMBOL;
    }
  }
}

limo_data *limpy_generator_reader(reader_stream *rs)
{
  int t;
  limo_data *token_ld;
  limo_data *cons;

  limo_data *compound;
  limo_data *lval;
  limo_data *list;
  limo_data *cond;
  
  limo_data *res;

  int gen_or_it;
  int lval_is_limpy_list = 0;
  int has_cond = 0;

  gen_or_it=limpy_gettoken(rs, &token_ld);
  switch (gen_or_it) {
  case '(': cons = make_sym("DCONS"); break;
  case '[': cons = make_sym("CONS"); break;
  default: limpy_parser_error("'(' or '[' expected.", rs);
  }

  compound = limpy_assignment_reader(rs);
  if (limpy_gettoken(rs, &token_ld) != limpy_TOKEN_FOR)
    limpy_parser_error("'for' expected.", rs);

  t = limpy_gettoken(rs, &token_ld);
  if (t == '[') {
    lval = limpy_list_reader(rs, ']'); // [ emacs
    lval_is_limpy_list = 1;
    lval = CDR(lval); // removing "LIST" token
  }
  else {
    limpy_ungettoken(t, token_ld);
    lval = limpy_expr_reader(rs);
  }
  
  if (limpy_gettoken(rs, &token_ld) != limpy_TOKEN_IN)
    limpy_parser_error("'in' expected.", rs);
  
  list = limpy_assignment_reader(rs);

  t = limpy_gettoken(rs, &token_ld);
  if (t == limpy_TOKEN_IF) {
    has_cond = 1;
    cond = limpy_assignment_reader(rs);

    t = limpy_gettoken(rs, &token_ld);
  }

  if (t!= (gen_or_it=='('?')':']'))
    limo_error("'%c' expected.", gen_or_it=='('?')':']');

  if (!has_cond)
    return make_cons(make_sym("LIMPY-GEN"),
		     make_cons(cons, 
			       make_cons(compound,
					 make_cons(lval,
						   make_cons(list, make_nil())))));
  else
    return make_cons(make_sym("LIMPY-GEN-COND"),
		     make_cons(cons, 
			       make_cons(compound,
					 make_cons(lval,
						   make_cons(list, 
							     make_cons(cond, make_nil()))))));
}


limo_data *limpy_list_reader(reader_stream *rs, int right_bracket)
{
  limo_data *ld, *token_ld;
  limo_data **ld_into = &ld;
  int c;

  ld = make_cons(make_sym("LIST"), make_nil());
  ld_into = &CDR(ld);

  c = limpy_gettoken(rs, &token_ld);

  while (1) {
    if (c == right_bracket) {  // emacs
      (*ld_into)->data.d_cons = NULL;
      break;
    }
    else
      (*ld_into)->data.d_cons = (limo_cons *)GC_malloc(sizeof (limo_cons));
      
    limpy_ungettoken(c, token_ld);

    (*ld_into)->data.d_cons->car = limpy_assignment_reader(rs);
    (*ld_into)->data.d_cons->cdr = make_limo_data();
    //    ld_into = &((*ld_into)->data.d_cons->cdr);
    ld_into = &CDR(*ld_into);
    (*ld_into)->type = limo_TYPE_CONS;

    c = limpy_gettoken(rs, &token_ld);

    if (c==',') 
      c=limpy_gettoken(rs, &token_ld);
    else if (c==right_bracket) // [ emacs
      continue;
    else
      limo_error("',' or '%c' expected.", right_bracket); // [ emacs
  }

  return ld;
}

limo_data *limpy_atom_reader(reader_stream *rs)
{
  int t;
  limo_data *token_ld;
  limo_data *limo_expr;

  t = limpy_gettoken(rs, &token_ld);
  switch (t) {
  case '$':
    limo_expr = reader(rs);
    return limo_expr;

  case '[':   // ] emacs
    limo_expr = limpy_list_reader(rs, ']'); // [ emacs
    return limo_expr;

  case '%':
    limo_expr = limpy_generator_reader(rs);
    return limo_expr;

  case '(':
    limo_expr = limpy_assignment_reader(rs);
    t = limpy_gettoken(rs, &token_ld);
    if (t != ')')  // ( emacs
      limpy_parser_error("')' expected.", rs); // ( emacs
    return limo_expr;

  case '"':
    limo_ungetc('"', rs);
    limo_expr = reader(rs);
    return limo_expr;

  case limpy_TOKEN_NUMBER:
  case limpy_TOKEN_SYMBOL: 
    return token_ld;

  default:
    limpy_parser_error("unimplemented", rs);
  }
}

limo_data *limpy_rgroup_reader(limo_data *left_side, reader_stream *rs)
{
  int token;
  limo_data *token_ld;
  limo_data *right_side;
  limo_data *codeblock;

  token = limpy_gettoken(rs, &token_ld);
  switch (token) {
  case '(':  // function call // ) emacs
    right_side = CDR(limpy_list_reader(rs, ')')); // parameters

    token = limpy_gettoken(rs, &token_ld);
    if (token == '{') {
      limo_data *iter = right_side;
      codeblock = limpy_block_reader(rs);
      if (!is_nil(iter)) {
	while (! is_nil(CDR(iter)))
	  iter = CDR(iter);
	CDR(iter) = make_cons(codeblock, make_nil());
      }
      else
	right_side = make_cons(codeblock, make_nil());
      
      return limpy_rgroup_reader(make_cons(left_side,right_side), rs);
    }
    else {
      limpy_ungettoken(token, token_ld);
      return limpy_rgroup_reader(make_cons(left_side,right_side), rs);
    }

  case '[':  // index // ] emacs
    // TODO implement slicing
    right_side = limpy_assignment_reader(rs);
    token = limpy_gettoken(rs, &token_ld);
    if (token != ']') // [ emacs
      limpy_parser_error("']' expected.", rs);
    return limpy_rgroup_reader(make_cons(make_sym("LIMPY-GETINDEX"),
					 make_cons(left_side, // index
						   make_cons(right_side, make_nil()))), rs); // list/string
    
  default:
    limpy_ungettoken(token, token_ld);
    return left_side;
  }
}

limo_data *limpy_expr_reader(reader_stream *rs)
{
  return limpy_rgroup_reader(limpy_atom_reader(rs), rs);
}

limo_data *limpy_assignment_reader(reader_stream *rs)
{
  int t;
  limo_data *token_ld;
  limo_data *lval;
  limo_data *rval;
  int lval_is_limpy_list = 0;
  int setq_or_setf;

  t =limpy_gettoken(rs, &token_ld);
  if (t == '[') {
    lval = limpy_list_reader(rs, ']');
    lval_is_limpy_list = 1;
  }
  else {
    limpy_ungettoken(t, token_ld);
    lval = limpy_expr_reader(rs);
  }

  t = limpy_gettoken(rs, &token_ld);

  if (t == '=' || t==limpy_TOKEN_SETF) {
    setq_or_setf = t;
    if (lval_is_limpy_list)
      lval = CDR(lval);
    rval = limpy_assignment_reader(rs);

    if (setq_or_setf == '=') {
      return make_cons(make_sym("LIMPY-ASSIGNQ"), 
		       make_cons(lval, make_cons(rval, make_nil())));
    }
    else if (setq_or_setf==limpy_TOKEN_SETF && lval_is_limpy_list) {
      return make_cons(make_sym("LIMPY-UNIFYF"), 
		       make_cons(lval, make_cons(rval, make_nil())));
    }
    else 
      return make_cons(make_sym("LIMPY-ASSIGNF"), 
		       make_cons(lval, make_cons(rval, make_nil())));
  }
  else {
    limpy_ungettoken(t, token_ld);
    return lval;
  }
}

limo_data *limpy_def_reader(reader_stream *rs)
{
  limo_data *name;
  limo_data *params;
  limo_data *codeblock;
  limo_data *token_ld;

  if (limpy_gettoken(rs, &name) != limpy_TOKEN_SYMBOL)
    limpy_parser_error("symbol expected.", rs);
  if (limpy_gettoken(rs, &token_ld) != '(') 
    limpy_parser_error("'(' expected.", rs);

  params = limpy_list_reader(rs, ')'); 

  if (limpy_gettoken(rs, &token_ld) != '{') 
    limpy_parser_error("'{' expected.", rs); 
  codeblock = limpy_block_reader(rs);
  
  return make_cons(make_sym("DEFUN"),
		   make_cons(name,
			     make_cons(CDR(params),
				       make_cons(codeblock, make_nil()))));
}

limo_data *limpy_statement_reader(reader_stream *rs)
{
  int t;
  limo_data *token_ld;
  limo_data *limo_expr;

  t = limpy_gettoken(rs, &token_ld);
  switch (t) {
  case limpy_TOKEN_DEF: 
    limo_expr= limpy_def_reader(rs);
    break;

  default:
    limpy_ungettoken(t, token_ld);
    limo_expr = limpy_assignment_reader(rs);
    break;
  }
  
  if (limpy_gettoken(rs, &token_ld) != ';')
    limpy_parser_error("';' expected.", rs);
  return limo_expr;
}

limo_data *limpy_block_reader(reader_stream *rs)
{
  limo_data *ld, *token_ld;
  limo_data **ld_into = &ld;
  int c;

  ld = make_cons(make_sym("PROGN"), make_nil());
  ld_into = &CDR(ld);

  c = limpy_gettoken(rs, &token_ld);

  while (1) {
    if (c == '}') {
      (*ld_into)->data.d_cons = NULL;
      break;
    }
    else
      (*ld_into)->data.d_cons = (limo_cons *)GC_malloc(sizeof (limo_cons));
      
    limpy_ungettoken(c, token_ld);

    (*ld_into)->data.d_cons->car = limpy_statement_reader(rs);
    (*ld_into)->data.d_cons->cdr = make_limo_data();
    //    ld_into = &((*ld_into)->data.d_cons->cdr);
    ld_into = &CDR(*ld_into);
    (*ld_into)->type = limo_TYPE_CONS;

    c = limpy_gettoken(rs, &token_ld);
  }

  return ld;
}
