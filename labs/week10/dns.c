// COMP1521 18s2 Week 10 Lab
// dns.c ... simple DNS lookup client

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv)
{
   struct in_addr ip;   // see "man 3 inet_aton"
   struct hostent *hp;  // see "man 3 gethostbyname"

   if (argc < 2) {
      printf("Usage: %s HostName|IPaddr\n", argv[0]);
      return 1;
   }

    // work out if argv[1] is an IP address
    // if not goto name
    char *p = argv[1];
    while (*p != '\0') {
       // printf("%c", *p);
        if (isalpha(*p)) {
            goto name;
        }
        p++;
    }
    
    // finding ip with name
    inet_aton(argv[1], &ip);
    hp = gethostbyaddr((const char*)&ip, sizeof(ip), AF_INET);    
    if (hp == NULL) {
        printf("No name associated with %s\n", argv[1]);
    } else {
        printf("%s -> %s\n", argv[1], hp->h_name);
        
        int i = 0;
        while (hp->h_aliases[i] != NULL) {
            printf("%s\n", hp->h_aliases[i]);
            i++;
        }
    }
    goto end;
    
    // finding ip by name
name:
    //inet_aton(argv[1], &ip);
    hp = gethostbyname(argv[1]);
    if (hp == NULL) {
        printf("Can't resolve %s\n", argv[1]);
    } else {
        struct in_addr* ia = (struct in_addr *)hp->h_addr;
        printf("%s\n", inet_ntoa(*ia));
       // printf("%s -> %d.%d.%d.%d\n", argv[1], hp->h_addr[0], hp->h_addr[1], hp->h_addr[2], hp->h_addr[3]);
       
       //char** aliases = hp->h_aliases;
       //www.clubpenguin.com
       int z = 0;
        while (hp->h_aliases[z] != NULL) {
            printf("%s\n", hp->h_aliases[z]);
            z++;
        }
        
    }
    
    
    
end:

   return 0;
}
