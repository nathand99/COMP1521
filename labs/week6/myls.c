// myls.c ... my very own "ls" implementation

#include <stdlib.h>
#include <stdio.h>
#include <bsd/string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXDIRNAME 100
#define MAXFNAME   200
#define MAXNAME    20

char *rwxmode(mode_t, char *);
char *username(uid_t, char *);
char *groupname(gid_t, char *);

int main(int argc, char *argv[])
{
   // string buffers for various names
   char dirname[MAXDIRNAME];
   char uname[MAXNAME+1];
   char gname[MAXNAME+1];
   char mode[MAXNAME+1];

   // collect the directory name, with "." as default
   if (argc < 2)
      strlcpy(dirname, ".", MAXDIRNAME);
   else
      strlcpy(dirname, argv[1], MAXDIRNAME);

   // check that the name really is a directory
   struct stat info;
   if (stat(dirname, &info) < 0)
      { perror(argv[0]); exit(EXIT_FAILURE); }
   if ((info.st_mode & S_IFMT) != S_IFDIR)
      { fprintf(stderr, "%s: Not a directory\n",argv[0]); exit(EXIT_FAILURE); }

  
  //open the directory to start reading
   DIR *df;
   df = opendir(dirname);
  
   // read directory entries
   struct dirent *entry;
  
  //while there are more entries in the directory
   while((entry = readdir(df)) != NULL){
     
     //ignore the object if its name starts with '.'
     if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
       continue;
     }
   
      //get the struct stat info for the object (using its path)
      sprintf(uname, "%s/%s", dirname, entry->d_name);
      struct stat buf;
      lstat(uname,&buf);
     
      //print the details using the object name and struct stat info
      uid_t ownerUID = buf.st_uid;
      gid_t groupGID = buf.st_gid;
      char * objectName = entry->d_name;
      mode_t modeInfo = buf.st_mode;
     
      printf("%s %-8.8s %-8.8s %8lld %s\n", rwxmode(modeInfo, mode), username(ownerUID, uname), groupname(groupGID, gname), (long long)buf.st_size, objectName);
     
   }

   // finish up
   closedir(df);
   return EXIT_SUCCESS;
}

// convert octal mode to -rwxrwxrwx string
char * rwxmode(mode_t mode, char *str)
{
    switch (mode & S_IFMT) {
      case S_IFDIR: printf("d");   break;
      case S_IFLNK: printf("l");  break;
      case S_IFREG: printf("-");   break;
      default: printf("?");      break;
    }
  
   if (mode & S_IRUSR) { printf("r");} else { printf("-"); };
   if (mode & S_IWUSR) { printf("w");} else { printf("-"); };
   if (mode & S_IXUSR) { printf("x");} else { printf("-"); };
   
   if (mode & S_IRGRP) { printf("r");} else { printf("-"); };
   if (mode & S_IWGRP) { printf("w");} else { printf("-"); };
   if (mode & S_IXGRP) { printf("x");} else { printf("-"); };
   
   if (mode & S_IROTH) { printf("r");} else { printf("-"); };
   if (mode & S_IWOTH) { printf("w");} else { printf("-"); };
   if (mode & S_IXOTH) { printf("x");} else { printf("-"); };

   str = "";

   return str;
}

// convert user id to user name
char *username(uid_t uid, char *name)
{
   struct passwd *uinfo = getpwuid(uid);
   if (uinfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)uid);
   else
      snprintf(name, MAXNAME, "%s", uinfo->pw_name);
   return name;
}

// convert group id to group name
char *groupname(gid_t gid, char *name)
{
   struct group *ginfo = getgrgid(gid);
   if (ginfo == NULL)
      snprintf(name, MAXNAME, "%d?", (int)gid);
   else
      snprintf(name, MAXNAME, "%s", ginfo->gr_name);
   return name;
}

