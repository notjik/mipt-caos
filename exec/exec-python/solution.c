#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char expression[1024];

    if (!fgets(expression, sizeof(expression), stdin)) {
        return 0;
    }

    expression[strcspn(expression, "\n")] = '\0';

    if (strlen(expression) == 0) {
        return 0;
    }

    FILE *f = fopen("prog.py", "w");
    if (!f) {
        perror("fopen");
        return 1;
    }

    fprintf(f,
        "print(eval('%s'))",
        expression
    );
    fclose(f);

    if (system("python3 prog.py") != 0) {
        system("rm -f prog.py");
        return 1;
    }
    system("rm -f prog.py");

    return 0;
}