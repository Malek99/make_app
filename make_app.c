#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<stdbool.h>
#include<sys/types.h>

#define MAX 4096

// to concatenate two strings (added into str)
void con(char *str, char *added)
{
     str += strlen(str);
     while(*added)
     {
         *str = *added;
         str++;
         added++;
     }
     *str = '\0';
}


// to replace the spaces in the file name with dashes - 
void dash(char *str)
{
    if(str[strlen(str) - 1] == ' ') // remove the last character if it is a space
        str[strlen(str) - 1] = '\0';
  
    while(*str)
    {
        if(*str == ' ')
            *str = '-';

        str++;
    }
}


// to check if the .desktop file already exists in /usr/share/applications directory or not
bool file_exist(const char *name)
{
    DIR *dir;
    struct dirent *file;

    dir = opendir("/usr/share/applications");

    while(file = readdir(dir))
    {
        if(!strcmp(name, file->d_name))
            return true;
    }

    return false;
}



int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./make_app <Executable file> <Icon>\nnote that this program requires root permission to make a .desktop file into /usr/share/applications/ directory\n");
        return 1;
    }

    if(geteuid() != 0) // to check for root permission
    {
        printf("this program requires root permission, try with sudo\n");
        return 2;
    }

    char file_str[MAX], word[200], name[200];
    file_str[0] = '\0'; // empty string

    con(file_str, "[Desktop Entry]\nType=Application\nEncoding=UTF-8\nName=");

    printf("Enter the name of the application:\n");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0'; // to remove the \n read by fgets
    con(file_str, name);
    
    printf("Do you want to add a comment? (y/n)\n");
    fgets(word, sizeof(word), stdin);

    if(word[0] == 'y' || word[0] == 'Y')
    {
        con(file_str, "\nComment=");
        printf("Enter a comment:\n");
        fgets(word, sizeof(word), stdin);
        con(file_str, word);
    }

    con(file_str, "\nExec=");

    if(argv[1][0] != '/') // to check if the path doesn't start from root directory
    {
        getcwd(word, sizeof(word)); // returns the path of the current directory (like pwd)
        con(word, "/");    
        con(file_str, word);
    }     
    
    con(file_str, argv[1]);

    con(file_str, "\nIcon=");

    if(argv[2][0] != '/')
        con(file_str, word);
        
    con(file_str, argv[2]);

    con(file_str, "\nTerminal=");
    printf("Is it a terminal application? (y/n)\n");
    fgets(word, sizeof(word), stdin);
    if(word[0] == 'y' || word[0] == 'Y')
        con(file_str, "true\n");
    else 
        con(file_str, "false\n");
    
   
    dash(name);
    con(name, ".desktop");
    word[0] = '\0';
    con(word, "/usr/share/applications/");
    con(word, name);

    if(file_exist(name))
    {
        printf("the file %s already exists, do you want to replace it? (y/n)\n", word);

        char op = getc(stdin);
        if(op != 'y' && op != 'Y')
            return 3;
    }
    
    FILE *desktop = fopen(word, "w"); // requires root permission

    if(!desktop)
    {
        printf("could not open %s\n", word);
        return 4;
    }
    
    fputs(file_str, desktop);
    fclose(desktop);
    
    printf("Done.\n");    
    return 0;
}