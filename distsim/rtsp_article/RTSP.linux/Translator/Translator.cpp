#include <Parser.h>

//#include <crtdbg.h>

int main(int argc, char *argv[])
{
#if 0
    // Set heap debug flag
    (void) _CrtSetDbgFlag(
        _CRTDBG_ALLOC_MEM_DF |
        //_CRTDBG_LEAK_CHECK_DF |
        _CRTDBG_CHECK_ALWAYS_DF);
#endif
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s filename.mdf\n", argv[0]);
        exit(1);
    }

    ParseMDFFile(argv[1]);

    GenerateCppCode();

    return 0;
}

