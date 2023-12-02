#include "mime.h"

const char* detect_mime_type(char *filename, FILE *fd)
{
    char *ext = strrchr(filename, '/');
    if (ext == NULL)
    {
        ext = filename;
    }

    ext = strrchr(filename, '.');
    if (ext == NULL)
    {
        return "text/html";
    }

    ext++;
    if (stricmp(ext, "jpeg") == 0 || stricmp(ext, "jpg") == 0)
    {
        return "image/jpeg";
    }
    if (stricmp(ext, "gif") == 0)
    {
        return "image/gif";
    }
    if (stricmp(ext, "png") == 0)
    {
        return "image/png";
    }
    if (stricmp(ext, "js") == 0)
    {
        return "application/javascript";
    }
    if (stricmp(ext, "css") == 0)
    {
        return "text/css";
    }
    if (stricmp(ext, "txt") == 0)
    {
        return "text/plain";
    }
    if (stricmp(ext, "pdf") == 0)
    {
        return "application/pdf";
    }
    if (stricmp(ext, "swf") == 0)
    {
        return "application/x-shockwave-flash";
    }

    return "text/html";
}
