#ifndef CONFIG_PARSER_H_INCLUDED
#define CONFIG_PARSER_H_INCLUDED

char *ltrim(char *in);

char *consume(char *in, char *str);

char *read_ident(char* in, char * out, int limit);

char *read_string(char *in, char *out, int limit);

char *read_int(char *in, int*out);


#endif // CONFIG_PARSER_H_INCLUDED
