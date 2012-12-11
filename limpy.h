char limpy_read_skip_space_comments(reader_stream *f);
void limpy_ungettoken(int token, limo_data *data);
int limpy_gettoken(reader_stream *rs, limo_data **ld);
limo_data *limpy_param_reader(reader_stream *rs);
limo_data *limpy_list_reader(reader_stream *rs, int right_bracket);
limo_data *limpy_expr_reader(reader_stream *rs);
limo_data *limpy_assignment_reader(reader_stream *rs);
limo_data *limpy_statement_reader(reader_stream *rs);
limo_data *limpy_block_reader(reader_stream *rs);

