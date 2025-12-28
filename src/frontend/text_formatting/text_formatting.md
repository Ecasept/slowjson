# Dokumentation text_formatting.h

<br> <br>

## Text anpassen
Die Anpassung von Textfarbe, Hintergrundfarbe und Stil des Textes erfolgt durch ANSI Escape Sequences. Diese sind vordefiniert als symbolische Konstanten.\
<br>
Beispiel:
```C
#define TXT_RED "\033[31m"
// Setzt die Textfarbe des nachfolgenden Textes auf rot
```

- `TXT_XXX`: Setzt Textfarbe auf `XXX`    
- `BACK_XXX`: Setzt Hintergrundfarbe auf `XXX`
- `TXT_XXX`: Setzt Textstil auf `XXX`
  - `BOLD`
  - `DIM`
  - `ITALIC`
  - `UNDERLINED`
  - `INVERSE`
- `END_STYLE`: Farbe und Stil wird zurückgesetzt

<br> <br> 

---
```C
void print_welcomescreen()
```
Ausgabe des Startbildschirms des Programm.\
Beim Programmstart wird zuerst, noch vor jeder Benutzereingabe, dieser Startbildschirm angezeigt



