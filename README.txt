Kompilacja (w przypadku bledu stack smaching detected dodac do kompliacji -fno-stack-protector):

gcc inf148084_inf148222_s.c -o serwer
gcc inf148084_inf148222_k.c -o klient

Uruchamianie:
./serwer
./klient

Plik inf148084_inf148222_s.c zawiera kod zrodlowy aplikacji serwera.
Plik inf148084_inf148222_k.c zawiera kod zrodlowy aplikacji klienta.