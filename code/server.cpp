#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

// Function to handle a single client
void handle_client(SOCKET client_socket) 
{
  std::string buffer;
  char temp[1024];

  while (true) 
  {
    int bytes_received = recv(client_socket, temp, sizeof(temp) - 1, 0);
    if (bytes_received <= 0) 
    {
      std::cout << "Client " << client_socket << " disconnected\n";
#ifdef _WIN32
      closesocket(client_socket);
#else
      close(client_socket);
#endif
      return;
    }

    // Add received data to the buffer
    temp[bytes_received] = '\0';
    buffer += temp;

    // Check for a delimiter (`\n`)
    size_t pos;
    while ((pos = buffer.find('\n')) != std::string::npos) 
    {
      std::string message = buffer.substr(0, pos); // Extract the message
      buffer.erase(0, pos + 1);                    // Remove processed part
      std::cout << "Client " << client_socket << " says: " << message
                << std::endl;
    }
  }
}

int main() 
{
  const int PORT = 8080;

#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == INVALID_SOCKET) 
  {
    std::cerr << "Socket creation failed\n";
#ifdef _WIN32
    WSACleanup();
#endif
    return -1;
  }

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) ==
      SOCKET_ERROR) 
      {
    std::cerr << "Bind failed\n";
#ifdef _WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif
    return -1;
  }

  if (listen(server_socket, 10) == SOCKET_ERROR) {
    std::cerr << "Listen failed\n";
#ifdef _WIN32
    closesocket(server_socket);
    WSACleanup();
#else
    close(server_socket);
#endif
    return -1;
  }

  std::cout << "Server is listening on port " << PORT << "\n";

  std::vector<std::thread> client_threads;
  while (true) 
  {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    SOCKET client_socket =
        accept(server_socket, (sockaddr *)&client_addr, &client_len);
    if (client_socket == INVALID_SOCKET) {
      std::cerr << "Client connection failed\n";
      continue;
    }

    std::cout << "New client connected: " << client_socket << "\n";
    client_threads.emplace_back(handle_client, client_socket);
  }

  for (auto &t : client_threads) 
  {
    if (t.joinable()) 
    {
      t.join();
    }
  }

#ifdef _WIN32
  closesocket(server_socket);
  WSACleanup();
#else
  close(server_socket);
#endif
  return 0;
}
