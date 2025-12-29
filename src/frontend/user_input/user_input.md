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
- Für ```size``` wird 0 übergeben oder für ```valid_input[]``` wird ```NULL```: ```INVALID_FUNCTION_INPUT```
<br> <br> <br>
---


```C
int flush()
```
Leert den Eingabepuffer.\
Rückgabe:
- flush erfolgreich: ```FLUSH_COMPLETE()```
- Pufferfehler: ```BUFFER_ERROR```
<br> <br> <br>
---
```C
int read_string(wchar_t **s)
```
Liest einen String ein den der Benutzer eingibt und speichert ihn in einem dynamische reservierten Array\ dessen Adresse in ```s``` gespeichert wird.\
Rückgabe:
- Gültige Benutzereingabe. ```VALID_USER_INPUT```
- Viermal ungültige Benutzereingabe: ```INVALID_USER_INPUT```
- Für ```s``` wurde ```NULL``` übergeben: ```INVALID_FUNCTION_INPUT```
- Pufferfehler: ```BUFFER_ERROR```
- Dynamische Speicherreservierung schlägt fehl: ```DYNAMIC_MEM_ERROR```



