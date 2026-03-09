# Jumpstart HTTP Server - Build Instructions

## Cross-Platform Support

This server now supports both **Windows** and **Linux**!

---

## Building on Windows (Visual Studio)

### Option 1: Visual Studio IDE

1. Open `Jumpstart.sln` in Visual Studio 2022
2. Build → Build Solution (Ctrl+Shift+B)
3. Run the project (F5)

### Option 2: CMake (Windows)

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\bin\Release\jumpstart.exe
```

---

## Building on Linux

### Prerequisites

```bash
sudo apt-get update
sudo apt-get install build-essential cmake g++
```

### Build Steps

```bash
# Clone repository
git clone https://github.com/Nethrazim/jumpstart.git
cd jumpstart

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

# Run
./bin/jumpstart
```

---

## Testing the Server

### From Browser

```
http://localhost:8008/
```

### From Command Line

**Linux/macOS:**
```bash
curl http://localhost:8008/
```

**Windows (PowerShell):**
```powershell
Invoke-WebRequest http://localhost:8008/
```

---

## Platform Differences Handled

| Feature | Windows | Linux | Abstracted |
|---------|---------|-------|------------|
| **Sockets** | WinSock2 | BSD Sockets | ✅ `platform.h` |
| **Poll** | `WSAPoll` | `poll` | ✅ `POLL()` macro |
| **Non-blocking** | `ioctlsocket` | `fcntl` | ✅ `SET_NONBLOCKING()` |
| **Error codes** | `WSAGetLastError()` | `errno` | ✅ `GET_SOCKET_ERROR()` |
| **Threading** | `std::thread` | `std::thread` | ✅ Standard C++ |
| **Processor count** | `GetSystemInfo()` | `sysconf()` | ✅ `get_processor_count()` |

---

## File Structure

```
jumpstart/
├── CMakeLists.txt          # Cross-platform build config
├── BUILD.md                # This file
├── Jumpstart/
│   ├── platform.h          # Platform abstraction layer
│   ├── jumpstart.cpp       # Main server (cross-platform)
│   ├── tcp_ip_listener.h/cpp
│   ├── app-router.h/cpp
│   ├── init-controller.h/cpp
│   └── ...
└── build/                  # Generated build files
```

---

## Clean Build

### Windows
```powershell
Remove-Item -Recurse -Force build
```

### Linux
```bash
rm -rf build
```

---

## Port Configuration

Default port: **8008**

To change, edit `tcp_ip_listener.cpp`:
```cpp
bool init(int port = 8080, bool bindAndListen = true);
```

---

## Troubleshooting

### Windows: "Cannot find ws2_32.lib"
- Ensure Windows SDK is installed
- Project Properties → Linker → Input → Additional Dependencies → Add `ws2_32.lib`

### Linux: "undefined reference to pthread"
- CMake should handle this automatically
- Manual fix: `g++ ... -lpthread`

### Permission Denied (Linux)
```bash
sudo ./bin/jumpstart  # If port < 1024
```

---

## Next Steps

- Add SSL/TLS support (OpenSSL)
- Implement gzip compression
- Add JSON parser
- Static file serving
- Template engine

Happy coding! 🚀
