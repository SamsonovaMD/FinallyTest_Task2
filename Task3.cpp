#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr int PORT = 8081;
constexpr int BUFFER_SIZE = 1024;
constexpr int MAX_CONNECTIONS = 5;

int main() {
    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка при создании сокета\n";
        return 1;
    }

    // Настройка адреса сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Ошибка при привязке сокета\n";
        close(serverSocket);
        return 1;
    }

    // Прослушивание входящих соединений
    if (listen(serverSocket, MAX_CONNECTIONS) == -1) {
        std::cerr << "Ошибка при прослушивании входящих соединений\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Сервер запущен. Ожидание подключений...\n";

    // Буфер для приема и отправки данных
    char buffer[BUFFER_SIZE];

    while (true) {
        // Принятие входящего соединения
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Ошибка при принятии соединения\n";
            close(serverSocket);
            return 1;
        }

        std::cout << "Новое подключение от " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // Чтение данных от клиента и отправка обратно подтверждения
        while (true) {
            int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead == -1) {
                std::cerr << "Ошибка при чтении данных от клиента\n";
                close(clientSocket);
                break;
            } else if (bytesRead == 0) {
                std::cout << "Соединение с клиентом закрыто\n";
                close(clientSocket);
                break;
            }

            // Вывод принятых данных
            std::cout << "Получено от клиента: " << buffer << std::endl;

            // Отправка подтверждения клиенту
            const char* response = "Сообщение получено";
            int bytesSent = send(clientSocket, response, strlen(response), 0);
            if (bytesSent == -1) {
                std::cerr << "Ошибка при отправке данных клиенту\n";
                close(clientSocket);
                break;
            }
        }
    }

    // Закрытие серверного сокета
    close(serverSocket);

    return 0;
}