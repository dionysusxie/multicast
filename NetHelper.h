/*
 * NetHelper.h
 *
 *  Created on: Jul 9, 2013
 *      Author: xieliang
 */

#ifndef NETHELPER_H_
#define NETHELPER_H_

#include <string>
#include <vector>

namespace allyes {

class NetHelper {
public:
    static std::string getDefaultNetInterface();
    static std::vector<std::string> getIP4Address(const std::string& net_interface);
    static std::string getIP4AddrOfDefInterface();
public:
    virtual ~NetHelper();
private:
    NetHelper();
};

} /* namespace allyes */
#endif /* NETHELPER_H_ */
