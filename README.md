# 2021-2022_Cunnac_OyselMestre

# 1. Contexte et objectifs du projet

- Réaliser une jauge de contrainte low-tech à base de graphite: [Pencil Drawn Strain Gauges and Chemiresistors on Paper](https://www.researchgate.net/publication/259846610_Pencil_Drawn_Strain_Gauges_and_Chemiresistors_on_Paper)
- Concevoir un shield Arduino contenant l'amplificateur du capteur et les composants électroniques annexes (émetteur/récepteur Bluetooth, encodeur rotatif et écran OLED)
- Réaliser une application mobile connectée à l'émetteur Bluetooth du capteur sous Android Studio
- Évaluer le capteur sur un banc de test pour rédiger une datasheet

# 2. Capteur de contrainte

> Notre capteur de contrainte utilise une jauge en papier munie d'un circuit résistif en graphite. L'intérêt de l'utilisation d'une telle jauge vient de sa facilité de mise en oeuvre. La jauge est directement tracée au crayon à papier et les matériaux nécessaires (papier et crayon) sont aisément procurables à moindre coût. 

> Dans son principe, une jauge de contrainte est un circuit résistif dont la résistance varie avec sa déformation. En mesurant les variations de résistance de la jauge, il est possible de déduire la contrainte et la déformation appliquées. Dans ce projet, notre jauge de contrainte est alimentée par une tension régulée de 5V d'une carte Arduino UNO. La mesure du courant circulant dans la jauge constitue le signal du capteur, qui doit être amplifié et filtré.

  ## 2.1. Circuit amplificateur
 
 > La conductance des jauges en graphite que nous utilisons est très faible (de l'ordre de la dizaine de nS) et est alimentée par une tension continue de 5V. Le circuit amplificateur du capteur doit alors être en mesure d'extraire l'information d'un signal en courant de très faible intensité.
 > Dans ce projet, nous avons décidé d'utiliser un montage transimpédance constitué d'un amplificateur opérationnel (AOP) pour fournir un signal en tension suffisant au convertisseur analogique-numérique (ADC) d'une carte Arduino UNO.
 
 >   Notre jauge de contrainte est composée d'une seule résistance en série. De ce fait, notre signal est sensible aux dérives en tension de l'amplificateur. Notre AOP doit donc avoir un offset de tension en entrée très faible de manière à ce qu'il ne puisse fausser le signal fourni à l'ADC. Cela restreint notre choix d'amplificateur opérationnel. Nos jauges de contrainte en graphite ont une conductivité aux alentours de 10 nS. La jauge étant alimentée par 5V, nous nous attendons à avoir un signal en courant de l'ordre de 50 nA. En shuntant ce courant par une résistance de 100 kΩ, le signal en tension fourni à l'AOP est de 5 mV. À titre de comparaison, l'amplificateur [LM741](https://www.ti.com/lit/ds/symlink/lm741.pdf) présente un offset en entrée typique de 1 mV, et pouvant atteindre 5 mV. Ce composant n'est donc pas adapté pour notre utilisation. Ainsi notre amplificateur doit spécifiquement présenter un offset en entrée faible. Pour cette raison, nous utilisons le [LTC1050](https://www.analog.com/media/en/technical-documentation/data-sheets/1050fb.pdf), pour sa tension de dérive de 5 µV, soit 1000 fois plus faible que notre signal nominal de 5 mV.
 
 >   À partir de cet AOP, nous élaborons l'architecture du circuit amplificateur. Ce dernier dispose de trois étages de filtrage:
 >   - à l'entrée, un filtre passe-bas (R1C1) permet d'éliminer les nuisances RF provenant des dispositifs environnants (type émetteur bluetooth)
 >   - un autre filtre (R3C4) couplé à l'AOP permet de filtrer la composante du bruit à 50 Hz provenant du réseau électrique
 >   - à la sortie de l'amplificateur, un dernier filtre (R5C2) permet de traiter les bruits dus à la fréquence d'échantillonage de l'ADC

 >   La capacité C3 sert à filtrer les irrégularités de la tension d'alimentation de l'amplificateur. La résistance R2 sert à calibrer l'amplificateur sur le domaine de tension souhaité, qui est celui de l'ADC du micro-contrôleur. Lors de la phase de prototypage du circuit, nous avons utilisé un potentiomètre digital à la place de cette résistance pour trouver sa valeur. Enfin, la résistance R4 protège l'AOP contre les décharges électrostatiques et constitue un filtre RC avec la capacité C1 pour les bruits en tension.

![Figure 1: Circuit amplificateur transimpédance](analog_circuit_images/analog_circuit.JPG "Circuit amplificateur transimpédance")

**Figure 1 - Circuit amplificateur transimpédance**
 
> Pour vérifier le fonctionnement du circuit sur LTSpice, nous simulons la déformation de la jauge par un pulse de tension en entrée (Figure 2). Ce pulse de tension génère une variation du courant en entrée de l'AOP. Virtuellement, cela représente la variation de conductance de la jauge due à sa déformation. Pour cette simulation, nous faisons varier la conductance de 5 nS à 20 nS, ce qui correspond à l'ordre de grandeur des jauges resistives utilisées. Ce faisant, nous balayons à peu près tout l'intervalle de signaux d'entrée possibles:
S
![Figure 2: Circuit amplificateur transimpédance pour simulation](analog_circuit_images/analog_circuit_full_sim.JPG "Circuit amplificateur transimpédance")

**Figure 2 - Simulation du fonctionnement du circuit amplificateur. Un pulse de tension permet de modéliser la déformation de la jauge et la variation du signal en courant.**
 

![Figure 3: Resultats de la simulation](analog_circuit_images/simulation_Vpulse.JPG)

**Figure 3: Résultats de la simulation. En vert, le signal en courant fourni par la jauge et en bleu la tension de sortie de l'AOP.**

> Sur la plage de conductance prise en compte, le signal de sortie passe d'environ 240 mV à 1.05 V, ce qui ne sature pas l'ADC. L'amplitude crête-à-crête du bruit en 50 Hz est de 14 mV RMS. Ce signal est donc détectable par l'ADC qui a une sensibilité d'environ 5 mV. Le rapport signal sur bruit est de 25.6 dB pour la plus faible conductance et de 37.2 dB pour la conductance la plus élevée. Si notre signal sur cette simulation n'est fondamentalement pas considéré comme bon en terme d'instrumentation (rapport signal sur bruit inférieur à 40 dB), remarquons que naturellement, plus le signal de sortie est élevé (i.e. plus la conductance est élevée), plus le bruit à 50 Hz est écrasé par le signal. Cela signifie que plus la conductance de la jauge est élevée, plus nos mesures seront fiables et précises. Pour cette raison, nous avons tout intérêt à utiliser des jauges de contrainte de résistance relativement faibles (autour de 50 MΩ ou moins) pour nos mesures. Pour avoir une plus grande liberté sur les conductances des jauges utilisées, nous aurions pu intégrer directement sur le PCB un potentiomètre digital à la place de R2. Cela permettrait (après une étape de calibration) d'adapter le signal de sortie à la jauge résistive utilisée pour obtenir un signal optimal. Néanmoins, si nous nous restreignons à utiliser des jauges aux conductances assez élevées, alors nous obtenons un signal tout à fait acceptable.

  ## 2.2. Intégration Arduino
  > Notre capteur de contrainte est intégré avec son circuit amplificateur sur un circuit imprimé (PCB) que nous avons conçu sur le logiciel KiCad. Ce PCB est un shield pour la carte Arduino UNO qui pilote le capteur et qui intègre plusieurs composants annexes:
  > - un module de communication Bluetooth HC-05 qui permet d'envoyer les données du capteur à un smartphone doté de l'application liée au capteur
  > - un écran OLED I2C couplé à un encodeur rotatoire KY-040 qui sert d'interface utilisateur et permet de sélectionner le mode de fonctionnement du capteur (affichage de la valeur de résistance et/ou envoi des données bluetooth)

> <p align="center">
> <img src="pcb_shield_app_images/pcb_screenshot.JPG"/>
> </p>

**Figure 4 - Design du PCB. Notez que la masse reliée à l'OLED et à l'encodeur est connectée à la masse de l'Arduino par un mince canal passant derrière les broches digitales 0-7 (tout en haut du PCB). Il aurait été préférable d'avoir une meilleure connexion entre les différentes zones de masse du circuit imprimé.**

  > Un défaut de fabrication du PCB a court-circuité la broche numérique 5 Arduino normalement non-connectée à la masse. Pour remédier à ce problème, nous avons dû gratter le PCB au scalpel pour déconnecter cette broche. Cependant, lors de cette opération, une partie du PCB normalement reliée à la masse est devenue flottante. La zone de masse en question alimentait l'écran OLED et l'encodeur, ce qui les rendait inutilisables. Pour rectifier cette erreur, nous avons relié cette piscine à la masse du circuit par un fil.

  > Une fois le perçage, les soudures et les rectificatifs du PCB faits, nous obtenons un shield Arduino opérationnel. Nous avons également pu effectuer nos premières mesures avec l'application mobile:

 > <div class="row" align="center">
 > <img src="pcb_shield_app_images/photo_shield.jpg"/>
 > <img src="pcb_shield_app_images/pcb_and_app_pic.jpg"/>
 > </div>

**Figure 5 - En haut: shield opérationnel alimenté par une carte Arduino UNO. L'écran OLED affiche le menu déroulant navigable à l'aide de l'encodeur rotatoire. En bas: capteur en fonctionnement connecté à l'application mobile (première version).**

# 3. Programme Arduino

> Le programme Arduino permet de calculer la valeur de la résistance du capteur à partir de la valeur de tension mesurée par l'ADC de la carte. Cette mesure résistive est ensuite envoyée à l'application mobile via Bluetooth si un message de demande a été envoyé par l'application. L'encodeur rotatif permet d'activer un menu sur l'OLED de choix d'affichage sur celui-ci : soit on affiche la valeur de la résistance, soit on affiche la valeur de la tension. On peu également choisir d'afficher une image ou les crédits (défilants).
> A la mise en route du programme, une animation d'initialisation apparaît sur l'écran. Après cela, on a directement accès à la valeur résistive mesurée par le capteur. En tournant l'encodeur rotatoire, on peut rentrer dans le menu et sélectionner l'affichage de son choix toujours grâce à la rotation de l'encodeur. Ensuite, on valide le choix en faisant une pression sur l'encodeur.

# 4. Application mobile

  > Dans le cadre de ce projet, nous avons programmé sur Android Studio l'application mobile connectée au module Bluetooth du capteur. Cette dernière est constituée d'une seule activité sur laquelle il est possible d'acquérir les données du capteur en temps réel et de les tracer sur un graphique dynamique. Une fois les données acquises, il est possible de les exporter sous forme de texte par email ou SMS. L'application propose également d'ouvrir toute une variété d'applications pour l'export des données (réseaux sociaux et autres). Un bouton d'aide en bas de l'activité ouvre une fenêtre surgissante pour informer l'utilisateur du fonctionnement de l'application. Pour l'affichage des données sur le graphique, nous avons utilisé les bibliothèques en libre accès de la compagnie [AnyChart](https://www.anychart.com/).

 > <div class="column" align="center">
 > <img src="pcb_shield_app_images/SensorApp_screenshot.JPG" alt="300" width="300"/>
 > <img src="pcb_shield_app_images/SensorApp_export_data_screenshot.jpg" alt="300" width="300"/>
 > </div>

 **Figure 6 - À gauche: page d'accueil de la dernière version de l'application mobile. Cette capture d'écran a été prise pendant le fonctionnement du capteur. À droite: fenêtre surgissante permettant l'export des données du capteur.**

 > Pour la connection Bluetooth, le smartphone doit être appareillé au module du capteur avant de pouvoir se connecter via l'application mobile. L'application ne permet pas de choisir à quel module se connecter. En réalité, l'application est liée à un module bluetooth en particulier (celui fourni avec le capteur) et lui seul pourra être connecté au smartphone pour la transmission de données. Une piste d'amélioration consiste à afficher la liste des dispositifs appareillés pour se connecter au module souhaité. Nous n'avons cependant pas pu implémenter cette fonctionnalité par manque de temps.

 > N.B: le fichier .apk de l'application mobile est situé dans le répertoire suivant: SensorApp/app/release/. Dans ce dossier se trouve aussi  le fichier .aab (Android App Bundle), correspondant au nouveau format de publication d'applications Android ayant l'avantage d'être plus compact que le format apk (plus d'informations [ici](https://developer.android.com/guide/app-bundle)).

# 5. Banc de test, datasheet et discussions
  > 
