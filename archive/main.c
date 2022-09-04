#define GREEN "\033[32m"
#define RESET "\033[0m"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void add_todo(int todo_length, char** todo, char* todo_file);
void list_todos(char* todo_file);
void delete_todo(int todo_num, char* todo_file);
void update_todo(int todo_num, int todo_length, char** new_todo, char* todo_file);
void complete_todo(int todo_num, char* todo_file);
int get_lines_in_file(char* todo_file);

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Need at least one argument!\n");
        return -1;
    }

    char* homedir = getenv("HOME");
    char tododir[4096];
    char* int_conv;
    struct stat st = {0};

    strncpy(tododir, homedir, sizeof(tododir) - 1);
    tododir[4095] = '\0';
    strncat(tododir, "/.todo", sizeof(homedir) - 1);

    if (stat(tododir, &st) != 0) {
        mkdir(tododir, 0744);
    }

    strncat(tododir, "/todo", sizeof(tododir) - 1);


    if (strncmp(argv[1], "add", sizeof(argv[1]) - 1) == 0) {
        add_todo(argc, argv, tododir);
    }
    else if (strncmp(argv[1], "list", sizeof(argv[1]) - 1) == 0) {
        list_todos(tododir);
    }
    else if (strncmp(argv[1], "del", sizeof(argv[1]) - 1) == 0) {
        int todo_number = strtol(argv[2], &int_conv, sizeof(argv[2]));
        delete_todo(todo_number, tododir);
    }
    else if (strncmp(argv[1], "update", sizeof(argv[1]) - 1) == 0) {
        int todo_number = strtol(argv[2], &int_conv, sizeof(argv[2]));
        update_todo(todo_number, argc, argv, tododir);
    }
    else if (strncmp(argv[1], "done", sizeof(argv[1]) - 1) == 0) {
        int todo_number = strtol(argv[2], &int_conv, sizeof(argv[2]));
        complete_todo(todo_number, tododir);
    }
    else {
        printf("Invalid option\n");
        return -1;
    }


    return 0;
}

void add_todo(int todo_length, char** todo, char* todo_file) {
    FILE* fp;
    fp = fopen(todo_file, "a+");
    char long_todo[4096] = { '\0' };

    if (todo_length > 2) {
        for (int i = 2; i < todo_length; i++) {
            strncat(long_todo, todo[i], sizeof(long_todo) - 1);
            strncat(long_todo, " ", sizeof(long_todo) - 1);
        }
    }
    else {
        strncat(long_todo, todo[2], sizeof(long_todo) - 1);
    }
    strncat(long_todo, "\n", sizeof(long_todo) - 1);

    long_todo[4095] = '\0';

    fprintf(fp, long_todo);
    fclose(fp);
}

void list_todos(char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    FILE* fp = fopen(todo_file, "r");
    int count = 1;
    char todo[4096];

    printf("Todo List\n");
    printf("---------\n");

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), fp) != NULL) {
            if (strstr(todo, "✓")) {
                printf(GREEN "%d. %s" RESET, count, todo);
            }
            else {
                printf("%d. %s", count, todo);
            }
        }
        count++;
    }

    fclose(fp);
}

void delete_todo(int todo_num, char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    int counter = 1;
    char todo[4096];
    FILE* fp = fopen(todo_file, "r");
    FILE* tmp = tmpfile();

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), fp) != NULL && todo_num != counter) {
            fprintf(tmp, todo);
        }
        counter++;
    }

    fclose(fp);

    FILE* f = fopen(todo_file, "w+");

    rewind(tmp);
    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(f, todo);
        }
    }
    fclose(f);
    fclose(tmp);
}

void update_todo(int todo_num, int todo_length, char** new_todo, char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    int counter = 1;
    char todo[4096];
    char long_todo[4096] = { '\0' };
    FILE* fp = fopen(todo_file, "r");
    FILE* tmp = tmpfile();

    if (todo_length > 3) {
        for (int i = 3; i < todo_length; i++) {
            strncat(long_todo, new_todo[i], sizeof(long_todo) - 1);
            strncat(long_todo, " ", sizeof(long_todo) - 1);
        }
    }
    else {
        strncat(long_todo, new_todo[3], sizeof(long_todo) - 1);
    }
    strncat(long_todo, "\n", sizeof(long_todo) - 1);
    long_todo[4095] = '\0';

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), fp) != NULL) {
            if (todo_num != counter) {
                fprintf(tmp, todo);
            }
            else {
                fprintf(tmp, long_todo);
            }
            counter++;
        }
    }
    fclose(fp);

    FILE* f = fopen(todo_file, "w+");
    rewind(tmp);

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(f, todo);
        }
    }
    fclose(f);
    fclose(tmp);
}

void complete_todo(int todo_num, char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    int counter = 1;
    char todo[4096];
    FILE* fp = fopen(todo_file, "r");
    FILE* tmp = tmpfile();

    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    if (tmp == NULL) {
        printf("Error creating tempfile\n");
        return;
    }

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), fp) != NULL) {
            if (todo_num == counter) {
                todo[strlen(todo) - 1] = '\0';
                strncat(todo, " ✓\n", sizeof(todo) - 1);
                fprintf(tmp, todo);
            }
            else {
                fprintf(tmp, todo);
            }
            counter++;
        }
    }

    todo[4095] = '\0';

    fclose(fp);

    FILE* f = fopen(todo_file, "w+");
    rewind(tmp);

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(f, todo);
        }
    }

    fclose(f);
    fclose(tmp);
}

int get_lines_in_file(char* todo_file) {
    FILE* fp = fopen(todo_file, "r");
    int count = 0;

    for (char c = getc(fp); c != EOF; c = getc(fp)) {
        if (c == '\n') {
            count++;
        }
    }

    fclose(fp);

    return count;
}
