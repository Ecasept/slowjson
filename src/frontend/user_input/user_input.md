# Dokumentation user_input.h




```C 
int read_command(char valid_input[], size_t size)
```

Liest ein einzelnes Zeichen ein, dass der Benutzer eingegeben hat. 
Wird verwendet, um zwischen den einzelnen screens des Programms zu navigieren.\
Rückgabe: 
- Wenn einzelnes Zeichen eingeben UND das Zeichen in ```valid_input[]``` enthalten ist: eingegebenes Zeichen
- Mehr als ein Zeichen eingeben ODER Zeichen nicht in ```valid_input[]``` enthalten: Funktion erlaubt so viele Eingabeversuche wie ```LIMIT_INPUT_ATTEMPTS```. Wird Anzahl überschritten, Rückgabe von ```INVALID_USER_INPUT```
- Pufferfehler: ```BUFFER_ERROR```
- Für size wird 0 übergeben: ```INVALID_FUNCTION_INPUT```
<br> <br> <br>
---


```C
int flush()
```
Leert den Eingabepuffer.\
Rückgabe:
- flush erfolgreich: ```FLUSH_COMPLETE()```
- Pufferfehler: ```BUFFER_ERROR```



