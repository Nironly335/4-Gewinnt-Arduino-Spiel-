## Authors

- Franz Regir
- Radmir Mullagaliev

# 4-Gewinnt (Connect Four)

Dieses Projekt implementiert das klassische Spiel "4 Gewinnt" auf einer 8x8 LED-Matrix mit zwei Spielern.

How-To:
1. Zwei Spieler setzen jeweils abwechselnd einen Punkt in die Matrix. Spieler 1 leuchtet immer, Spieler 2 blinkt.
2. Wenn ein Spieler dran ist, erscheint ein neuer (beständiger oder blinkender) Punkt auf der 8x8-Matrix. Dieser Punkt kann durch den linken und den rechten Button bewegt werden.
3. Wenn der bewegliche Punkt liegt, wo man ihn haben möchte, kann man den mittleren Button drücken, um die Eingabe zu bestätigen. Dadurch endet die Runde und der Gegenspieler ist dran.
4. Falls einer der Spieler 4 Punkte in einer Reihe hat (horizontal, vertikal oder diagonal), hat dieser gewonnen. Falls das Spielfeld voll wird, ohne dass dies eintritt, tritt ein Unentschieden auf.

Grober Ablauf einer Runde:
1. Punkte von Spieler 1 werden gesetzt.
   LCD gibt an, welcher Spieler dran ist und gibt kurze Instruktionen zu den Buttons.
3. Spieler kann den zu setzenden Punkt nach links und rechts bewegen.
  Dieser Loop wird gebrochen, sobald die Position des Punktes durch die mittlere Taste bestätigt wird.
  Währenddessen blinken die Punkte von Spieler 2.
4. Der Punkt wird gesetzt.
  Es wird geprüft, ob jemand gewonnen hat oder ein Unentschieden eingetreten ist.
  Falls ja:
    LCD gibt das Ergebnis an (Sieger oder Unentschieden) und bietet eine neue Runde an.
    Durch Druck des Mittleren Buttons (siehe LCD) wird das Spielfeld zurückgesetzt und ein neues Spiel beginnt.
  Falls nein:
    Der Spieler wechselt. Der andere Spieler ist nun am Zug und der Ablauf beginnt wieder in Schritt 1.
