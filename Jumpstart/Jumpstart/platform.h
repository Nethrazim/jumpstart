#pragma once

// Platform abstraction layer for Windows/Linux compatibility

#ifdef _WIN32
    // Windows includes
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    
    // Type aliases
    typedef SOCKET socket_t;
    typedef WSAPOLLFD pollfd_t;
    
    // Function macros
    #define CLOSE_SOCKET(s) closesocket(s)
    #define POLL(fds, nfds, timeout) WSAPoll(fds, (ULONG)(nfds), timeout)
    #define GET_SOCKET_ERROR() WSAGetLastError()
    #define SET_NONBLOCKING(s) { u_long mode = 1; ioctlsocket(s, FIONBIO, &mode); }
    
    // Error codes
    #define WOULDBLOCK_ERROR WSAEWOULDBLOCK
    #define SOCKET_INTERRUPTED WSAEINTR
    
    // Poll events (already defined in Windows)
    // POLLRDNORM, POLLWRNORM, POLLERR, POLLHUP, POLLNVAL
    
    // System info
    inline unsigned int get_processor_count() {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
    }
    
#else
    // Linux/POSIX includes
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <errno.h>
    #include <cstring>
    
    // Type aliases
    typedef int socket_t;
    typedef struct pollfd pollfd_t;
    
    // Constants
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    
    // Function macros
    #define CLOSE_SOCKET(s) close(s)
    #define POLL(fds, nfds, timeout) poll(fds, nfds, timeout)
    #define GET_SOCKET_ERROR() errno
    #define SET_NONBLOCKING(s) { \
        int flags = fcntl(s, F_GETFL, 0); \
        fcntl(s, F_SETFL, flags | O_NONBLOCK); \
    }
    
    // Error codes
    #define WOULDBLOCK_ERROR EAGAIN
    #define SOCKET_INTERRUPTED EINTR
    
    // Poll events mapping
    #define POLLRDNORM POLLIN
    #define POLLWRNORM POLLOUT
    #define POLLNVAL 0x0020
    
    // System info
    #include <thread>
    inline unsigned int get_processor_count() {
        return std::thread::hardware_concurrency();
    }
    
#endif

// Platform initialization/cleanup
namespace Platform {
    inline bool initialize() {
#ifdef _WIN32
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
        return true; // Linux doesn't need socket initialization
#endif
    }
    
    inline void cleanup() {
#ifdef _WIN32
        WSACleanup();
#else
        // Nothing to do on Linux
#endif
    }
}
