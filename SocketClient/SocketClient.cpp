#include <Windows.h> // Включение заголовочного файла для работы с функциями Windows API
#include <iostream> // Включение заголовочного файла для стандартного ввода/вывода
#include <WinSock2.h> // Включение заголовочного файла для работы с сокетами в Windows
#include <WS2tcpip.h> // Включение заголовочного файла для работы с интернет-протоколами (TCP/IP)

using namespace std; // Использование стандартного пространства имен `std`

int main() { // Начало определения функции main

    WSADATA wsaData; // Структура для информации о использовании библиотеки Winsock
    ADDRINFO hints; // Структура для указания параметров сетевого адреса
    ADDRINFO* addrResult; // Указатель на структуру, содержащую информацию об адресах
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для установления соединения
    char recvBuffer[512]; // Буфер для принимаемых данных

    const char* sendBuffer1 = "Hello from client 1"; // Буфер для отправляемых данных 1
    const char* sendBuffer2 = "Hello from client 2"; // Буфер для отправляемых данных 2

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация библиотеки Winsock
    if (result != 0) { // Проверка успешности инициализации
        cout << "WSAStartup failed with result: " << result << endl; // Вывод сообщения об ошибке
        return 1; // Возврат из функции с ошибкой
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуление памяти структуры hints
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // Потоковый сокет
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    result = getaddrinfo("localhost", "666", &hints, &addrResult); // Получение информации об адресе
    if (result != 0) { // Проверка успешности
        cout << "getaddrinfo failed with error: " << result << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создание сокета для установления соединения
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на ошибку
        cout << "Socket creation failed" << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Установление соединения с сервером
    if (result == SOCKET_ERROR) { // Проверка успешности соединения
        cout << "Unable to connect to server" << endl; // Вывод сообщения об ошибке подключения
        closesocket(ConnectSocket); // Закрытие сокета
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0); // Отправка данных 1
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "Send failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }
    cout << "Sent: " << result << " bytes" << endl; // Вывод количества отправленных байт

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0); // Отправка данных 2
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "Send failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }
    cout << "Sent: " << result << " bytes" << endl; // Вывод количества отправленных байт

    result = shutdown(ConnectSocket, SD_SEND); // Отключение отправки данных
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "Shutdown failed, error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Освобождение ресурсов библиотеки Winsock
        return 1; // Возврат из функции с ошибкой
    }

    do { // Цикл обмена данными
        ZeroMemory(recvBuffer, 512); // Обнуление буфера приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных
        if (result > 0) { // Проверка успешного приема
            cout << "Received " << result << " bytes" << endl; // Вывод количества принятых байт
            cout << "Received data: " << recvBuffer << endl; // Вывод принятых данных
        }
        else if (result == 0) { // Проверка на закрытую связь
            cout << "Connection closed" << endl; // Вывод сообщения о закрытии соединения
        }
        else { // Обработка ошибки приема данных
            cout << "Recv failed, error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
        }
    } while (result > 0); // Повторение цикла, пока данные принимаются

    closesocket(ConnectSocket); // Закрытие сокета
    freeaddrinfo(addrResult); // Освобождение ресурсов
    WSACleanup(); // Освобождение ресурсов библиотеки Winsock
    return 0; // Возврат из функции без ошибок
}