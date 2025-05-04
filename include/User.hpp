// platform/user.h
#pragma once

#include <string>

#if defined(_WIN32)
#include <Lmcons.h> // for UNLEN
#include <iphlpapi.h>
#include <windows.h>
#pragma comment(lib, "iphlpapi.lib")

inline std::string get_username() {
  char username[UNLEN + 1];
  DWORD size = UNLEN + 1;
  if (GetUserNameA(username, &size)) {
    return std::string(username);
  } else {
    return "unknown";
  }
}

inline std::string get_ip_address() {
  char hostname[256];
  if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    return "unknown";
  struct hostent *host = gethostbyname(hostname);
  if (!host || !host->h_addr_list[0])
    return "unknown";
  return std::string(inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
}

inline std::string get_mac_address() {
  IP_ADAPTER_INFO AdapterInfo[16];
  DWORD buflen = sizeof(AdapterInfo);
  DWORD status = GetAdaptersInfo(AdapterInfo, &buflen);
  if (status != ERROR_SUCCESS)
    return "unknown";
  char mac[18];
  sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", AdapterInfo[0].Address[0],
          AdapterInfo[0].Address[1], AdapterInfo[0].Address[2],
          AdapterInfo[0].Address[3], AdapterInfo[0].Address[4],
          AdapterInfo[0].Address[5]);
  return std::string(mac);
}

#elif defined(__linux__) || defined(__APPLE__)
#include <arpa/inet.h>
#include <cstring>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <unistd.h>

inline std::string get_username() {
  struct passwd *pw = getpwuid(getuid());
  return pw ? pw->pw_name : "unknown";
}

inline std::string get_ip_address() {
  struct ifaddrs *ifaddr, *ifa;
  if (getifaddrs(&ifaddr) == -1)
    return "unknown";
  std::string ip = "unknown";
  for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET &&
        !(ifa->ifa_flags & IFF_LOOPBACK)) {
      char host[NI_MAXHOST];
      getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST,
                  nullptr, 0, NI_NUMERICHOST);
      ip = host;
      break;
    }
  }
  freeifaddrs(ifaddr);
  return ip;
}

inline std::string get_mac_address() {
  struct ifaddrs *ifaddr, *ifa;
  if (getifaddrs(&ifaddr) == -1)
    return "unknown";

  for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr || !(ifa->ifa_flags & IFF_UP) ||
        (ifa->ifa_flags & IFF_LOOPBACK))
      continue;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
      continue;

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::snprintf(ifr.ifr_name, IFNAMSIZ, "%s", ifa->ifa_name);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
      close(fd);
      unsigned char *mac =
          reinterpret_cast<unsigned char *>(ifr.ifr_hwaddr.sa_data);
      char mac_str[18];
      std::snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      freeifaddrs(ifaddr);
      return std::string(mac_str);
    }
    close(fd);
  }

  freeifaddrs(ifaddr);
  return "unknown";
}

#else
inline std::string get_username() { return "unknown"; }
inline std::string get_ip_address() { return "unknown"; }
inline std::string get_mac_address() { return "unknown"; }
#endif
