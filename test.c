#include <stdio.h>
#include <stdlib.h>

int main() {
    int ret = system("short_path");
    if (ret == 0) {
        printf("Test: OK\n");
    } else {
        printf("Test failed: exit code %d\n", ret);
    }
    return 0;
}
