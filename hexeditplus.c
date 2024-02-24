#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct state{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    /*
     .
     .
     Any additional fields you deem necessary
    */
} state;

typedef struct menuEntry{
    char* name;
    void (*run)(state *s);
} menuEntry;

void not_implemented(){
    printf("Not implemented yet\n");
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void toggle_debug_mode(state *s){

    s->debug_mode = s->debug_mode == 0 ? 1 : 0;
    if (s->debug_mode == 1){
        fprintf(stderr,"Debug flag now on\n");
        fprintf(stderr, "Debug: unit_size=%d, file_name='%s', mem_count=%zu\n",
                s->unit_size, s->file_name, s->mem_count);
    } else{
        fprintf(stderr,"Debug flag now off\n");
    }
}
void set_file_name(state *s){
    printf("Enter file name: ");
    scanf("%127s", s->file_name);
    clear_stdin();
    if (s->debug_mode == 1){
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}
void set_unit_size(state *s){
    int size;
    printf("Enter unit size: ");
    scanf("%d",&size);
    clear_stdin();

    if (size == 1 || size == 2 || size == 4){
        s->unit_size = size;

        if (s->debug_mode == 1){
            fprintf(stderr, "Debug: set size to %d\n", size);
        }
    }
    else{
        fprintf(stderr, "Error: invalid unit size\n");
    }

    if (s->debug_mode == 1){
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}
void load_into_memory(state *s){
    not_implemented();
}
void toggle_display_mode(state *s){
    not_implemented();
}
void memory_display(state *s){
    not_implemented();
}
void save_into_file(state *s){
    not_implemented();
}
void memory_modify(state *s){
    not_implemented();
}
void quit(state *s){
    if (s->debug_mode) {
        fprintf(stderr, "Debug: quitting\n");
    }
    exit(0);
}

menuEntry menu[] = {
        {"Toggle Debug Mode", toggle_debug_mode},
        {"Set File Name", set_file_name},
        {"Set Unit Size", set_unit_size},
        {"Load Into Memory", load_into_memory},
        {"Toggle Display Mode",toggle_display_mode},
        {"Memory Display", memory_display},
        {"Save Into File", save_into_file},
        {"Memory Modify", memory_modify},
        {"Quit", quit},
        {NULL, NULL} // Terminating entry
};
int menu_size = sizeof (menu) / sizeof (menu[0]);

void print_menu(){
    int i = 0;
    //print menu
    while(menu[i].name != NULL) {
        printf("%d-%s\n", i, menu[i].name);
        i++;
    }
}

int main (int argc, char **argv){
    char* line = (char*)malloc(512);
    int choice;
    state state;

    while(1){
        print_menu();
        fgets(line, 512, stdin);
        choice = atoi(line);
        if(choice >= 0 && choice < menu_size){
            menu[choice].run(&state);
        }
        else{
            printf("Not within bounds\n");
        }
    }
}