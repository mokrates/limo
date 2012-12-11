#include "limo.h"

// chartokens: (, ), {, }, [, ], ',', ;, *, %, $
#define limpy_TOKEN_EQUALS 1
#define limpy_TOKEN_MULMUL 2

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
    limo_error("parser error, putback_token already full. implementation error");

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

  if (strchr("[]{}()=+-$%,;\"", c))
    return c;

  if (isalnum(c)) {
    i=0;
    while (isalnum(c) && i<255) {
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

limo_data *limpy_param_reader(reader_stream *rs)
{

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
  default: limo_error("'(' or '[' expected.");
  }

  compound = limpy_assignment_reader(rs);
  if (limpy_gettoken(rs, &token_ld) != limpy_TOKEN_FOR)
    limo_error("'for' expected.");

  t = limpy_gettoken(rs, &token_ld);
  if (t == '[') {
    lval = limpy_list_reader(rs);
    lval_is_limpy_list = 1;
    lval = CDR(lval); // removing "LIST" token
  }
  else {
    limpy_ungettoken(t, token_ld);
    lval = limpy_expr_reader(rs);
  }
  
  if (limpy_gettoken(rs, &token_ld) != limpy_TOKEN_IN)
    limo_error("'in' expected.");
  
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


limo_data *limpy_list_reader(reader_stream *rs)
{
  limo_data *ld, *token_ld;
  limo_data **ld_into = &ld;
  int c;

  ld = make_cons(make_sym("LIST"), make_nil());
  ld_into = &CDR(ld);

  c = limpy_gettoken(rs, &token_ld);

  while (1) {
    if (c == ']') {  // emacs
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
    else if (c==']') // [ emacs
      continue;
    else
      limo_error("',' or ']' expected."); // [ emacs
  }

  return ld;
}

limo_data *limpy_expr_reader(reader_stream *rs)
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
    limo_expr = limpy_list_reader(rs);
    return limo_expr;

  case '%':
    limo_expr = limpy_generator_reader(rs);
    return limo_expr;

  case '"':
    limo_ungetc('"', rs);
    limo_expr = reader(rs);
    return limo_expr;

  case limpy_TOKEN_NUMBER:
  case limpy_TOKEN_SYMBOL:
    return token_ld;

  default:
    limo_error("unimplemented");
  }
}

limo_data *limpy_assignment_reader(reader_stream *rs)
{
  int t;
  limo_data *token_ld;
  limo_data *lval;
  limo_data *rval;
  int lval_is_limpy_list = 0;

  t =limpy_gettoken(rs, &token_ld);
  if (t == '[') {
    lval = limpy_list_reader(rs);
    lval_is_limpy_list = 1;
  }
  else {
    limpy_ungettoken(t, token_ld);
    lval = limpy_expr_reader(rs);
  }

  t = limpy_gettoken(rs, &token_ld);

  if (t == '=') {
    if (lval_is_limpy_list)
      lval = CDR(lval);
    rval = limpy_assignment_reader(rs);
    return make_cons(make_sym("LIMPY-ASSIGN"), 
		     make_cons(lval, make_cons(rval, make_nil())));
  }
  else {
    limpy_ungettoken(t, token_ld);
    return lval;
  }
}

limo_data *limpy_statement_reader(reader_stream *rs)
{
  int t;
  limo_data *token_ld;
  limo_data *limo_expr;

  limo_expr = limpy_assignment_reader(rs);
  
  if (limpy_gettoken(rs, &token_ld) != ';')
    limo_error("';' expected.");
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
