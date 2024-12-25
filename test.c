#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char *get_internet_ip() {
    struct ifaddrs *ifaddr, *ifa;
    char *ip_address = NULL;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_INET &&
            strcmp(ifa->ifa_name, "lo") != 0) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            ip_address = malloc(INET_ADDRSTRLEN);
            if (ip_address == NULL) {
                perror("malloc");
                freeifaddrs(ifaddr);
                return NULL;
            }
            inet_ntop(AF_INET, &sa->sin_addr, ip_address, INET_ADDRSTRLEN);
            break;
        }
    }

    freeifaddrs(ifaddr);
    return ip_address;
}

int main() {
    char *ip = get_internet_ip();
    if (ip) {
        printf("Internet IP Address: %s\n", ip);
        free(ip);
    } else {
        printf("Could not find an internet IP address.\n");
    }
    return 0;
}
