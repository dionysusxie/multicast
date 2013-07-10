
/* Receiver/client multicast Datagram example. */

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "NetHelper.h"

using namespace std;


struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];

const uint16_t PORT = 8649;
const char *MULTICAST_ADDR = "239.2.11.71";


int main(int argc, char *argv[]) {
    const string LOCAL_ADDR = allyes::NetHelper::getIP4AddrOfDefInterface();

    // show some basic info
    {
        cout << "Receive data from multi-addr " << MULTICAST_ADDR << ":" << PORT << endl;
        cout << "Local IP: " << LOCAL_ADDR << endl;
    }


    /* Create a datagram socket on which to receive. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Opening datagram socket error");
        exit(1);
    }
    else {
        printf("Opening datagram socket....OK.\n");
    }

    /* Enable SO_REUSEADDR to allow multiple instances of this */
    /* application to receive copies of the multicast datagrams. */
    {
        int reuse = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)) < 0) {
            perror("Setting SO_REUSEADDR error");
            close(sd);
            exit(1);
        }
        else {
            printf("Setting SO_REUSEADDR...OK.\n");
        }
    }

    /* Bind to the proper port number with the IP address */
    /* specified as INADDR_ANY. */
    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(PORT);
    localSock.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr*) &localSock, sizeof(localSock))) {
        perror("Binding datagram socket error");
        close(sd);
        exit(1);
    }
    else {
        printf("Binding datagram socket...OK.\n");
    }

    /* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
    /* interface. Note that this IP_ADD_MEMBERSHIP option must be */
    /* called for each local interface over which the multicast */
    /* datagrams are to be received. */
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
    group.imr_interface.s_addr = inet_addr(LOCAL_ADDR.c_str());

    if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &group,
            sizeof(group)) < 0) {
        perror("Adding multicast group error");
        close(sd);
        exit(1);
    }
    else {
        printf("Adding multicast group...OK.\n");
    }


    /* Read from the socket. */
    while (true) {
        datalen = sizeof(databuf);
        const ssize_t num_read = read(sd, databuf, datalen);
        if (num_read < 0) {
            perror("Reading datagram message error");
            close(sd);
            exit(1);
        }
        else {
            const string str_read(databuf, num_read);
            cout << "[received] Length = " << num_read << "; Content = {" << str_read
                 << "}" << endl;
        }
    }


    return 0;
}
