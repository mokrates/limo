#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "limo.h"
#ifndef NO_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif //!NO_READLINE


#define BUFFER_SIZE 257

extern char **environ;

char *prompt=NULL;

#ifndef NO_READLINE
///// history file for readline

// TODO: the two following functions stink
// and i don't quite know, why read history
// is called after every [return] on readline.

void limo_history_write_history(char *rl)
{
  char *home;
  char **env;
  char history_file_name[256];

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
#endif //!NO_READLINE

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

  if (!is_nil(rs->ungetc_buf)) {
    c=CAR(rs->ungetc_buf)->d_string[0];
    rs->ungetc_buf = CDR(rs->ungetc_buf);
    return c;
  }

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

#ifndef NO_READLINE
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
#endif //!NO_READLINE

  default:
    throw(make_sym("UNKNOWN-READER-STREAM-TYPE"));
  }
}

char limo_eof(reader_stream *rs)
{
  switch (rs->type) {
  case RS_FILE: return feof(rs->stream.file);
  case RS_STR: return rs->stream.str[rs->pos]=='\0';

#ifndef NO_READLINE
  case RS_READLINE: return rs->eof;
#endif //!NO_READLINE

  default:
    throw(make_sym("UNKNOWN-READER-STREAM-TYPE"));
  }
}

#ifndef NO_READLINE
int limo_rl_inited = 0;
reader_stream *limo_rs_make_readline(limo_data *env)
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
  rs->ungetc_buf = nil;
  rs->eof=0;
  rs->env = env;
  rs->filename="*READLINE*";
  return rs;
}
#endif //!NO_READLINE

reader_stream *limo_rs_from_file(FILE *f, char *filename, limo_data *env)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));
  memset(rs, 0, sizeof *rs);

  rs->type = RS_FILE;
  rs->stream.file = f;
  rs->ungetc_buf = nil;
  rs->eof=0;
  rs->filename = filename;
  rs->line = 0;
  rs->pos = 0;
  rs->env = env;
  return rs;
}

reader_stream *limo_rs_from_string(char *str, limo_data *env)
{
  reader_stream *rs = (reader_stream *)GC_malloc(sizeof (reader_stream));
  memset(rs, 0, sizeof *rs);

  rs->type = RS_STR;
  rs->stream.str = limo_strdup(str);
  rs->ungetc_buf = nil;
  rs->eof=0;
  rs->filename = "*STRING*";
  rs->line = 0;
  rs->pos = 0;
  rs->env = env;
  return rs;
}

void limo_ungetc(char c, reader_stream *rs)
{
  rs->ungetc_buf = make_cons(make_char(c), rs->ungetc_buf);
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

  default:
    limo_error("limo_rs_annotation: unknown reader stream type");
    return NULL;  // never reached.
  }
}

//////////////////////////////////////////////////////////

int read_skip_space_comments(reader_stream *f)
{
  int c;
  c=limo_getc(f);
  while ((isspace(c) || c==';') && !(c==EOF) && !limo_eof(f)) {
    if (c == ';')
      while (c != '\n')
	c=limo_getc(f);
    else
      c=limo_getc(f);
  }
  return c;
}

//////////////////////////////////////////////////////////
// Read a dispatch macro
// CAUTION! Can return NULL in case of
// for example #|comment|#

limo_data *read_dispatch_macro(reader_stream *f)
{
  char disp_char[2]="\0\0";
  limo_data *readtable;
  limo_data *readtable_entry;
  limo_data *res;
  disp_char[0] = limo_getc(f);

  // get dispatch function
  readtable = var_lookup(f->env, make_sym("*READTABLE*")); // throws exception if *READTABLE* doesn't exist.
  readtable_entry = dict_get_place(readtable, make_string(disp_char))->cons;    // returns nil, if entry doesn't exist
  if (!readtable_entry)
    throw(make_cons(make_sym("MACRO-DISPATCH-ERROR"), make_string(disp_char)));
  res = eval(make_cons(CDR(readtable_entry),
		       make_cons(make_special(sym_reader_stream, f),
				 nil)),
	     f->env);
  if (is_nil(res))
    return NULL;
  else {
    if (res->type != limo_TYPE_CONS)
      throw(make_cons(make_sym("MACRO-DISPATCH-ERROR"), make_string("reader macro must return either nil or a cons")));

    return CAR(res);
  }
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
      (*ld_into)->type = limo_TYPE_NIL;
      break;
    }
    else {
      (*ld_into)->type = limo_TYPE_CONS;
    }

    if (c == '#') {
      limo_data *mres = read_dispatch_macro(f);
      if (!mres) {
        c=read_skip_space_comments(f);
        continue;
      }
      
      CAR((*ld_into)) = mres;
    }
    else {
      limo_ungetc(c, f);
      CAR((*ld_into)) = reader(f);
    }

    c=read_skip_space_comments(f);
    if (c=='.') { // pair
      CDR(*ld_into) = reader(f); // read exactly one item
      c=read_skip_space_comments(f);
      
      while (c=='#') {
        // this reads ( item1 ... itemN . #|comment1_after_dot|# item_after_dot #|>>HERE<<|#)
        limo_data *mres = read_dispatch_macro(f);
        if (mres)
          limo_error("syntax error (read_list) (%s, %i)", f->filename, f->line);
        c=read_skip_space_comments(f);
      }
        
      if (c != ')' && c != ']')
	limo_error("syntax error (read_list) (%s, %i)", f->filename, f->line);
      break;
    }
    else {
      limo_ungetc(c, f);
      CDR(*ld_into) = make_limo_data();
      ld_into = &CDR(*ld_into);
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
    if (strchr(buf, '.') || strchr(buf, 'e') || strchr(buf, 'E'))
      return make_float_from_str(buf);
    else
      return make_rational_from_str(buf);
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
  else if (c=='#') {  // dispatch-char
    limo_data *res;
    res = read_dispatch_macro(f);
    if (!res)  // empty result, possibly #| comment |#
      return reader(f);
    else
      return annotate(res, la);
  }
  else {
    limo_ungetc(c, f);
    return annotate(read_sym_num(f), la);
  }
}
