Client
+   • przyjęcie nazwy użytkownika (nazwy muszą być unikalne),
+/- • zarejestrowanie użytkownika w zadanym serwerze, 
-   • wyświetlanie listy użytkowników zalogowanych na serwerze, 
-   • wyświetlenie listy zarejestrowanych kanałów (pokoi)
-   • wyświetlenie listy użytkowników zapisanych do pokoi
-   • zapisanie się użytkownika do pokoju
-   • wypisanie się użytkownika z pokoju
-   • wyświetlanie czatu prowadzonego na wskazanym kanale (każda wiadomość to czas wysłania,
-     nazwa użytkownika i treść) — wyświetlane jest ostatnich 10 wiadomości,
-   • możliwość wysłania wiadomości na wskazany kanał
-   • możliwość odebrania wiadomości z kanału
-   • możliwość odebrania wiadomości prywatnej (wyraźnie rozróżnionej),
-   • możliwość wysłania wiadomości prywatnej do dowolnego użytkownika (w ramach pokoi do których należy wysyłający użytkownik)
-   • możliwość wylogowania z systemu
-   • odpowiadanie na komunikaty heartbeat serwera.

Serwer
+   • rejestrowanie i wyrejestrowanie użytkowników
-   • przesłanie na żądanie klienta listy użytkowników / kanałów / użytkowników korzystających z kanału
-   • rejestrowanie nowego kanału
-   • zapisanie/wypisanie użytkownika do/z kanału
-   • odbiór i przesłanie odebranych wiadomości do wszystkich swoich użytkowników korzystających z danego kanału
-   • odbiór i przesłanie wiadomości prywatnych do wskazanego użytkownika na tym serwerze
-   • weryfikacja obecności klientów (mechanizm heartbeat)
-   • odbiór wiadomości i wysyłanie ich do wszystkich innych serwerów o ile są na nich odpowiednie kanały
-   • przesłanie informacji o zakończeniu pracy do pozostałych serwerów (skutkuje to usunięciem
-   informacji o serwerze na pozostałych serwerach-