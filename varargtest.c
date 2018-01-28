

#include <stdarg.h>
#include <stdio.h>

static void bar(int firstarg, int first, int second, va_list argp)
{
    printf("%d, %d, %d, %d, %d, %d\n", firstarg, first, second, va_arg(argp, int), va_arg(argp, int), va_arg(argp, int));
}


static void foo(int firstarg, ...)
{
    if (!firstarg)
    {
        return;
    }
    else
    {
        va_list argp;

        va_start(argp, firstarg);
        printf("%p\n", argp);
        bar(firstarg, 5, 6, argp);
        va_end(argp);
    }
}

void main(void)
{
    foo(1, 2, 3, 4);
}