/*
 * sender_02.cpp
 *
 *  Created on: Jul 8, 2013
 *      Author: xieliang
 */

/* Send Multicast Datagram code example. */
#include <iostream>
#include <sstream>
#include <boost/assert.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include "NetHelper.h"

using namespace std;
using namespace boost;


struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;

char databuf[1024] = "Multicast test message lol!";
const int datalen = sizeof(databuf);

const char*     MULTICAST_ADDR = "239.2.11.71";
const uint16_t  MULTICAST_PORT = 8649;


int main(int argc, char *argv[]) {
    string LOCAL_ADDR;

    // get default net interface and its ip address
    {
        const string def_interface = allyes::NetHelper::getDefaultNetInterface();
        cout << "Default net interface: " << def_interface << endl;

        vector<string> ip4_addr = allyes::NetHelper::getIP4Address(def_interface);
        cout << "Its IP4 addr: ";
        for (vector<string>::const_iterator it = ip4_addr.begin(); it != ip4_addr.end(); it++) {
            cout << *it << " - ";
        }
        cout << endl;

        BOOST_ASSERT_MSG(!(ip4_addr.empty()), "Can't find the IP address of default net interface!");
        LOCAL_ADDR = ip4_addr[0];
    }


    // show some basic infos
    {
        cout << "Multicast addr: " << MULTICAST_ADDR << ":" << MULTICAST_PORT << endl;
        cout << "Local addr: " << LOCAL_ADDR << endl;
    }

    /* Create a datagram socket on which to send. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("Opening datagram socket error");
        exit(1);
    }
    else {
        printf("Opening the datagram socket...OK.\n");
    }

    /* Initialize the group sockaddr structure with a */
    /* group address of 225.1.1.1 and port 5555. */
    memset((char *) &groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    groupSock.sin_port = htons(MULTICAST_PORT);

    /* Disable loopback so you do not receive your own datagrams.
     {
     char loopch = 0;
     if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
     {
     perror("Setting IP_MULTICAST_LOOP error");
     close(sd);
     exit(1);
     }
     else
     printf("Disabling the loopback...OK.\n");
     }
     */

    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */
    localInterface.s_addr = inet_addr(LOCAL_ADDR.c_str());
    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *) &localInterface,
            sizeof(localInterface)) < 0) {
        perror("Setting local interface error");
        exit(1);
    }
    else {
        printf("Setting the local interface...OK\n");
    }


    unsigned msg_counter = 0;
    while (true) {
        /* Send a message to the multicast group specified by the*/
        /* groupSock sockaddr structure. */
        /*int datalen = 1024;*/

        // Send a string with a null-character!
        string msg;
        {
            ostringstream os;
            os << "msg " << msg_counter << " from sender-raw";
            msg = os.str();
        }

        // send out the msg
        cout << "[sending] " << msg << endl;
        if (sendto(sd, msg.c_str(), msg.size() + 1, 0, (struct sockaddr*) &groupSock, sizeof(groupSock)) < 0) {
            perror("Sending datagram message error");
        }
        else {
            msg_counter++;
        }

//        /* Try the re-read from the socket if the loopback is not disable */
//        if (read(sd, databuf, datalen) < 0) {
//            perror("Reading datagram message error\n");
//            close(sd);
//            exit(1);
//        }
//        else {
//            printf("[received] %s\n", databuf);
//        }

        sleep(1);
    }

    return 0;
}

