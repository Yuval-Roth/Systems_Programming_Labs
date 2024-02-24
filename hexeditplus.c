#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct state{
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    int display_mode; // 0 for decimal, 1 for hex
} state;

typedef struct menuEntry{
    char* name;
    void (*run)(state *s);
} menuEntry;

static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

void not_implemented(){
    printf("Not implemented yet\n");
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
    char* line = (char*)malloc(128);
    printf("Enter file name: ");
    fgets(line, 101, stdin);
    sscanf(line,"%s\n", s->file_name);
    free(line);
    if (s->debug_mode == 1){
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}
void set_unit_size(state *s){
    int size;
    char* line = (char*)malloc(128);
    printf("Enter unit size: ");
    fgets(line, 128, stdin);
    sscanf(line,"%d\n", &size);
    free(line);

    if (size == 1 || size == 2 || size == 4){
        s->unit_size = size;

        if (s->debug_mode == 1){
            fprintf(stderr, "Debug: set size to %d\n", size);
        }
    }
    else{
        fprintf(stderr, "Error: invalid unit size\n");
    }
}
void load_into_memory(state *s){
    char *line;
    int location, length;
    FILE* file;

    if (strcmp(s->file_name, "" ) == 0){
        fprintf(stderr, "Error, file name is empty\n");
        return;
    }

    if ((file = fopen(s->file_name, "r")) == NULL){
        perror("fopen");
        return;
    }

    line = malloc(sizeof(char) * 512);
    printf("Please enter <location> <length>\n");
    fgets(line, 512, stdin);
    sscanf(line,"%x %d", &location, &length);
    free(line);

    if (s->debug_mode){
        fprintf(stderr,"Debug: file name = %s, location = 0x%x, length = %d\n",s->file_name,location,length);
    }

    fseek(file, 0, SEEK_END);
    s->mem_count = ftell(file);

    fseek(file, location, SEEK_SET);
    fread(s->mem_buf,sizeof(char),length,file);
    fclose(file);
    printf("Loaded %d units into memory\n",length);
}
void toggle_display_mode(state *s){
    if (s->display_mode == 0){
        s->display_mode = 1;
        fprintf(stderr,"Display flag now on, hexadecimal representation\n");
    } else{
        s->display_mode = 0;
        fprintf(stderr,"Display flag now off, decimal representation\n");
    }
}
void memory_display(state *s){
    unsigned int addr, length;

    if (strcmp(s->file_name, "" ) == 0){
        fprintf(stderr, "Error, file name is empty\n");
        return;
    }

    printf("Enter address and length\n");
    char* line = (char*)malloc(128);
    fgets(line, 128, stdin);
    sscanf(line,"%x %d\n", &addr, &length);
    free(line);

    printf(s->display_mode ? "Hexadecimal\n===========\n" : "Decimal\n=======\n");

    for (size_t i = 0; i < length; ++i) {
        unsigned int val;
        memcpy(&val, &s->mem_buf[addr + i * s->unit_size], s->unit_size);

        if (s->display_mode) {
            printf(hex_formats[s->unit_size-1],val);
        } else {
            printf(dec_formats[s->unit_size-1], val);
        }
    }

    printf("\n");

}
void save_into_file(state *s){
    FILE *file;
    int source_addr, target_loc, length;

    if (strcmp(s->file_name, "" ) == 0){
        fprintf(stderr, "Error, file name is empty\n");
        return;
    }

    printf("Please enter <source-address> <target-location> <length>\n");
    char* line = (char*)malloc(512);
    fgets (line, 512, stdin);
    sscanf(line,"%x %x %d", &source_addr, &target_loc, &length);
    free(line);

    file = fopen(s->file_name, "r+");

    // check that we are not trying to write beyond the file
    if (target_loc >= s->mem_count){
        fprintf(stderr, "Error: Target location exceeds the size of the file.\n");
        fclose(file);
        return;
    }

    fseek(file, target_loc * s->unit_size, SEEK_SET);
    fwrite(&s->mem_buf[source_addr], s->unit_size, length, file);
    fclose(file);
}
void memory_modify(state *s){
    int location,val;
    char* line = (char*)malloc(512);

    if (strcmp(s->file_name, "" ) == 0){
        fprintf(stderr, "Error, file name is empty\n");
        return;
    }

    printf("Please enter <location> <val>\n");

    fgets (line, 512, stdin);
    sscanf(line,"%x %x", &location, &val);
    free(line);
    if(location < 0 || location * s->unit_size >= s->mem_count){
        fprintf(stderr, "Error: location is out of bounds\n");
        return;
    }
    memcpy(&s->mem_buf[location * s->unit_size], &val, s->unit_size);
    if(s->debug_mode){
        printf("Debug: location = %x, val = %x\n", location,val);
    }
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
int menu_size = sizeof (menu) / sizeof (menu[0]) - 1;

void print_menu(){
    int i = 0;
    //print menu
    while(menu[i].name != NULL) {
        printf("%d-%s\n", i, menu[i].name);
        i++;
    }
}

state getNewState() {
    state state;
    strcpy(state.file_name,"");
    state.mem_count = 0;
    state.debug_mode = 0;
    state.unit_size = 1;
    state.display_mode = 0;
    return state;
}

int main (int argc, char **argv){
    char* line = (char*)malloc(512);
    int choice;
    state state = getNewState();

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