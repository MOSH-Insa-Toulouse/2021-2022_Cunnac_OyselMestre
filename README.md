# 2021-2022_Cunnac_OyselMestre

MOSH INSA Toulouse: readme 2 qualité

# 1. Contexte et objectifs du projet

- Réaliser une jauge de contrainte low-tech à base de graphite [Pencil Drawn Strain Gauges and Chemiresistors on Paper](https://www.researchgate.net/publication/259846610_Pencil_Drawn_Strain_Gauges_and_Chemiresistors_on_Paper)
- Concevoir un shield Arduino contenant l'amplificateur du capteur et les composants électroniques annexes (émetteur/récepteur Bluetooth, encodeur rotatif et écran OLED)
- Réaliser une application mobile connectée à l'émetteur Bluetooth du capteur sous Android Studio
- Évaluer le capteur sur un banc de test pour rédiger une datasheet
- Comparer la technologie mise en oeuvre avec autres types de jauges de contrainte (**on n'y est pas du tout pour le moment mais tout va bien se passer**)

# 2. Capteur de contrainte

> Notre capteur de contrainte utilise une jauge en papier munie d'un circuit résistif en graphite. L'intérêt de l'utilisation d'une telle jauge vient de sa facilité de
> mise en oeuvre. La jauge est directement tracé au crayon à papier et les matériaux nécessaires (papier et crayon) sont aisément procurables à moindre coût. 

  ## 2.1. Circuit amplificateur
  > Bonjour je suis le circuit amplificateur au LTC1050
  ## 2.2. Shield Arduino et autres composants
  > Parce qu'on aime la créativité au gp, notre capteur dispose d'un émetteur BT et est fourni avec un afficheur OLED muni d'un encodeur rotatoire. Ça c'est du capteur !
# 3. Application mobile
  > Une application mobile codée sous Android Studio (***Kotlin***) reçoit les informations du capteur par communication BT et permet d'exporter les données par SMS. Ça c'est le futur de l'instrumentation.
# 4. Banc de test, datasheet et discussions
  > On ne sera pas très bavards ici
