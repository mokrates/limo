#include <stdio.h>
#include <gc/gc.h>
#include <ctype.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "limo.h"

#define BUFFER_SIZE 257

char *prompt=NULL;

char *rl_completer_generator(const char *text, int state) 
{
  static limo_data *lookup_pos;
  limo_data *env_list = globalenv->data.d_env;
  static int textlen=0;
  char *res;

  if (!state) {
    lookup_pos = globalenv->data.d_env;
    textlen = strlen(text);
  }
  
  while (!is_nil(lookup_pos)) {
    if (!strncasecmp(CAR(CAR(lookup_pos))->data.d_string, text, textlen)) {
      res = (char *)malloc(strlen(CAR(CAR(lookup_pos))->data.d_string));
      strcpy(res, CAR(CAR(lookup_pos))->data.d_string);
      lookup_pos= CDR(lookup_pos);
      return res;
    }
    else
      lookup_pos= CDR(lookup_pos);
  }
  return NULL;
}

int limo_getc(reader_stream *rs)
{
  if (rs->ungetc_buf_pos > 0)
    return rs->ungetc_buf[--(rs->ungetc_buf_pos)];

  switch (rs->type) {
  case RS_FILE:
    return fgetc(rs->stream.file);

  case RS_STR:
    if (!rs->stream.str[rs->pos])
      return -1;
    else
      return rs->stream.str[rs->pos++];

  case RS_READLINE:
    if (!rs->stream.readline[rs->pos]) {
      char *rl = readline(prompt);
      if (rl == NULL) { // EOF
	rs->eof=1;
	return -1;
      }
      else {
	rs->stream.readline = (char *)GC_malloc(strlen(rl)+2);
	strcpy(rs->stream.readline, rl);
	rs->stream.readline[strlen(rs->stream.readline)]=' ';
	free(rl);
	add_history(rs->stream.readline);
	rs->pos = 0;
      }
    }
    return rs->stream.readline[rs->pos++];
  }
}

char limo_eof(reader_stream *rs)
{
  switch (rs->type) {
  case RS_FILE: return feof(rs->stream.file);
  case RS_STR: return rs->stream.str[rs->pos]=='\0';
  case RS_READLINE: return rs->eof;
  }
}

reader_stream *limo_rs_make_readline(void)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));

  rl_completion_entry_function = rl_completer_generator;

  rs->type = RS_READLINE;
  rs->stream.readline = (char *)GC_malloc(1);
  rs->stream.readline[0] = '\0';
  rs->ungetc_buf_pos=0;
  rs->eof=0;
  return rs;
}

reader_stream *limo_rs_from_file(FILE *f)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));
  rs->type = RS_FILE;
  rs->stream.file = f;
  rs->ungetc_buf_pos=0;
  rs->eof=0;
  return rs;
}

void limo_ungetc(char c, reader_stream *rs)
{
  rs->ungetc_buf[rs->ungetc_buf_pos++] = c;
}

//////////////////////////////////////////////////////////

char read_skip_space_comments(reader_stream *f)
{
  char c;
  c=limo_getc(f);
  while ((isspace(c) || c==';') && !limo_eof(f)) {
    c=limo_getc(f);
    if (c == ';')
      while (c != '\n')
	c=limo_getc(f);
  }
  return c;
}

limo_data *read_list(reader_stream *f)
{
  limo_data *ld=(limo_data *)GC_malloc(sizeof (limo_data));
  limo_data **ld_into = &ld;
  char c;

  ld->type = limo_TYPE_CONS;

  c=read_skip_space_comments(f);
  while (1) {
    if (c == ')') {
      (*ld_into)->data.d_cons = NULL;
      break;
    }
    else
      (*ld_into)->data.d_cons = (limo_cons *)GC_malloc(sizeof (limo_cons));
      
    limo_ungetc(c, f);
    (*ld_into)->data.d_cons->car = reader(f);

    c=read_skip_space_comments(f);
    if (c=='.') { // pair
      (*ld_into)->data.d_cons->cdr = reader(f);
      c=read_skip_space_comments(f);
      if (c != ')')
	limo_error("syntax error (read_list)");
      break;
    }
    else {
      limo_ungetc(c, f);
      (*ld_into)->data.d_cons->cdr = (limo_data *)GC_malloc(sizeof (limo_data));
      ld_into = &((*ld_into)->data.d_cons->cdr);
      (*ld_into)->type = limo_TYPE_CONS;
    }
    c=read_skip_space_comments(f);
  }

  return ld;
}

limo_data *reader_macro(reader_stream *f)
{
  limo_data *ld;
  char c;
  char *macro;

  c=limo_getc(f);
  switch (c) {
  case '\'': macro="QUOTE"; break;
  case '`': macro="QUASIQUOTE"; break;
  case ',': 
    if ((c=limo_getc(f)) == '@')
      macro="SPLICE";
    else {
      macro="COMMA";
      limo_ungetc(c,f);
    }
    break;
  default: limo_error("this should not happen (reader_macro)");
  }

  return make_cons(make_sym(macro), make_cons(reader(f), make_nil()));
}

limo_data *read_sym_num(reader_stream *f)
{
  int i;
  char buf[BUFFER_SIZE];
  char c;

  for (i=0; i<BUFFER_SIZE-1; ++i) {
    c=limo_getc(f);
    if (strchr(")", c) || isspace(c)) {
      limo_ungetc(c, f);
      break;
    }
    buf[i]=c;
  }
  buf[i]='\0';

  if (isdigit(buf[0]) ||
      (buf[0] == '-' && isdigit(buf[1])))
    return make_number_from_str(buf);
  else 
    return make_sym(buf);
}

limo_data *read_string(reader_stream *f)
{
  limo_data *ld = (limo_data *)GC_malloc(sizeof (limo_data));
  int i;
  char buf[BUFFER_SIZE];
  char c;
  for (i=0; i<BUFFER_SIZE-1; ++i) {
    c=limo_getc(f);
    if (c=='"')
      break;
    else if (c=='\\')
      switch (c=limo_getc(f)) {
      case 'n': c='\n'; break;
      case 't': c='\t'; break;
      case '"': c='"'; break;
      case '\\':c='\\'; break;
      }
    buf[i]=c;
  }
  buf[i]='\0';
  ld->type = limo_TYPE_STRING;
  ld->data.d_string = (char *)GC_malloc(strlen(buf) + 1);
  strcpy(ld->data.d_string, buf);
  return ld;
}

limo_data *reader(reader_stream *f)
{
  char c;

  prompt = "λimo > ";

  c=read_skip_space_comments(f);

  prompt = "λimo>> ";

  if (limo_eof(f))
    return make_nil();

  if (c=='(')  // list
    return read_list(f);
  else if (c=='"')
    return read_string(f);
  else if (strchr("'`,", c)) {  // reader-macro-chars
    limo_ungetc(c, f);
    return reader_macro(f);
  }
  else {
    limo_ungetc(c, f);
    return read_sym_num(f);
  }
}
