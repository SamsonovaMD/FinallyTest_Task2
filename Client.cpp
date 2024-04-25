#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr int PORT = 8081;
constexpr int BUFFER_SIZE = 1024;

int main() {
    // Создание сокета
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Ошибка при создании сокета\n";
        return 1;
    }

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Замените на IP-адрес вашего сервера, если он находится на другом компьютере

    // Установка соединения с сервером
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Ошибка при установке соединения с сервером\n";
        close(clientSocket);
        return 1;
    }

    // Отправка сообщения серверу
    const char* message = "Привет, сервер!";
    int bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent == -1) {
        std::cerr << "Ошибка при отправке данных серверу\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Сообщение отправлено серверу\n";

    // Получение подтверждения от сервера
    char buffer[BUFFER_SIZE];
    int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesRead == -1) {
        std::cerr << "Ошибка при чтении данных от сервера\n";
        close(clientSocket);
        return 1;
    }

    buffer[bytesRead] = '\0';
    std::cout << "Получено от сервера: " << buffer << std::endl;

    // Закрытие клиентского сокета
    close(clientSocket);

    return 0;
}
