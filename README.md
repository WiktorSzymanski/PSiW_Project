# PSiW_Project
gcc -fno-stack-protector newServer.c -o newServer
gcc client.c -o client


Client mtype:
    1 - prośba o dołączenie
    2 - informacja serwera o zajętej nazwie urzytkownika
Server mtype:
    1 - łączenie się klienta
    2 - rozłączanie klienta