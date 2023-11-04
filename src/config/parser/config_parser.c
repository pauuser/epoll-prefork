#include "config_parser.h"
#include "../../utils/utils.h"


int is_alphanumeric(char c)
{
    return c >= '0' && c <= '9' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

char *consume(char *in, char *str)
{
    while (*str)
    {
        if (*str++ != *in++)
        {
            die_with_error("parse error");
        }
    }
    return in;
}

char *read_ident(char* in, char * out, int limit)
{
    in = ltrim(in);
    int normal_break = 0;
    while(*in != 0 && --limit >= 0)
    {
        if (is_alphanumeric(*in))
        {
            *out = *in; out++; in++;
        }
        else
        {
            normal_break = 1;
            break;
        }
    }

    if (!normal_break && *in && limit >= 0)
    {
        die_with_error("too long token");
    }

    *out = 0;
    return in;
}

char *read_string(char *in, char *out, int limit)
{
    in = ltrim(in);
    in = consume(in, "\"");
    while(*in && *in != '"' && --limit >= 0)
    {
        *out++ = *in++;
    }
    *out = '\0';
    in = consume(in, "\"");

    return in;
}

char *read_int(char *in, int*out)
{
    *out = 0;
    in = ltrim(in);
    while(*in)
    {
        char curr = *in;
        if (curr >= '0' && curr <= '9')
        {
            *out *= 10;
            *out += curr - '0';
            in++;
        }
        else
        {
            break;
        }
    }

    return in;
}
