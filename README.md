# PSiW_Project
gcc -fno-stack-protector newServer.c -o newServer
gcc client.c -o client


Client mtype:
    1 - prośba o dołączenie
    2 - informacja serwera o zajętej nazwie urzytkownika
    3 - lista urzytkownikow
    4 - lista pokoi
    5 - dołączanie do pokoi
    6 - wychodzenie z pokoju
    7 - wysyłanie/odbieranie wiadomosci
Server mtype:
    1 - łączenie się klienta
    2 - rozłączanie klienta
    3 - lista urzytkownikow
    4 - lista pokoi
    5 - dołączanie do pokoi
    6 - wychodzenie z pokoju
    7 - wysyłanie/odbieranie wiadomosci