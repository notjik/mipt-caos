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

    FILE *f = fopen("prog.c", "w");
    if (!f) {
        perror("fopen");
        return 1;
    }

    fprintf(f,
        "#include <stdio.h>\n"
        "int main() {\n"
        "    int result = (int)(%s);\n"
        "    printf(\"%%d\\n\", result);\n"
        "    return 0;\n"
        "}\n",
        expression
    );
    fclose(f);

    if (system("gcc -o bin prog.c") != 0) {
        system("rm -f prog.c");
        return 1;
    }
    system("rm -f prog.c");

    if (system("./bin") != 0) {
        system("rm -f bin");
        return 1;
    }
    system("rm -f bin");

    return 0;
}