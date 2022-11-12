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
void clear_screen();
int get_lines_in_file(char* todo_file);
void usage();

int main(int argc, char** argv) {
    if (argc == 1) {
	usage();
        return -1;
    }

    char* homedir = getenv("HOME");
    char tododir[4096];
    char* int_conv;
    struct stat st = {0};

    strncpy(tododir, homedir, sizeof(tododir) - 1);
    strncat(tododir, "/.todo", sizeof(homedir) - 1);

    if (stat(tododir, &st) != 0) {
        mkdir(tododir, 0744);
    }

    strncat(tododir, "/todo", sizeof(tododir) - 1);
    tododir[4095] = '\0';


    if (strncmp(argv[1], "add", sizeof(argv[1]) - 1) == 0) {
        add_todo(argc, argv, tododir);
    }
    else if (strncmp(argv[1], "list", sizeof(argv[1]) - 1) == 0) {
        list_todos(tododir);
    }
    else if (strncmp(argv[1], "del", sizeof(argv[1]) - 1) == 0) {
	if (argc < 3) {
	    usage();
	    return -1;
	}
        int todo_number = strtol(argv[2], &int_conv, 0);
        delete_todo(todo_number, tododir);
    }
    else if (strncmp(argv[1], "update", sizeof(argv[1]) - 1) == 0) {
	if (argc < 4) {
	    usage();
	    return -1;
	}
        int todo_number = strtol(argv[2], &int_conv, 0);
        update_todo(todo_number, argc, argv, tododir);
    }
    else if (strncmp(argv[1], "done", sizeof(argv[1]) - 1) == 0) {
	if (argc < 3) {
	    usage();
	    return -1;
	}
        int todo_number = strtol(argv[2], &int_conv, 0);
        complete_todo(todo_number, tododir);
    }
    else {
	usage();
    }


    return 0;
}

void add_todo(int todo_length, char** todo, char* todo_file) {
    FILE* fp;
    fp = fopen(todo_file, "a+");
    char long_todo[4096] = { '\0' };

    if (fp == NULL) {
        perror("File not found");
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
        perror("Need to create a todo first");
        return;
    }
    FILE* fp = fopen(todo_file, "r");
    if (fp == NULL) {
	perror("Unable to open file");
    }
    int count = 1;
    char todo[4096];


    system("clear");
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
    if (lines == -1) {
        perror("Need to create a todo first");
        return;
    }
    int counter = 1;
    char todo[4096];
    FILE* fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }
    FILE* tmp = tmpfile();

    if (tmp == NULL) {
        perror("Unable to create temp file");
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

    FILE* f = fopen(todo_file, "w+");

    if (f == NULL) {
        perror("Unable to open file");
        fclose(tmp);
        return;
    }

    rewind(tmp);
    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(f, "%s", todo);
        }
    }
    fclose(f);
    fclose(tmp);
}

void update_todo(int todo_num, int todo_length, char** new_todo, char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    if (lines == -1) {
        perror("Need to create a todo first");
        return;
    }
    int counter = 1;
    char todo[4096];
    char long_todo[4096] = { '\0' };
    FILE* fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }
    FILE* tmp = tmpfile();

    if (tmp == NULL) {
        perror("Unable to open temp file");
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

    FILE* f = fopen(todo_file, "w+");
    if (f == NULL) {
        perror("Unable to open file");
        fclose(tmp);
        return;
    }

    rewind(tmp);

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(f, "%s", todo);
        }
    }
    fclose(f);
    fclose(tmp);
}

void complete_todo(int todo_num, char* todo_file) {
    int lines = get_lines_in_file(todo_file);
    if (lines == -1) {
        perror("Need to create a todo first");
        return;
    }
    int counter = 1;
    char todo[4096];
    FILE* fp = fopen(todo_file, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    FILE* tmp = tmpfile();

    if (tmp == NULL) {
	perror("Unable to create temp file");
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

    FILE* f = fopen(todo_file, "w+");
    if (f == NULL) {
	perror("Error opening file");
        fclose(tmp);
        return;
    }
    
    rewind(tmp);

    for (int i = 0; i < lines; i++) {
        if (fgets(todo, sizeof(todo), tmp) != NULL) {
            fprintf(f, "%s", todo);
        }
    }

    fclose(f);
    fclose(tmp);
}

int get_lines_in_file(char* todo_file) {
    FILE* fp = fopen(todo_file, "r");
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

void clear_screen() {
    printf("\e[1;1H\e[2]");
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


