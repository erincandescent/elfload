typedef int (*puts_t)(const char *s);
puts_t xputs;
puts_t *pxputs = &xputs;


void _start(puts_t p)
{
    *pxputs = p;

    (*pxputs)("Hello, world!");
}
