#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

void r_dir(char *name)
{
    DIR *pdir;
    struct dirent *pde;

    char *dir_p[30];
    char temp[30][50];

    int i = 0, k = 0;

    for (int m = 0; m < 30; m++)
        dir_p[m] = NULL;
    for (int b = 0; b < 30; b++)
    {
        for (int n = 0; n < 50; n++)
            temp[b][n] = NULL;
    }

    if (!(pdir = opendir(name)))
        return;

    printf("%s: \n", name);

    while ((pde = readdir(pdir)) != NULL)
    {
        if (pde->d_type == DT_DIR)
        {
            char path[1024];
            if (strcmp(pde->d_name, ".") == 0 || strcmp(pde->d_name, "..") == 0 || strcmp(pde->d_name, ".vscode") == 0 || strcmp(pde->d_name, ".git") == 0)
                continue;

            printf("%s%5s", pde->d_name, "");

            snprintf(path, sizeof(path), "%s/%s", name, pde->d_name);

            for (int c = 0; c < strlen(path); c++)
            {
                temp[i][c] = path[c];
            }
            dir_p[i] = temp[i];
            i++;
        }
        else
        {
            printf("%s%5s", pde->d_name, "");
        }
    }
    printf("\n\n");
    while (dir_p[k] != NULL)
    {
        if (dir_p[k] == NULL)
            break;
        r_dir(dir_p[k]);
        k++;
    }
    closedir(pdir);
}

int main(void)
{
    r_dir(".");
    return 0;
}