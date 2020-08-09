#ifndef DJB2
#define DJB2
unsigned long
djb2(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
	hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
#endif
