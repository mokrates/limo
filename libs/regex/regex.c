#include <regex.h>
#include <sys/types.h>
#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

static limo_data *regex_error_sym;

/* static char *regex_error(int error) { */
/*   switch (error) { */
/*   case REG_NOMATCH: return "REG_NOMATCH"; */
/*   case REG_BADPAT: return "REG_BADPAT"; */
/*   case REG_ECOLLATE: return "REG_ECOLLATE"; */
/*   case REG_ECTYPE: return "REG_ECTYPE"; */
/*   case REG_EESCAPE: return "REG_EESCAPE"; */
/*   case REG_ESUBREG: return "REG_ESUBREG"; */
/*   case REG_EBRACK: return "REG_EBRACK"; */
/*   case REG_EPAREN: return "REG_EPAREN"; */
/*   case REG_EBRACE: return "REG_EBRACE"; */
/*   case REG_BADBR: return "REG_BADBR"; */
/*   case REG_ERANGE: return "REG_ERANGE"; */
/*   case REG_ESPACE: return "REG_ESPACE"; */
/*   case REG_BADRPT: return "REG_BADRPT"; */
/*   default: return "REG_UNDEFINED"; */
/*   } */
/* } */

static void regex_error(int errcode, regex_t *preg)
{
  size_t errlen;
  char *errmsg;
  errlen = regerror(errcode, preg, NULL, 0);
  errmsg = (char *)GC_malloc(errlen + 1);
  regerror(errcode, preg, errmsg, errlen+1);
  regfree(preg);
  throw(make_cons(regex_error_sym, make_string(errmsg)));
}
		
BUILTIN(builtin_regex_match)
{
  limo_data *ldpattern;
  limo_data *ldstring;
  limo_data *ldngroups;
  limo_data *ldcflags;
  limo_data *ldeflags;
  limo_data *result;
  int ngroups=0;
  int cflags=0;
  int eflags=0;
  regex_t preg;
  regmatch_t *pmatch;
  int ret;
  
  REQUIRE_ARGC("REGEX-MATCH", 5);
  ldpattern = eval(FIRST_ARG, env);
  ldstring  = eval(SECOND_ARG, env);
  ldngroups = eval(THIRD_ARG, env);
  ldcflags = eval(FOURTH_ARG, env);
  ldeflags = eval(FIFTH_ARG, env);
  
  if (ldcflags->type != limo_TYPE_GMPQ ||
      ldeflags->type != limo_TYPE_GMPQ ||
      ldpattern->type != limo_TYPE_STRING ||
      ldstring->type != limo_TYPE_STRING)
      limo_error("(REGEX-MATCH pattern string ngroups cflags eflags)");

  cflags = GETINTFROMMPQ(ldcflags);
  eflags = GETINTFROMMPQ(ldeflags);
  ngroups= GETINTFROMMPQ(ldngroups);
  pmatch = GC_malloc(ngroups * sizeof (regmatch_t));

  if (ret = regcomp(&preg, ldpattern->d_string, cflags))
    regex_error(ret, &preg);
  if (ret = regexec(&preg, ldstring->d_string, ngroups, pmatch, eflags))
    result = nil;
  else {
    int i;
    limo_data **cursor;
    cursor = &result;
    for (i=0; i<ngroups; ++i) {
      limo_data *match;
      if (pmatch[i].rm_so == -1) {
        match = nil;
      } else {
        int matchlen = pmatch[i].rm_eo - pmatch[i].rm_so;
        match = make_limo_data();
        match->type = limo_TYPE_STRING;
        match->d_string = (char *)GC_malloc(matchlen + 1);
        memcpy(match->d_string, ldstring->d_string + pmatch[i].rm_so, matchlen);
        match->d_string[matchlen] = '\0';
        match->hash = matchlen;
      }
      (*cursor) = make_cons(match, NULL);
      cursor = &CDR(*cursor);
    }
    (*cursor) = make_nil();
  }

  regfree(&preg);
  
  return result;
}

void limo_init_regex(limo_data *env)
{
  struct { char *name; int flag; }
  flaglist[] = {
		{ "REG_EXTENDED", REG_EXTENDED},
		{ "REG_ICASE", REG_ICASE},
		{ "REG_NOSUB", REG_NOSUB},
		{ "REG_NEWLINE", REG_NEWLINE},
		{ "REG_NOTBOL", REG_NOTBOL},
		{ "REG_NOTEOL", REG_NOTEOL}
  };
  limo_data *flagdict = make_dict();
  int i;
  for (i=0; i<sizeof flaglist / sizeof flaglist[0]; ++i)
    dict_put(flagdict, make_sym(flaglist[i].name), make_rational_from_long_long(flaglist[i].flag));

  setq(env, make_sym("REGEX-CONST"), flagdict);
  INSBUILTIN(builtin_regex_match, "REGEX-MATCH");

  regex_error_sym = make_sym("REGEX-ERROR");
}
