# gradeviewer
A command-line application to manage your university grades

## Features
- Add and edit your courses and their credits
- Attach extra information like semester and module group
- Add your exam grades
- View your current average grade
- Persist all your data in a local JSON file

## Usage
*TODO*

## Build Instructions
*TODO*


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


>### Hilfe
Das Programm enthält eine zentrale **Hilfe** auf der die wesentlichen Programmfunktionen erklärt werden. Die Seite bietet dem Nutzer eine erste Anlaufstelle bei Fragen zur Nutzung des Programms. <br>

<!-- Die **Hilfe** kann von jeder Seite innerhalb des Programms, mit Ausnahme der Eingabedialoge, über ``[h]`` aufgerufen werden.  -->
<br><br><br><br>


## Presentation
1. **Franz**
   - Einleitung
   - Programm starten
   - Neue Veranstaltung anlegen
   - Betonung:
      - Benutzerfreundlichkeit: Hilfeseiten, Zusätzliche Kennzahlen (LP pro Semester und pro Modulgruppe), Farbliche Hervorhebungen, ausfürhliche Fehlerbehandlung und Weiterleitung des Nutzers an die Hilfe
    
2. **Jakob**
   - Programm beenden und neu starten
      - Betonung: **json parser** als besonderer Aspekt des Programms
   - Veranstaltung bearbeiten
  
3. **Adrian**
   - Notendurchschnitt (**Betonung**: Freiwillige Zusatzaufgabe wurde bearbeitet) -> Erklärung: Nach welcher Prüfungsordnung und wie genau wird der aktuelle Durchschnitt berechnet
   - Verwendung von KI
    
## Dokumentation: Aufteilung Adrian - Franz
**Franz**
- Programm starten
- Veranstaltungsübersicht
- Neue Veranstaltung anlegen
- Modulgruppe anlegen
- Hilfeseite

**Adrian**
- Veranstaltung bearbeiten
- Notendurchschnitt berechenen
