#define WIN32_LEAN_AND_MEAN // Определяет макрос, чтобы избавиться от некоторых менее используемых элементов Windows.h

#include <Windows.h> // Подключение заголовочного файла Windows.h, который содержит определения для работы с Windows API
#include <iostream> // Подключение заголовочного файла для работы с вводом/выводом
#include <WinSock2.h> // Подключение заголовочного файла для работы с сокетами в Windows
#include <WS2tcpip.h> // Подключение заголовочного файла для работы с интернет-протоколами (TCP/IP)

using namespace std; // Использование стандартного пространства имен `std`

int main() { // Начало определения функции main

    WSADATA wsaData; // Структура для информации о использовании библиотеки Winsock
    ADDRINFO hints; // Структура для указания параметров сетевого адреса
    ADDRINFO* addrResult; // Указатель на структуру, содержащую информацию об адресах
    SOCKET ListenSocket = INVALID_SOCKET; // Сокет для прослушивания соединений
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для установления соединения
    char recvBuffer[512]; // Буфер для принимаемых данных

    const char* sendBuffer = "Hello from server"; // Буфер для отправляемых данных

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация библиотеки Winsock
    if (result != 0) { // Проверка успешности инициализации
        cout << "WSAStartup failed with result: " << result << endl; // Вывод сообщения об ошибке
        return 1; // Возврат из функции с ошибкой
    }

    // Инициализация структуры hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // Потоковый сокет
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP
    hints.ai_flags = AI_PASSIVE; // Для IP-адреса, на который клиенты могут подключаться

    result = getaddrinfo(NULL, "666", &hints, &addrResult); // Получение информации об адресе
    if (result != 0) { // Проверка успешности
        cout << "getaddrinfo failed with error: " << result << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    // Создание сокета для прослушивания
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) { // Проверка на ошибку
        cout << "Socket creation failed" << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    // Привязка сокета к адресу
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "Bind failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    // Ожидание соединений
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "Listen failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    // Принятие соединения
    ConnectSocket = accept(ListenSocket, NULL, NULL);
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на ошибку
        cout << "Accept failed, error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    closesocket(ListenSocket); // Закрытие сокета прослушивания

    // Цикл обмена данными
    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных
        if (result > 0) { // Проверка на успешный прием данных
            cout << "Received " << result << " bytes" << endl; // Вывод количества принятых байт
            cout << "Received data: " << recvBuffer << endl; // Вывод принятых данных

            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // Отправка данных
            if (result == SOCKET_ERROR) { // Проверка на ошибку
                cout << "Send failed, error: " << result << endl; // Вывод сообщения об ошибке
                closesocket(ConnectSocket); // Закрытие сокета
                freeaddrinfo(addrResult); // Освобождение ресурсов
                WSACleanup(); // Освобождение ресурсов библиотеки Winsock
                return 1; // Возврат из функции с ошибкой
            }
        }
        else if (result == 0) { // Проверка на закрытую связь
            cout << "Connection closing" << endl; // Вывод сообщения о закрытии соединения
        }
        else { // Иной случай ошибки
            cout << "Recv failed, error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
            closesocket(ConnectSocket); // Закрытие сокета
            freeaddrinfo(addrResult); // Освобождение ресурсов
            WSACleanup(); // Освобождение ресурсов библиотеки Winsock
            return 1; // Возврат из функции с ошибкой
        }
    } while (result > 0); // Повторение цикла, пока данные принимаются

    result = shutdown(ConnectSocket, SD_SEND); // Закрытие отправки данных
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "Shutdown failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    closesocket(ConnectSocket); // Закрытие сокета установленного соединения
    freeaddrinfo(addrResult); // Освобождение ресурсов
    WSACleanup(); // Освобождение ресурсов библиотеки Winsock
    return 0; // Возврат из функции без ошибок
}