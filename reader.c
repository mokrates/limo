#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "limo.h"

#define BUFFER_SIZE 257

extern char **environ;

char *prompt=NULL;

///// history file for readline

// TODO: the two following functions stink
// and i don't quite know, why read history
// is called after every [return] on readline.

void limo_history_write_history(char *rl)
{
  char *home;
  char **env;
  char history_file_name[256];
  FILE *f;

  env=environ;
  while (*env != NULL && strncmp("HOME=", *env, 5))
    env++;

  if (*env != NULL) {
    home = *env+5;
    strncpy(history_file_name, home, 200);
    strcat(history_file_name, "/.limo_history");

    write_history(history_file_name);
  }
}

void limo_history_read_history(void)
{
  char *home;
  char **env;
  static char history_file_name[256]="\0";
  FILE *f;

  if (history_file_name[0]!='\0')
    return;

  env=environ;
  while (*env != NULL && strncmp("HOME=", *env, 5))
    env++;

  if (*env != NULL) {
    home = *env+5;
    strncpy(history_file_name, home, 200);
    strcat(history_file_name, "/.limo_history");

    clear_history();
    read_history(history_file_name);
  }
}


///// !history file for readline

char *rl_completer_generator(const char *text, int state)
{
  static limo_data *lookup_pos;
  static int textlen=0;
  char *res;

  if (!state) {
    lookup_pos = dict_to_list(CDR(globalenv));
    textlen = strlen(text);
  }

  while (!is_nil(lookup_pos)) {
    if (!strncasecmp(CAR(CAR(lookup_pos))->d_string, text, textlen)) {
      res = (char *)malloc(strlen(CAR(CAR(lookup_pos))->d_string)+1); // yes, malloc
      strcpy(res, CAR(CAR(lookup_pos))->d_string);
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
      if (limo_register & LR_SIGINT) {
        limo_register &= ~LR_SIGINT;
        limo_error("Keyboard Interrupt");
      }
      if (rl == NULL) { // EOF
	rs->eof=1;
	return EOF;
      }
      else {
	rs->stream.readline = (char *)GC_malloc(strlen(rl)+2);
	strcpy(rs->stream.readline, rl);
	if (strlen(rl)) {
	  add_history(rl);
	  limo_history_write_history(rl);
	}
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

int limo_rl_inited = 0;
reader_stream *limo_rs_make_readline(void)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));
  memset(rs, 0, sizeof *rs);

  if (!limo_rl_inited) {
    rl_completion_entry_function = rl_completer_generator;
    rl_completer_word_break_characters = " \t\n\"\\'`@{([";
    rl_catch_signals = 1;
    limo_rl_inited = 1;
  }
  rl_variable_bind("blink-matching-paren", "on");
  limo_history_read_history();

  rs->type = RS_READLINE;
  rs->stream.readline = (char *)GC_malloc(1);
  rs->stream.readline[0] = '\0';
  rs->ungetc_buf_pos=0;
  rs->eof=0;
  rs->filename="*READLINE*";
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

int read_skip_space_comments(reader_stream *f)
{
  int c;
  c=limo_getc(f);
  while ((isspace(c) || c==';' || c=='#') && !(c==EOF) && !limo_eof(f)) {
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
    if (c == EOF)
      limo_error("syntax error (read_list) (%s, %i)", f->filename, f->line);
    if (c == ')' || c==']') { // ( for emacs
      (*ld_into)->d_cons = NULL;
      break;
    }
    else
      (*ld_into)->d_cons = (limo_cons *)GC_malloc(sizeof (limo_cons));
      
    limo_ungetc(c, f);
    (*ld_into)->d_cons->car = reader(f);

    c=read_skip_space_comments(f);
    if (c=='.') { // pair
      (*ld_into)->d_cons->cdr = reader(f);
      c=read_skip_space_comments(f);
      if (c != ')' && c != ']')
	limo_error("syntax error (read_list) (%s, %i)", f->filename, f->line);
      break;
    }
    else {
      limo_ungetc(c, f);
      (*ld_into)->d_cons->cdr = make_limo_data();
      ld_into = &((*ld_into)->d_cons->cdr);
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
  int c;

  for (i=0; i<BUFFER_SIZE-1; ++i) {
    c=limo_getc(f);
    if (strchr(")]", c) || isspace(c) || c==EOF) {  // ( for emacs
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

// strings (ld->d_string) has a buffer which is stringlen+1 bytes long
// and is zero-terminated. BUT in ld->hash (in union with ld->string_length),
// the stringlength is saved and
// strings can contain \0
limo_data *read_string(reader_stream *f)
{
  limo_data *ld = make_limo_data();
  int i;
  char *buf;
  int c;
  int cur_bufsiz = BUFFER_SIZE;

  buf = (char *)GC_malloc(cur_bufsiz);
  for (i=0;; ++i) {
    c=limo_getc(f);
    if (c=='"')
      break;
    if (c==EOF)
      limo_error("EOF in middle of string");
    else if (c=='\\')
      switch (c=limo_getc(f)) {
      case EOF: limo_error("EOF in middle of string");
      case 'n': c='\n'; break;
      case 't': c='\t'; break;
      case '"': c='"'; break;
      case '\\':c='\\'; break;
      case 'r': c='\r'; break;
      case '(': c='('; break;     // can be used to not confuse emacs paredit mode
      case ')': c=')'; break;     // see https://www.gnu.org/software/guile/manual/guile.txt
      case '0': c='\0'; break;
      }
    buf[i]=c;
    if (i>=cur_bufsiz-1) {
      cur_bufsiz += BUFFER_SIZE;
      buf = (char *)GC_realloc(buf, cur_bufsiz);
    }
  }
  buf[i]='\0';
  ld->type = limo_TYPE_STRING;
  ld->string_length = i;
  ld->d_string = (char *)GC_malloc(i + 1);
  memcpy(ld->d_string, buf, i+1);
  return ld;
}

inline limo_data *annotate(limo_data *ld, limo_annotation *la)
{
  ld->annotation = la;
  return ld;
}

limo_data *reader(reader_stream *f)
{
  int c;
  limo_data *ld;
  limo_annotation *la;

  prompt = "\xce\001\xbb\002imo > ";   // the lambda is a wide-char, so we ignore one char to get readline to count the prompt length correctly

  c=read_skip_space_comments(f);
  la = limo_rs_annotation(f);

  prompt = "\xce\001\xbb\002imo>> ";

  if (c == EOF)
    return make_nil();

  if (c=='(') { // list
    return annotate(read_list(f), la);
  }
  else if (c=='[') { // (BRACKET list...)
    return annotate(make_cons(make_sym("BRACKET"), read_list(f)), la);
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
  else if (c=='{') {  // limpy start
    return limpy_block_reader(f);
  }
  else {
    limo_ungetc(c, f);
    return annotate(read_sym_num(f), la);
  }
}
