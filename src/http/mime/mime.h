#ifndef MIME_H_INCLUDED
#define MIME_H_INCLUDED

#include <stdio.h>
#include <string.h>

#include "utils.h"

const char* detect_mime_type(char *filename, FILE *fd);

#endif // MIME_H_INCLUDED
