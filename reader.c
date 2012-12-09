#include <stdio.h>
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
  static int textlen=0;
  char *res;

  if (!state) {
    lookup_pos = dict_to_list(CDR(globalenv->data.d_env));
    textlen = strlen(text);
  }
  
  while (!is_nil(lookup_pos)) {
    if (!strncasecmp(CAR(CAR(lookup_pos))->data.d_string, text, textlen)) {
      res = (char *)malloc(strlen(CAR(CAR(lookup_pos))->data.d_string)+1);
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
  int c;

  if (rs->ungetc_buf_pos > 0)
    return rs->ungetc_buf[--(rs->ungetc_buf_pos)];

  switch (rs->type) {
  case RS_FILE:
    c=fgetc(rs->stream.file);
    if (c=='\n') {
      rs->line++;
      rs->pos =0;
    }
    else if (c=='\t')
      rs->pos +=8;
    else
      rs->pos ++;

    return c;

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
	if (strlen(rl))
	  add_history(rl);
	rs->stream.readline[strlen(rs->stream.readline)]='\n';
	free(rl);

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
  memset(rs, 0, sizeof *rs);

  rl_completion_entry_function = rl_completer_generator;

  rs->type = RS_READLINE;
  rs->stream.readline = (char *)GC_malloc(1);
  rs->stream.readline[0] = '\0';
  rs->ungetc_buf_pos=0;
  rs->eof=0;
  return rs;
}

reader_stream *limo_rs_from_file(FILE *f, char *filename)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));
  memset(rs, 0, sizeof *rs);

  rs->type = RS_FILE;
  rs->stream.file = f;
  rs->ungetc_buf_pos=0;
  rs->eof=0;
  rs->filename = filename;
  rs->line = 0;
  rs->pos = 0;
  return rs;
}

reader_stream *limo_rs_from_string(char *str)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));
  memset(rs, 0, sizeof *rs);

  rs->type = RS_STR;
  rs->stream.str = limo_strdup(str);
  rs->ungetc_buf_pos=0;
  rs->eof=0;
  rs->filename = "*STRING*";
  rs->line = 0;
  rs->pos = 0;
  return rs;
}

void limo_ungetc(char c, reader_stream *rs)
{
  rs->ungetc_buf[rs->ungetc_buf_pos++] = c;
}

limo_annotation *limo_rs_annotation(reader_stream *rs)
{
  switch (rs->type) {
  case RS_FILE:
    return make_annotation(rs->filename, rs->line, rs->pos);
  
  case RS_STR:
    return make_annotation("*STRING*", 0, rs->pos);

  case RS_READLINE:
    return make_annotation("*READLINE*", 0, rs->pos);
  }
}

//////////////////////////////////////////////////////////

char read_skip_space_comments(reader_stream *f)
{
  char c;
  c=limo_getc(f);
  while ((isspace(c) || c==';' || c=='#') && !limo_eof(f)) {
    if (c == ';' || c=='#')
      while (c != '\n')
	c=limo_getc(f);
    else
      c=limo_getc(f);
  }
  return c;
}

limo_data *read_list(reader_stream *f)
{
  limo_data *ld=make_limo_data();
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
      (*ld_into)->data.d_cons->cdr = make_limo_data();
      ld_into = &((*ld_into)->data.d_cons->cdr);
      (*ld_into)->type = limo_TYPE_CONS;
    }
    c=read_skip_space_comments(f);
  }

  return ld;
}

limo_data *reader_macro(reader_stream *f)
{
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
  limo_data *ld = make_limo_data();
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

inline limo_data *annotate(limo_data *ld, limo_annotation *la)
{
  ld->annotation = la;
  return ld;
}

limo_data *reader(reader_stream *f)
{
  char c;
  limo_data *ld;
  limo_annotation *la;

  prompt = "λimo > ";

  c=read_skip_space_comments(f);
  la = limo_rs_annotation(f);

  prompt = "λimo>> ";

  if (limo_eof(f))
    return make_nil();

  if (c=='(') { // list
    return annotate(read_list(f), la);
  }
  else if (c=='"') 
    return annotate(read_string(f), la);
  else if (strchr("'`,", c)) {  // reader-macro-chars
    limo_ungetc(c, f);
    return annotate(reader_macro(f), la);
  }
  else if (c==')') {
    limo_error("Syntax_error: too many )s");
    return annotate(make_nil(), la);
  }
  else {
    limo_ungetc(c, f);
    return annotate(read_sym_num(f), la);
  }
}
