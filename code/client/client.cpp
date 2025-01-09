#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

int main() 
{
  const char *server_ip = "127.0.0.1";
  const int PORT = 8080;

  // Step 1: Initialize Winsock
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) 
  {
    std::cerr << "WSAStartup failed with error: " << result << "\n";
    return -1;
  }

  // Step 2: Create a socket
  SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket == INVALID_SOCKET) 
  {
    std::cerr << "Socket creation failed with error: " << WSAGetLastError()
              << "\n";
    WSACleanup();
    return -1;
  }

  // Step 3: Set up the server address structure
  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

  // Step 4: Connect to the server
  result = connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr));
  if (result == SOCKET_ERROR) 
  {
    std::cerr << "Connection failed with error: " << WSAGetLastError() << "\n";
    closesocket(client_socket);
    WSACleanup();
    return -1;
  }

  std::cout << "Connected to the server.\n";

  // Step 5: Main loop
  
  while (true) 
  {
    std::string message;
    std::getline(std::cin, message);
    std::cout << message << "\n";
    result = send(client_socket, message.c_str(),
                  static_cast<int>(message.size()), 0);
    if (result == SOCKET_ERROR) 
    {
      std::cerr << "Send failed with error: " << WSAGetLastError() << "\n";
      closesocket(client_socket);
      WSACleanup();
      return -1;
    }
  }

  // Step 6: Close the socket
  closesocket(client_socket);
  WSACleanup();

  return 0;
}