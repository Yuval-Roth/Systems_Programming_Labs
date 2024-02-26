
#include "stdio.h"
int getInt(char *c);
int main(int argc, char** argv) {
    printf("%d\n",getInt(argv[1]));
    return getInt(argv[1]);
}
int getInt(char *c) {
    int count = 0;
    while (*c != '\0') {
        if (*c >= '0' && *c <= '9') {
            count++;
        }
        c++;
    }
    return count;
}
