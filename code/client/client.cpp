#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

std::atomic<bool> running(true); // Atomic flag to handle graceful exit

// Function to handle receiving messages from the server
void receive_messages(SOCKET client_socket) 
{
    char buffer[1024];
    while (running) 
    {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) 
        {
            buffer[bytes_received] = '\0'; // Null-terminate the received message
            std::cout << buffer << std::endl;
        } 
        else if (bytes_received == 0) 
        {
            std::cout << "Server disconnected.\n";
            running = false;
        } 
        else 
        {
            std::cerr << "Receive failed with error: " << WSAGetLastError() << "\n";
            running = false;
        }
    }
}

int main() 
{
    const char *server_ip = "127.0.0.1";
    const int PORT = 8080;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) 
    {
        std::cerr << "WSAStartup failed with error: " << result << "\n";
        return -1;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) 
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    result = connect(client_socket, (sockaddr *)&server_addr, sizeof(server_addr));
    if (result == SOCKET_ERROR) 
    {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to the server.\n";

    // Start the receive thread
    std::thread receive_thread(receive_messages, client_socket);

    // Main loop to send messages to the server
    while (running) 
    {
        std::string message;
        std::getline(std::cin, message);

        if (message == "exit") // Allow graceful exit
        {
            running = false;
            break;
        }

        result = send(client_socket, message.c_str(),
                      static_cast<int>(message.size()), 0);
        if (result == SOCKET_ERROR) 
        {
            std::cerr << "Send failed with error: " << WSAGetLastError() << "\n";
            running = false;
            break;
        }
    }

    // Wait for the receive thread to finish
    if (receive_thread.joinable()) 
    {
        receive_thread.join();
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}