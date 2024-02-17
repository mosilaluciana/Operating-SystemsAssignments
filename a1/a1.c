#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


#define MAGIC "KJ97"

void list(char *path, int recursiv, char *name_ends_with, int size,int ok)
{

	DIR *dir = opendir(path);	 // deschide directorul
	struct dirent *entry = NULL; // va contine numele fisierului in campul d_name
	struct stat fileMetadata;
	char fullPath[1024];
	char path2[1024];
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			snprintf(fullPath, 1024, "%s/%s", path, entry->d_name);
			if (lstat(fullPath, &fileMetadata) == 0)
			{
				if (ok == 1)
				{
					strcpy(path2,entry->d_name);
					int n = strlen(entry->d_name) - strlen(name_ends_with);
					strcpy(path2,path2+n);
					if(strcmp(name_ends_with,path2)==0)
					{
						printf("%s\n",fullPath);
					}
				}
				else
				if (size != -1)
				{
					if (S_ISREG(fileMetadata.st_mode))
					{
						if (fileMetadata.st_size > size)
						{
							printf("%s\n", fullPath);
						}
					}
				}
				else
				{
					printf("%s\n", fullPath);
				}
				if (recursiv == 1)
				{
					if (S_ISDIR(fileMetadata.st_mode))
						list(fullPath, recursiv, name_ends_with, size,ok);
				}
			}
		}
	}
		closedir(dir); 
	}


	int parse(int fd)
	{
		char name[10];
		int offset=0;
		short int  type=0;
		int size=0;
		char magic[5];

		short int header_size=0;
		char nrSectiuni=0;
		short int version=0;

		//citim magic si verificam daca este corect
		read(fd,magic,4);
		magic[4]='\0';
		if(strcmp(magic,"KJ97")!=0){
			printf("ERROR\nwrong magic\n");
			return -1;
		}
		
		//citim header 
		read(fd,&header_size,2);
			
		//citim versiunea
		read(fd,&version, 2);
		if(version<94 || version>136)
		{
			printf("ERROR\nwrong version\n");
			return -1;
		}	
		
		
		//Citim numarul de sectiuni
		read(fd, &nrSectiuni, 1);
		if(nrSectiuni <4 || nrSectiuni >18)
		{
			printf("ERROR\nwrong sect_nr\n");
			return -1;
		}
		
		int j=0;	
		while(j<nrSectiuni)
		{
			read(fd, name, 9);
			name[9]='\0';
			
			read(fd, &type, 2);
			if(type!=89 && type!=15 && type!=60 &&type!=46 && type!=42 && type!=93)
			{
				printf("ERROR\nwrong sect_types\n");
				return -1;
			}
			read(fd, &offset, 4);
			read(fd, &size, 4);
			
			
			j++;
		}
		lseek(fd,-19*nrSectiuni,SEEK_CUR);
		
		printf("SUCCESS\n");
		printf("version=%d\n", version);
		printf("nr_sections=%d\n", nrSectiuni);

		
		
		j=0;
		while(j<nrSectiuni)
		{
			read(fd, name, 9);
			name[9]='\0';
			read(fd, &type, 2);
			read(fd, &offset, 4);
			read(fd, &size, 4);
			printf("section%d: %s %d %d\n", j+1, name, type, size);
			
			j++;
		}
		
		
		
		return 0;
	}



	int main(int argc, char **argv)
	{	
				char path[512];

		if (argc >= 2)
		{
			if (strcmp(argv[1], "variant") == 0)
			{
				printf("35801\n");
			}
			else if (strcmp(argv[1], "list") == 0)
			{
				int recursiv = 0;
				int size = -1;
				int ok = 0;
				char name_ends_with[512];

				for(int j=2;j<argc;j++)
				{
					if (strncmp(argv[j], "path=", 5) == 0)
						strcpy(path, argv[j] + 5);
					else if (strcmp(argv[j], "recursive") == 0)
						recursiv = 1;
					else if (strncmp(argv[j], "name_ends_with=", 15) == 0)
						{strcpy(name_ends_with, argv[j] + 15);ok=1;}
					else if (strncmp(argv[j], "size_greater=", 13) == 0)
						size = atoi(argv[j] + 13);	
				}
				// printf("%s",path);
				struct stat fileMetadata;
				if (stat(path, &fileMetadata) == -1 || !S_ISDIR(fileMetadata.st_mode))
					printf("ERROR\ninvalid directory path\n");
				else
				{
					printf("SUCCESS\n");
					list(path, recursiv, name_ends_with, size,ok);
				}
				return 0;
			}
			else if(strcmp(argv[1], "parse") == 0)
			{
				
					strcpy(path, argv[2] + 5);
					int fd;
					if((fd=open(path,O_RDONLY)) >= 0)
					{
						parse(fd);
						close(fd);
					}
					else
					printf("ERROR\ninvalid file path\n");
			}
		}
	return 0;
	}
