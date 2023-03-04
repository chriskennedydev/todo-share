#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void add_todo(int todo_length, char **todo, char *todo_file);
void list_todos(char *todo_file);
void delete_todo(int todo_num, char *todo_file);
void update_todo(int todo_num, int todo_length, char **new_todo, char *todo_file);
void complete_todo(int todo_num, char *todo_file);
int get_lines_in_file(char *todo_file);
void usage();

int main(int argc, char **argv) {
    if (argc == 1) {
        usage();
        return -1;
    }

    char *homedir = getenv("HOME");
    char tododir[4096];
    char *int_conv;
    struct stat st = {0};
    time_t now;
    struct tm *clock;
    char time_string[64];

    time(&now);
    clock = localtime(&now);

    strftime(time_string, sizeof(time_string), "%A, %B %d, %Y -- %r%n", clock);

    strncpy(tododir, homedir, sizeof(tododir) - 1);
    strncat(tododir, "/.todo", sizeof(tododir) - 1);

    if (stat(tododir, &st) != 0) {
        mkdir(tododir, 0744);
    }

    strncat(tododir, "/todo", sizeof(tododir) - 1);
    tododir[4095] = '\0';

    char *cmd = argv[1];

    if (strncmp(cmd, "add", strlen(cmd)) == 0) {
        add_todo(argc, argv, tododir);
    }
    else if (strncmp(cmd, "list", strlen(cmd)) == 0) {
        printf("%s", time_string);
        list_todos(tododir);
    }
    else if (strncmp(cmd, "del", strlen(cmd)) == 0) {
        if (argc < 3) {
            usage();
            return -1;
        }
        char *num = argv[2];
        int todo_number = strtol(num, &int_conv, 0);
        delete_todo(todo_number, tododir);
    }
    else if (strncmp(cmd, "update", strlen(cmd)) == 0) {
        if (argc < 4) {
            usage();
            return -1;
        }
        char *num = argv[2];
        int todo_number = strtol(num, &int_conv, 0);
        update_todo(todo_number, argc, argv, tododir);
    }
    else if (strncmp(cmd, "done", strlen(cmd)) == 0) {
        if (argc < 3) {
            usage();
            return -1;
        }
        char *num = argv[2];
        int todo_number = strtol(num, &int_conv, 0);
        complete_todo(todo_number, tododir);
    }
    else {
        usage();
    }

    return 0;
}

void add_todo(int todo_length, char **todo, char *todo_file) {
    FILE *fp;
    fp = fopen(todo_file, "a+");
    char long_todo[4096] = { '\0' };

    if (fp == NULL) {
        perror(RED "File not found" RESET);
        return;
    }

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

    fprintf(fp, "%s", long_todo);
    fclose(fp);
}

void list_todos(char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    if (lines == -1) {
        perror(RED "Need to create a todo first" RESET);
        return;
    }
    FILE *fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror(RED "Unable to open file" RESET);
    }
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

void delete_todo(int todo_num, char *todo_file) {
    int lines = get_lines_in_file(todo_file);
    if (lines == -1) {
        perror(RED "Need to create a todo first" RESET);
        return;
    }
    int counter = 1;
    char todo[4096];
    FILE *fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror(RED "Unable to open file" RESET);
        return;
    }
    FILE *tmp = tmpfile();

    if (tmp == NULL) {
        perror(RED "Unable to create temp file" RESET);
        fclose(fp);
        return;
    }

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), fp) != NULL) {
            if (todo_num != counter) {
                fprintf(tmp, "%s", todo);
            }
        }
        counter++;
    }

    fclose(fp);

    FILE *rf = fopen(todo_file, "w+");

    if (rf == NULL) {
        perror(RED "Unable to open file" RESET);
        fclose(tmp);
        return;
    }

    rewind(tmp);
    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(rf, "%s", todo);
        }
    }
    fclose(rf);
    fclose(tmp);
}

void update_todo(int todo_num, int todo_length, char **new_todo, char *todo_file) {
    int lines = get_lines_in_file(todo_file);
    if (lines == -1) {
        perror(RED "Need to create a todo first" RESET);
        return;
    }
    int counter = 1;
    char todo[4096];
    char long_todo[4096] = { '\0' };
    FILE *fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror(RED "Unable to open file" RESET);
        return;
    }
    FILE *tmp = tmpfile();

    if (tmp == NULL) {
        perror(RED "Unable to open temp file" RESET);
        fclose(fp);
        return;
    }

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
                fprintf(tmp, "%s", todo);
            }
            else {
                fprintf(tmp, "%s", long_todo);
            }
            counter++;
        }
    }
    fclose(fp);

    FILE *rf = fopen(todo_file, "w+");
    if (rf == NULL) {
        perror(RED "Unable to open file" RESET);
        fclose(tmp);
        return;
    }

    rewind(tmp);

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(rf, "%s", todo);
        }
    }
    fclose(rf);
    fclose(tmp);
}

void complete_todo(int todo_num, char *todo_file) {
    int lines = get_lines_in_file(todo_file);
    if (lines == -1) {
        perror(RED "Need to create a todo first" RESET);
        return;
    }
    int counter = 1;
    char todo[4096];
    FILE *fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror(RED "Error opening file" RESET);
        return;
    }
    FILE *tmp = tmpfile();

    if (tmp == NULL) {
        perror(RED "Unable to create temp file" RESET);
        fclose(fp);
        return;
    }

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), fp) != NULL) {
            if (todo_num == counter) {
                todo[strlen(todo) - 1] = '\0';
                strncat(todo, "✓\n", sizeof(todo) - 1);
                fprintf(tmp, "%s", todo);
            }
            else {
                fprintf(tmp, "%s", todo);
            }
            counter++;
        }
    }

    todo[4095] = '\0';

    fclose(fp);

    FILE *rf = fopen(todo_file, "w+");
    if (rf == NULL) {
        perror(RED "Error opening file" RESET);
        fclose(tmp);
        return;
    }

    rewind(tmp);

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(rf, "%s", todo);
        }
    }

    fclose(rf);
    fclose(tmp);
}

int get_lines_in_file(char *todo_file) {
    FILE *fp = fopen(todo_file, "r");
    int count = 0;

    if (fp == NULL) {
        return -1;
    }

    for (char c = getc(fp); c != EOF; c = getc(fp)) {
        if (c == '\n') {
            count++;
        }
    }

    fclose(fp);

    return count;
}

void usage() {
    printf("todo: todo cmd item(s)\n");
    printf("cmd: add | del | done | list | update\n");
    printf("add: todo add my todo\n");
    printf("del: todo del 2\n");
    printf("done: todo done 1\n");
    printf("update: todo update 2 new todo\n");
    printf("list: todo list\n");
}

