#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

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


int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./make_app <Executable file> <Icon>\n");
        return 1;
    }

    char file_str[MAX], word[200], name[200];
    file_str[0] = '\0'; // empty string

    con(file_str, "[Desktop Entry]\nType=Application\nEncoding=UTF-8\nName=");

    printf("Enter the name of the application:\n");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0'; // to remove the \n read by fgets
    con(file_str, name);
    
    printf("Do you want to add a comment? (y/n)\n");
    scanf("%s", word);

    getc(stdin); // just to trem the \n to prevent the next gets from reading it
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
    scanf("%s", word);
    if(word[0] == 'y' || word[0] == 'Y')
        con(file_str, "true\n");
    else 
        con(file_str, "false\n");
    
   
    dash(name);
    con(name, ".desktop");
    word[0] = '\0';
    con(word, "/usr/share/applications/");
    con(word, name);
    FILE *desktop = fopen(word, "w"); // requires root permission

    if(!desktop)
    {
        printf("could not open %s, try with sudo\n", word);
        return 3;
    }
    
    fputs(file_str, desktop);
    fclose(desktop);
    
    printf("Done.\n");    
    return 0;
}