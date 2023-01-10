#include <limo.h>

#define INS_DATE_BUILTIN(f, name) setq(limo_date_env, make_sym(name), make_builtin(f))
#define INS_DATE_VAR(val, name)   setq(limo_date_env, make_sym(name), val)

limo_data *sym_date;

#ifdef __MINGW32__
struct tm *localtime_r(time_t *tt, struct tm *tm)
{
  memcpy(tm, localtime(tt), sizeof (struct tm));
  return tm;
}
#endif

limo_data *structtm2ld(struct tm *tm)
{
  limo_data *ld;
  ld = make_cons(make_number_from_long_long(tm->tm_isdst), nil);
  ld = make_cons(make_number_from_long_long(tm->tm_yday), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_wday), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_year), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_mon), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_mday), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_hour), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_min), ld);
  ld = make_cons(make_number_from_long_long(tm->tm_sec), ld);
  return ld;
}

struct tm *ld2structtm(limo_data *ld)
{
  struct tm *tm;
  tm = (struct tm *)GC_malloc_atomic(sizeof (struct tm));
  tm->tm_sec = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_min = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_hour = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_mday = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_mon = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_year = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_wday = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_yday = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  tm->tm_isdst = GETINTFROMMPQ(CAR(ld)); ld=CDR(ld);
  return tm;
}

BUILTIN(builtin_date_localtime)
{
  limo_data *ld_unixtime;
  struct tm *tm;
  time_t t;

  REQUIRE_ARGC("LOCALTIME", 1);
  ld_unixtime = eval(FIRST_ARG, env);
  
  tm = (struct tm *)GC_malloc_atomic(sizeof (struct tm));
  t = GETINTFROMMPQ(ld_unixtime);
  return structtm2ld(localtime_r(&t, tm));
}

BUILTIN(builtin_date_strftime)
{
  limo_data *ld_tm_time, *ld_format;
  struct tm *tm;
  char *out;

  REQUIRE_ARGC("STRFTIME", 2);
  ld_format = eval(FIRST_ARG, env);
  ld_tm_time = eval(SECOND_ARG, env);

  out = (char *)GC_malloc_atomic(200);
  tm = ld2structtm(ld_tm_time);
  strftime(out, 200, ld_format->d_string, tm);
  return make_string(out);
}

void limo_init_date(limo_data *env)
{
  limo_data *limo_date_env;
  sym_date = make_sym("date");

  limo_date_env = make_env(nil);
  
  INS_DATE_BUILTIN(builtin_date_localtime, "LOCALTIME");
  INS_DATE_BUILTIN(builtin_date_strftime, "STRFTIME");

  setq(env, make_sym("_DATE"), limo_date_env);
}
