typedef struct LIMO_MARSHAL_DATA {
  GType expected_return_type;
} limo_marshal_data;

void limo_closure_marshal(GClosure *closure, GValue *return_value, guint n_param_values, const GValue *param_values, gpointer invocation_hint, gpointer marshal_data)
{
  int i;
  limo_data *arglist, **next_arg, *arg, **darg, *result;
  limo_marshal_data *lmd = (limo_marshal_data *)marshal_data;
  limo_data *lcd = closure->data;  // limo closure data

  next_arg = &arglist;
  for (i=0; i<n_param_values; ++i){ 
   switch (G_VALUE_TYPE(&param_values[i])) {
    case G_TYPE_CHAR:
      arg = make_rational_from_long_long(g_value_get_char(&param_values[i]));
      break;

    case G_TYPE_UCHAR:
      arg = make_rational_from_long_long(g_value_get_uchar(&param_values[i]));
      break;

    case G_TYPE_BOOLEAN:
      arg = make_rational_from_long_long(g_value_get_boolean(&param_values[i]));
      break;

    case G_TYPE_INT:
      arg = make_rational_from_long_long(g_value_get_int(&param_values[i]));
      break;

    case G_TYPE_UINT:
      arg = make_rational_from_long_long(g_value_get_uint(&param_values[i]));
      break;

    case G_TYPE_LONG:
      arg = make_rational_from_long_long(g_value_get_long(&param_values[i]));
      break;

    case G_TYPE_ULONG:
      arg = make_rational_from_long_long(g_value_get_ulong(&param_values[i]));
      break;

    case G_TYPE_INT64:
      arg = make_rational_from_long_long(g_value_get_int64(&param_values[i]));
      break;

    case G_TYPE_UINT64:
      arg = make_rational_from_long_long(g_value_get_uint64(&param_values[i]));
      break;

    case G_TYPE_ENUM:
      arg = make_rational_from_long_long(g_value_get_enum(&param_values[i]));
      break;

    case G_TYPE_FLAGS:
      arg = make_rational_from_long_long(g_value_get_flags(&param_values[i]));
      break;

    case G_TYPE_FLOAT:
    case G_TYPE_DOUBLE:
      limo_error("marshaling floats is unimplemented yet");
      break;

    case G_TYPE_STRING:
      arg = make_string(g_value_get_string(&param_values[i]));
      break;

    case G_TYPE_POINTER:
      arg = make_special(sym_limogtk_gpointer, g_value_get_pointer(&param_values[i]));
      break;

    default:
      if (g_value_type_compatible(G_VALUE_TYPE(&param_values[i]), G_TYPE_OBJECT))
        arg = make_special(sym_limogtk_gpointer, g_value_get_object(&param_values[i]));
      else
        limo_error("tried to marshal unknown datatype: %s in %ith argument", G_VALUE_TYPE_NAME(&param_values[i]), i);
    }

    (*next_arg) = make_cons(arg, nil);
    next_arg = &CDR(*next_arg);
  }

  result = eval(make_cons(CAR(lcd), arglist), CDR(lcd));

  /* if (return_value) */
  /*   printf("marshal: a return_value seems to be excepted... - return_value has type %s\n", G_VALUE_TYPE_NAME(return_value)); */
  //  return_value = irgendwie_casten(result, lmd->expected_return_type);
}

GClosure *limogtk_closure_new(limo_data *ld_closure)
{
  GClosure *lcl;

  lcl = (GClosure *)g_closure_new_simple(sizeof (GClosure), ld_closure);
  g_closure_set_marshal(lcl, limo_closure_marshal);
  return lcl;
}

BUILTIN(builtin_g_signal_connect)
{
  limo_data *ld_instance, *ld_signal, *ld_handler, *ld_after, *ld_closure;
  GClosure *closure;

  REQUIRE_ARGC("G-SIGNAL-CONNECT", 4);
  ld_instance = eval(FIRST_ARG, env);
  ld_signal   = eval(SECOND_ARG, env);
  ld_handler  = eval(THIRD_ARG, env);
  ld_after    = eval(FOURTH_ARG, env);

  REQUIRE_TYPE("G-SIGNAL-CONNECT", ld_signal, limo_TYPE_STRING);
  REQUIRE_TYPE("G-SIGNAL-CONNECT", ld_handler, limo_TYPE_LAMBDA);
  REQUIRE_TYPE("G-SIGNAL-CONNECT", ld_instance, limo_TYPE_SPECIAL);

  ld_closure = make_cons(ld_handler, env);  
  closure = limogtk_closure_new(ld_closure);
    
  if (make_rational_from_long_long(g_signal_connect_closure(G_OBJECT(CDR(ld_instance->d_special) -> d_special_intern),
                                                          ld_signal->d_string,
                                                          closure,
                                                          !is_nil(ld_after))))
    return ld_closure;
  else
    return nil;
}
