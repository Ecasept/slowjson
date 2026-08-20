# slowjson
An overengineered, standalone zero-dependency JSON manipulation library written in pure C11. Developed as the persistence layer for a university course planner group project at the University of Augsburg. But mainly because i wanted to challenge myself and learn new stuff.

## Features
- **DOM Manipulation:** Helpers to store and modify and JSON types as C datatypes
- **Serialization:** Ability to convert between JSON objects and strings
- **I/O Helpers:** Easy integration for reading and writing to files

## Implementation
It features many “sub-libraries” and helpful utilties that i included as a learning experience, under the pretext that we weren’t allowed to use external libraries. This includes:
- A custom Arena memory allocator with valgrind and asan integration
- A custom string and string view class
- A generic dynamic array implementation in c
- A hashmap with separate chaining and a polynomial rolling hash function
- A full UTF8 (and 16) encoder and decoder
- A test runner that tests the library on multiple well established json test suites and on memory safety
- A consistent error handling paradigm throughout the whole codebase (failed memory allocations, eg. because of an OOM, panic and exit)
## Constraints
(based on the group project requirements)
- C11 compliance
- no external libraries
- cross-compatibility
- no (non-avoidable) compilation warnings or errors
- compiles without any additional compiler parameters other than `-std=c11 -Wall -Wextra -pedantic`

## Performance
Even under the strict project and time constraints, the parser includes a fair amount of performance enhancing techniques. Among other things, it utilizes custom arena allocators, aggressive function inlining, multiple fast paths, direct memory mapping, zero-copy string views and extensive performance tests. However it still does not beat most established json parsers.

<img width="882" height="567" alt="ResizedImage_2026-08-20_18-50-27_7335" src="https://github.com/user-attachments/assets/6b62952e-2ea4-4a78-88c6-0a8f7b8b4c73" alt="Comparing slowjson with established parsers on different datasets. slowjson is mostly dead last by a good amount"/>

## Context
slowjson is a standalone JSON manipulation library that I developed for a group project in my first semester at the University of Augsburg. I was responsible for the persistence layer, while the other members built the actual university course planner on top of the library. We received full marks for the project.

The slowjson library itself is located in `src/json`, while `src/data` contains the usage of the library for this application. The other directories contain the actual application code for the study course organization program.
## Usage
See `src/data/load.c` and `src/data/save.c`
## Building

This project uses [make](https://en.wikipedia.org/wiki/Make_(software)) as its build system.

```sh
# Alias for `make all`
make

# Build the program (output: ./build/gradeviewer).
# Only changed files are recompiled.
make all

# Build and run
make run

# Force a full rebuild, then run
make run rebuild=1

# Build and run tests
make run test=1

# Run Valgrind (memory leak checks)
make valgrind

# Run Callgrind profiling
make profile
```

See the `Makefile` for additional targets and options.

If `make` is not available on your system, you can compile manually with `gcc`:

```sh
gcc -Wall -Wextra -pedantic -std=c11 src/data/load.c src/data/save.c src/frontend/edit_events/edit_events.c src/frontend/text_formatting/text_formatting.c src/frontend/user_input/user_input.c src/json/internal/config.c src/json/internal/deserialize.c src/json/internal/jsonvalue.c src/json/internal/lexer/main.c src/json/internal/lexer/number.c src/json/internal/parser.c src/json/internal/serialize.c src/json/test/fxx_test.c src/json/test/jsonperf.c src/json/test/jsontestsuite.c src/json/test/test.c src/json/utils/alloc/arena.c src/json/utils/custom_error.c src/json/utils/hashmap/hashmap.c src/json/utils/hashmap/hashmap_node.c src/json/utils/list.c src/json/utils/string/dstring.c src/json/utils/string/file.c src/json/utils/string/string_view.c src/json/utils/unicode/utf16.c src/json/utils/unicode/utf8.c src/json/utils/unicode/wchar.c src/main/main.c src/midend/data.c src/midend/mid.c
```

> **Note**
> Although the requirement was that the project should compile without warnings, some warnings may appear across different compilers and operating systems.  
> In particular, certain Windows/MinGW toolchains may warn about `%zu` (`size_t`) even when runtime behavior is correct.

## Original README

## Nutzungshinweise
>### Allgemeines
Im folgenden Abschnitt werden Tastatureingaben durch eckige Klammern ```[]``` gekennzeichnet (z. B. Taste ```[q]``` zum beenden des Programms).
<br> <br> 
- Mit Ausnahme aller Eingabedialoge, kann von jeder Seite innerhalb des Programms mit ```[h]``` die **Hilfe** aufgerufen werden. 
<br> <br>
- Mit Ausnahme aller Eingabedialoge kann das Programm jederzeit über ```[q]``` beendet werden.
<br> <br>
- Wird eine ungültige Eingabe getätigt erscheint eine Fehlermeldung die den Nutzer darüber informiert welche Eingaben gültig sind. Anschließend hat der Nutzer erneut die Möglichkeit eine Eingabe zu tätigen.
<br>
Wird viermal direkt hintereinander eine falsche Eingabe getätigt ruft das Programm automatisch die **Hilfe** auf. <br>
Wird bei angezeigter **Hilfe** viermal hintereinander eine falsche Eingabe getätigt wird das Programm automatisch beendet.
<br><br><br><br>


>### Programmstart
Nach dem Start des Programms wird zunächst der **Startbildschirm** angezeigt. Wurde das Programm zum allerersten Mal gestartet wird automatisch eine neue Datei angelegt in der die Veranstaltungen gespeichert werden. Wurden bereits Veranstaltungen angegelegt so wird die entsprechende *.json*-Datei automatisch geladen.<br> <br>
**Fehlerbehandlung Datei laden** <br>
Tritt beim laden der Datei ein Fehler auf, erscheint auf dem Startbildschirm eine Fehlermeldung. <br> Der Nutzer hat drei Möglichkeiten mit dem Fehler umzugehen:
<br>
- ``[r]``: Erneut versuchen die Datei zu laden
- ``[n]``: Eine neue Datei anlegen in der die neu hinzugefügten Veranstaltungen gespeichert werden. Hierbei wird die bisher bestehende Datei gelöscht und alle Daten darin gehen verloren.
- ``[r]``: Das Programm wird beendet
<br> <br>

**Optionen** <br>
Vom Startbildschirm aus gibt es drei verschiedene Optionen:
- ``[v]``: Ruft die **Veranstaltungsübersicht** auf
- ``[h]``: Ruft die **Hilfe** des Programms auf
- ``[q]``: Das Programm wird beendet
<br><br><br><br>


>### Veranstaltungsübersicht
Die **Veranstaltungsübersicht** listet alle gespeicherte Veranstaltungen auf.
<br> 
Die Veranstaltungen können nach Modulgruppe oder nach Semester sortiert angezeigt werden. Über ``[a]`` wird die Sortierung umgeschaltet. 
<br>
Zu jeder Veranstaltung werden der ``Name``, der ``Status``, die Anzahl der ``Leistungspunkte`` und die ``Note`` angezeigt. <br>
Bei der Sortierung nach Semester, wird zusätzlich die Summe der Leistungspunkte angezeigt die pro Semester erbracht wurden. <br>
Bei der Sortierung nach Modulgruppe wird pro Modulgruppe angezeigt, wie viele Leistungspunkte bereits in der Modulgruppe erbracht wurden und wie viele Leistungspunkte insgesamt in der Modulgruppe zu erbringen sind.
<br> <br>
Der ``Status`` der Veranstaltung unterscheidet zwischen drei Zuständen:
- ``ausstehend`` (es wurde noch keine Prüfung geschrieben)
- ``nicht bestanden`` (Note 4.3 oder schlechter)
- ``bestanden`` (Note 4.0 oder besser)
<br>

Zu jeder ``bestandenen`` und ``nicht bestandenen`` Veranstaltung wird ``Note`` angezeigt. Bei Veranstaltungen mit ``Status`` ``ausstehend`` wird keine Note angezeigt.
<br><br>


**Optionen** <br>
Von der Veranstaltungsübersicht aus gibt es folgende Optionen:
- ``[n]``: Neue Veranstaltung hinzufügen
- ``[m]``: Neue Modulgruppe hinzufügen
- ``[d]``: Aktuellen Leistungsstand anzeigen
- ``[b]``: Daten einer bestehenden Veranstaltung bearbeiten
<br><br><br><br>




>### Veranstaltung hinzufügen
Die Option zum hinzufügen einer neuen Veranstaltung ist von der **Veranstaltungsübersicht** und der Anzeige des **Leistungsstands** aus aufrufbar. Mit ``[n]`` wird der Dialog zum hinzufügen einer neuen Veranstaltung aufgerufen. Anschließend müssen schrittweise alle nötigen Daten der Veranstaltung eingegeben werden:
<br>

  1.   **Name**  

  2.   **Modulgruppe** <br> Jede Veranstaltung muss zwingend einer Modulgruppe zugeordnet werden. Es kann entweder eine bereits bestehende Modulgruppe gewählt oder eine neue Modulgruppe angelegt werden.

  3. **Semester** <br> 
      1. Auswahl:
         - Wintersemester ``[w]``
      
         - Sommersemester ``[s]``
      2. Jahreszahl des Semesterbeginns <br> Beispielsweise ``[2025]`` für Wintersemester 2025/2026 oder ``[2027]`` für Sommersemester 2027.

4. **Leistungspunkte**

5. **Note** <br> Das hinzufügen der Note ist **optional**. <br>
Wird ``[j]`` gewählt so muss anschließend eine gültige Note eingegeben werden. Entsprechend der Note wird der ``Status`` der Veranstaltung automatisch auf ``bestanden`` oder ``nicht bestanden`` gesetzt. <br> Bei der Wahl von ``[n]`` muss keine Note eingegeben werden und der ``Status`` der Veranstaltung wird automatisch auf ``ausstehend`` gesetzt.
<br><br>
Nachdem die Eingabe der Daten abgeschlossen wurde wird automatisch wieder die Seite angezeigt von welcher aus der Dialog zum hinzufügen der Veranstaltung aufgerufen wurde (entweder **Veranstaltungsübersicht** oder **Leistungsstand**).

<br><br><br><br>


>### Modulgruppe anlegen
Die Option zum hinzufügen einer neuen Modulgruppe ist von der **Veranstaltungsübersicht** und der Anzeige des **Leistungsstands** aus aufrufbar. Mit ``[m]`` wird der Dialog zum hinzufügen einer neuen Modulgruppe aufgerufen. Anschließend müssen schrittweise alle nötigen Daten der Modulgruppe eingegeben werden:
<br>

  1.   **Name**  

  2.   **Leistungspunkte** <br> Es muss die Anzahl der Leistungspunkte eingegeben werden die insgesamt in dieser Modulgruppe erbracht werden müssen.

<br><br>
Nachdem die Eingabe der Daten abgeschlossen wurde wird automatisch wieder die Seite angezeigt von welcher aus der Dialog zum hinzufügen der Modulgruppe aufgerufen wurde (entweder **Veranstaltungsübersicht** oder **Leistungsstand**).
<br><br><br><br>



>### Veranstaltung bearbeiten
Die Option zum bearbeiten einer Veranstaltung ist von der **Veranstaltungsübersicht** und der Seite des **Leistungsstands** aufrufbar. Mit ``[b]`` erscheint der Dialog zum bearbeiten einer Veranstaltung. Zunächst gibt man mithilfe eiens Indexes an, welche Veranstaltung man bearbeiten möchte. Hat man dies ausgewählt wird einem nochmal schön übersichtlich die Veranstaltung angezeigt, die man bearbeitet. 
Jetzt hat man verschiedene Möglichkeiten zum Bearbeiten des Moduls. 
1. ``[l]`` Veranstaltung löschen 
2. ``[n]`` Note ändern/hinzufügen 
3. ``[m]`` Modulgruppe ändern 
4. ``[v]`` Zurück zur Veranstaltungsübersicht
5. ``[c]`` Andere Veranstaltung wählen
Bei jeder Auswahlmöglichkeit kommt man dann zu einer weiteren Auswahl, wie zb.: ``Neue Note eingeben`` oder ``Note auf "ausstehend" setzen``.

<br><br><br><br>

>### Notendurchschnitt berechnen (nach FPO 2018 Bachelor Informatik)
Die Seite **Leistungsstand** wird mit ``[d]`` aufgerufen. Dort wird einem dann der Aktuelle Notendurchschnitt nach FPO 2018 Bachelor Informatik und die Summe der erreichten Leistungspunkte angezegit. 

Der Notendurchschnitt wird wiefolgt berechnet: 
Alle bestandenen und benoteten Veranstaltungen werden den jeweiligen Modulgruppen zugeordnet und daraus schrittweise der Gesamtdurchschnitt ermittelt.

Der Ablauf der Berechnung erfolgt wie folgt:
1. Modulgruppen durchlaufen
   Für jede vorhandene Modulgruppe werden alle zugehörigen Veranstaltungen gesucht.
2.	Geeignete Veranstaltungen auswählen 
   Es werden nur Veranstaltungen berücksichtigt, die:
	- bestanden sind
	- eine gültige Note besitzen (note >= 1.0)
	- zur aktuellen Modulgruppe gehören
3.	Gruppennote berechnen 
   Aus den ausgewählten Veranstaltungen wird eine Gruppennote als leistungspunkt-gewichteter Mittelwert berechnet.
4.	Gewichtung mit Modulgruppen-LP 
   Die Gruppennote wird mit den vorgesehenen Leistungspunkten der Modulgruppe (lp_todo) gewichtet und zum Gesamtergebnis addiert.

Unbenotete oder nicht bestandene Veranstaltungen werden ignoriert.
Falls keine bewerteten Leistungspunkte vorhanden sind, gibt die Funktion 0.0 zurück.

Rückgabewert: gewichteter Gesamtnotendurchschnitt (double).

<br><br><br><br>


>### Hilfe
Das Programm enthält eine zentrale **Hilfe** auf der die wesentlichen Programmfunktionen erklärt werden. Die Seite bietet dem Nutzer eine erste Anlaufstelle bei Fragen zur Nutzung des Programms. <br>

Die **Hilfe** kann von jeder Seite innerhalb des Programms, mit Ausnahme der Eingabedialoge, über ``[h]`` aufgerufen werden.
<br><br><br><br>
