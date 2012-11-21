#include <stdio.h>
#include "limo.h"

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

void string_writer(limo_data *ld)
{
  char *s = ld->data.d_string;
  printf("\"");
  while (*s) {
    switch (*s) {
    case '\n': printf("\\n"); break;
    case '\\': printf("\\"); break;
    case '"': printf("\""); break;
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

void writer(limo_data *ld)
{
  switch (ld->type) {
  case limo_TYPE_STRING: string_writer(ld); break;
  case limo_TYPE_GMPQ: number_writer(ld); break;
  case limo_TYPE_CONS: cons_writer(ld); break;
  case limo_TYPE_SYMBOL: printf("%s", ld->data.d_string); break;
  case limo_TYPE_BUILTIN: printf("#<builtin:%p>", ld->data.d_builtin); break;
  case limo_TYPE_LAMBDA: printf("#<lambda:"); writer(CDR(ld->data.d_lambda)); printf(">"); break;
  case limo_TYPE_MACRO: printf("#<macro:"); writer(CDR(ld->data.d_lambda)); printf(">"); break;
  case limo_TYPE_ENV: printf("#<env:"); writer(ld->data.d_env); printf(">"); break;
  case limo_TYPE_EAGAIN: printf("#<eagain:"); writer(CAR(ld->data.d_eagain)); printf(">"); break;
  }
}
