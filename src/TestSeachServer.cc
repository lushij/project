#include "SearchEngine.h"

void test()
{
    SearchEngine server(4, 10, "0.0.0.0", 8888);
    server.start();
}

int main(int argc, char *argv[])
{
    test();
    return 0;
}

