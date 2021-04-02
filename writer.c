#include <stdio.h>
#include "limo.h"

void list_put_str(limo_data ***dest, char *str)
{
  (**dest) = make_cons(make_string(str), nil);
  *dest = &CDR(**dest);
}

void l_cons_writer(limo_data ***dest, limo_data *ld)
{
  list_put_str(dest, "(");
  while (ld->type == limo_TYPE_CONS && ld->data.d_cons) {
    l_writer(dest, ld->data.d_cons->car);
    if (ld->data.d_cons->cdr) {
      if (ld->data.d_cons->cdr->type != limo_TYPE_CONS) {
	list_put_str(dest, " . ");
	l_writer(dest, ld->data.d_cons->cdr);
	break;
      }
      else {
	ld=ld->data.d_cons->cdr;
	if (ld->data.d_cons)
	  list_put_str(dest, " ");
      }
    }
  }
  list_put_str(dest, ")");
}

void cons_writer(limo_data *ld)
{
  printf("(");
  while (ld->type == limo_TYPE_CONS && ld->data.d_cons) {
    writer(ld->data.d_cons->car);
    if (ld->data.d_cons->cdr) {
      if (ld->data.d_cons->cdr->type != limo_TYPE_CONS) {
	printf(" . ");
	writer(ld->data.d_cons->cdr);
	break;
      }
      else {
	ld=ld->data.d_cons->cdr;
	if (ld->data.d_cons)
	  printf(" ");
      }
    }
  }
  printf(")");
}

void l_string_writer(limo_data ***dest, limo_data *ld)
{
  char *s = ld->data.d_string;
  int i;
  char cbuf[3] = "\0\0\0";
  list_put_str(dest, "\"");
  for (i=0; i<ld->hash; ++i) {
    switch (*s) {
    case '\r': list_put_str(dest, "\\r"); break;
    case '\t': list_put_str(dest, "\\t"); break;
    case '\n': list_put_str(dest, "\\n"); break;
    case '\\': list_put_str(dest, "\\"); break;
    case '\0': list_put_str(dest, "\\0"); break;
    case '"': list_put_str(dest, "\\\""); break;
    default:
      cbuf[0] = *s;
      list_put_str(dest, cbuf);
    }
    s++;
  }
  list_put_str(dest, "\"");
}

void string_writer(limo_data *ld)
{
  char *s = ld->data.d_string;
  int i;
  printf("\"");
  for (i=0; i<ld->hash; ++i) {
    switch (*s) {
    case '\r': printf("\\r"); break;
    case '\t': printf("\\t"); break;
    case '\n': printf("\\n"); break;
    case '\\': printf("\\"); break;
    case '\0': printf("\\0"); break;
    case '"': printf("\\\""); break;
    default: printf("%c", *s);
    }
    s++;
  }
  printf("\"");
}

void number_writer(limo_data *ld)
{
  printf("%s", repr_number(ld));
}

void l_number_writer(limo_data ***dest, limo_data *ld)
{
  list_put_str(dest, repr_number(ld));
}

void writer(limo_data *ld) // not threadsafe!
{
  static int in_env=0;

  switch (ld->type) {
  case limo_TYPE_STRING: string_writer(ld); break;
  case limo_TYPE_GMPQ: number_writer(ld); break;
  case limo_TYPE_CONS: cons_writer(ld); break;
  case limo_TYPE_SYMBOL: printf("%s", ld->data.d_string); break;
  case limo_TYPE_BUILTIN: printf("#<builtin:%p>", ld->data.d_builtin); break;
  case limo_TYPE_DICT:
    if (in_env)
      printf("#<dict>");
    else {
      in_env++;
      printf("#<dict: "); writer(dict_to_list(ld)); printf(">");
      in_env--;
    }
    break;

  case limo_TYPE_CONST:
    printf("[%s]", CDR(ld)->data.d_string);
    break;

  case limo_TYPE_THUNK:
  case limo_TYPE_LAMBDA: 
  case limo_TYPE_MACRO:
    printf("#<%s:", (ld->type==limo_TYPE_LAMBDA?"lambda":
		     (ld->type==limo_TYPE_MACRO?"macro":
		      "thunk")));
    writer(CDR(ld));
    if (CAR(ld) != globalenv) {
      printf(", env: ");
      writer(CAR(ld));
    }
    printf(">"); 
    break;

  case limo_TYPE_ENV: 
    if (in_env)
      printf("#<env>");
    else {
      in_env++;
      printf("#<env:"); writer(ld); printf(">");
      in_env--;
    }
    break;

  case limo_TYPE_SPECIAL: writer_special(ld); break;
  case limo_TYPE_SPECIAL_INTERN: writer_special_intern(ld); break;
  case limo_TYPE_VCACHE: printf("<"); writer(CDR(ld)); printf(">"); break;

  default:
    printf("**BROKEN DATA(%i)**", ld->type);
    break;
  }
}

void l_writer(limo_data ***dest, limo_data *ld) // not threadsafe!
{
  static int in_env=0;

  switch (ld->type) {
  case limo_TYPE_STRING: l_string_writer(dest, ld); break;
  case limo_TYPE_GMPQ: l_number_writer(dest, ld); break;
  case limo_TYPE_CONS: l_cons_writer(dest, ld); break;
  case limo_TYPE_SYMBOL: list_put_str(dest, ld->data.d_string); break;
  case limo_TYPE_BUILTIN:
    {
      char buf[20];
      list_put_str(dest, "#<builtin:");
      snprintf(buf, 20, "%p", ld->data.d_builtin);
      list_put_str(dest, buf);
      list_put_str(dest, ">");
    }
    break;
  case limo_TYPE_DICT:
    if (in_env)
      list_put_str(dest, "#<dict>");
    else {
      in_env++;
      list_put_str(dest, "#<dict: "); l_writer(dest, dict_to_list(ld)); list_put_str(dest, ">");
      in_env--;
    }
    break;

  case limo_TYPE_CONST:
    list_put_str(dest, "[");
    list_put_str(dest, CDR(ld)->data.d_string);
    list_put_str(dest, "]");
    break;

  case limo_TYPE_THUNK:
  case limo_TYPE_LAMBDA:
  case limo_TYPE_MACRO:
    list_put_str(dest, (ld->type==limo_TYPE_LAMBDA?"#<lambda":
		     (ld->type==limo_TYPE_MACRO?"#<macro":
		      "#<thunk")));
    l_writer(dest, CDR(ld));
    if (CAR(ld) != globalenv) {
      list_put_str(dest, ", env: ");
      l_writer(dest, CAR(ld));
    }
    list_put_str(dest, ">");
    break;

  case limo_TYPE_ENV:
    if (in_env)
      list_put_str(dest, "#<env>");
    else {
      in_env++;
      list_put_str(dest, "#<env:"); l_writer(dest, ld); list_put_str(dest, ">"); 
      in_env--;
    }
    break;

  case limo_TYPE_SPECIAL: l_writer_special(dest, ld); break;
  case limo_TYPE_SPECIAL_INTERN: l_writer_special_intern(dest, ld); break;
  case limo_TYPE_VCACHE: list_put_str(dest, "<"); l_writer(dest, CDR(ld)); list_put_str(dest, ">"); break;

  default:
    {
      char buf[20];
      snprintf(buf, 20, "%i", ld->type);
      list_put_str(dest, "**BROKEN DATA(");
      list_put_str(dest, buf);
      list_put_str(dest, "**");
    }
    break;
  }
}
