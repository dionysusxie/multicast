/*
 * NetHelper.cpp
 *
 *  Created on: Jul 9, 2013
 *      Author: xieliang
 */

#include <boost/assert.hpp>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sstream>
#include "NetHelper.h"

using namespace std;
using namespace boost;

namespace allyes {

string NetHelper::getDefaultNetInterface() {
    FILE *f = fopen("/proc/net/route" , "r");

    const int BUF_SIZE = 100;
    char line[BUF_SIZE];
    string def_net_interface;

    while (fgets(line , 100 , f)) {
        char *save_ptr = NULL;
        char *interface = strtok_r(line , " \t", &save_ptr);
        char *destination = strtok_r(NULL , " \t", &save_ptr);

        if (interface != NULL && destination != NULL) {
            if (strcmp(destination , "00000000") == 0) {
                def_net_interface = interface;
                break;
            }
        }
    }

    fclose(f);
    f = NULL;

    BOOST_ASSERT_MSG(!(def_net_interface.empty()), "Can't find the default net interface!");
    return def_net_interface;
}

// @return  The IP4 address of the 'net_interface'.
vector<string> NetHelper::getIP4Address(const std::string& net_interface) {
    vector<string> ret_ip4_addr;

    //which family do we require , AF_INET or AF_INET6
    const int fm = AF_INET;

    struct ifaddrs *ifaddr = NULL;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        BOOST_ASSERT(false);
        return ret_ip4_addr;
    }


    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        bool is_this_netif = (strcmp(ifa->ifa_name, net_interface.c_str()) == 0);
        if (is_this_netif) {
            const int family = ifa->ifa_addr->sa_family;
            if (family == fm) {
                char host[NI_MAXHOST];
                int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
                        NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

                if (s != 0) {
                    ostringstream err;
                    err << "getnameinfo() failed: " << gai_strerror(s);
                    BOOST_ASSERT_MSG(false, err.str().c_str());
                    break;
                }
                else {
                    ret_ip4_addr.push_back(host);
                }
            }
        }
    }

    // clear
    freeifaddrs(ifaddr);
    ifaddr = NULL;

    // return
    return ret_ip4_addr;
}

string NetHelper::getIP4AddrOfDefInterface() {
    vector<string> addr = NetHelper::getIP4Address(NetHelper::getDefaultNetInterface());
    BOOST_ASSERT_MSG(!(addr.empty()), "Can't get any IP4 addr of default net interface!");
    return addr[0];
}

NetHelper::NetHelper() {
    // TODO Auto-generated constructor stub

}

NetHelper::~NetHelper() {
    // TODO Auto-generated destructor stub
}

} /* namespace allyes */
