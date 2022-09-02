#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_todo(int todo_length, char** todo, char* todo_file);

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Need at least one argument!\n");
        return -1;
    }

    add_todo(argc, argv, "test.txt");

    return 0;
}

void add_todo(int todo_length, char** todo, char* todo_file) {
    FILE* fp;
    fp = fopen(todo_file, "a+");
    char long_todo[4096] = { '\0' };

    if (long_todo == NULL) {
        printf("Cannot get todo\n");
        return;
    }

    if (todo_length > 2) {
        for (int i = 2; i < todo_length; i++) {
            if (long_todo == 0) {
                return;
            }
            else {
                strncat(long_todo, todo[i], sizeof(long_todo) - 1);
                strncat(long_todo, " ", sizeof(long_todo) - 1);
            }
        }
    }
    else {
        if (long_todo == 0) {
            return;
        }
        else {
            strncat(long_todo, todo[2], sizeof(long_todo) - 1);
        }
    }
    strncat(long_todo, "\n", sizeof(long_todo) - 1);

    long_todo[4095] = '\0';

    fprintf(fp, long_todo);
    fclose(fp);
}
