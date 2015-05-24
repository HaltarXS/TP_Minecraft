#OLD
# TP_Minecraft
Projet d'IA pour un Minecraft maison

Le but est de mettre en place un github (en partant de mon projet, désolé ahah) dans lequel je (et ceux qui veullent) vais mettre en place l'architecture UML que j'ai donné
En gros, on aurait juste un système d'héritage/interface à mettre en place pour faire nos propre IA qu'on mettra dans un dossier a notre nom ou autre !

<h1>TO DO</h1>
- RessourceManager (Je commence ce soir - 31/03)
- Fabrique Ressource ( "   "   "   "   "   "   " )
- IAManager
- Fabrique IA
- Définir IABase
- Système d'états

<h2>RessourceManager</h2>
Créer au lancement du programme, c'est lui qui va connaitre l'ensemble des ressources du jeu.
Il va faire appel à une fabrique pour créer les différentes Ressources qui seront présentes dans le jeux.
Il pourrait par exemple fournir à une IA l'emplacement le plus proche d'une ressource particulière.

<h2>Fabrique Ressource</h2>
Appeller par le RessourceManager pour instancier les ressources.

<h2>IAManager</h2>
Créer au lancement du programme, c'est lui qui va connaitre l'ensemble des IAs du jeu.
Il va faire appel à une fabrique pour créer les différentes IAs qui seront présentes dans le jeux.
Il va à chaque frame appeller faire l'Update() d'un nombre limité d'IA afin de répartir limiter la charge de traitement.

<h2>Frabrique IA</h2>
Appeller par le IAManager pour instancier les IAs.

<h2>IABase</h2>
La classe mère des IAs. Il est nécessaire que chaque IA hérite d'elle ou d'une de ses filles afin d'assurer le bon fonctionnement de l'ensemble des IAs.
Elle va posser les attributs et méthodes de bases nécessaire à sa survie.
Elle aurat également un système d'état afin de définir le comportement qu'elle devra suivre.

<h2>Système d'états</h2>
La mise en place d'un système d'états me parait le plus intéressant (mais peut tout à faire etre modifier).
Le principe serait qu'une IA sera à un moment T dans un état (ex: AllerManger). Cette état serait appeler à l'Update() de l'IA et prendrait en paramètre l'IA appelante afin de déterminer s'il faut continuer cette état ou en changer (ex: Ai je faim ? Non => Determiner nouvel état; Oui => Ressource en vue ? Oui => Aller et manger; Non => Aller plus loin/ demander au ressource manager)

<h1>States Machines avec Messages - Guillaume Pastor</h1>
Système de machines à états avec des messages pour que les entités puisse communiquer entre elles. 
Exemple d'entité commenté utilisant toutes les fonctionnalités des states machines dans GuillaumePastor/StateMachineTest/TestStateMachine.cpp
Les futures entités à créer devront hériter de StateMachine et devront surcharger la méthode States() pour utiliser les states machines et les messages.

<h1>Pathfinding - Guillaume Pastor</h1>
Pathfinding. La classe est un singleton. 
Il y a deux fonctions FindPath() : 
*La première permet de trouver un chemin dans des types de cubes donnés en paramètre. 
Elle prend en compte les champs de bits pour la recherche dans plusieurs types de cubes. Voir l'exemple dans main.cpp.
*La deuxième permet de trouver un chemin sur le sol elle prend en paramètre une step (taille de cube franchissable).
L'entité va trouver un chemin passant sur n'importe quel type de cube sauf l'eau, sauf si le dernier paramètre est à true.
Voir Pathfinding.cpp et les exemples d'utilisation dans main.cpp

N'hésitez pas à faire vos retour sur le groupe facebook.
