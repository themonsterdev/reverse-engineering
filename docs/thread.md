# Thread (informatique)

## Sommaire

- Introduction
- Utilisations
	- Interface graphique (GUI)
	- Programme de calcul intensif
- Threads et multitâche
- Références

## Introduction

Un thread est similaire à un processus car tous deux représentent l'exécution d'un ensemble d'instructions du langage machine d'un processeur.
Du point de vue de l'utilisateur, ces exécutions semblent se dérouler en parallèle. Toutefois, là où chaque processus possède sa propre mémoire
virtuelle, les threads d'un même processus se partagent sa mémoire virtuelle. Par contre, tous les threads possèdent leur propre pile d'exécution.

## Utilisations

Les threads sont classiquement utilisés avec l'interface graphique (`Graphical user interface`) d'un programme, pour des attentes asynchrones dans les
télécommunications ou encore pour des programmes de calcul intensif (comme l'`encodage d'une vidéo`, les `simulations mathématiques`, etc.).

### Interface graphique (GUI)

Dans le cas d'une interface graphique, les interactions de l'utilisateur avec le processus, par l'intermédiaire des périphériques d'entrée, sont gérées
par un thread, technique similaire à celle utilisée pour les attentes asynchrones. Les calculs lourds sont gérés par un ou plusieurs autres threads. Cette
technique de conception de logiciel est avantageuse dans ce cas, car l'utilisateur peut continuer d'interagir avec le programme même lorsque celui-ci est en
train d'exécuter une tâche. Une application pratique se retrouve dans les traitements de texte où la correction orthographique est exécutée tout en permettant
à l'utilisateur de continuer à entrer son texte. L'utilisation des threads permet donc de rendre l'utilisation d'une application plus fluide, car il n'y a plus
de blocage durant les phases de traitements intenses.

### Programme de calcul intensif

Dans le cas d'un programme de calcul intensif, l'utilisation de plusieurs threads permet de paralléliser le traitement, ce qui, sur les machines multiprocesseur, permet de l'effectuer bien plus rapidement.

## Threads et multitâche

Les threads partagent une information sur :

- L'état du processus
- Les zones de mémoires

Ainsi que d'autres ressources. En fait, à part leur pile d’appel, des mécanismes comme le `Thread Local Storage` et quelques rares exceptions spécifiques à
chaque implémentation, les threads partagent tout.

Puisqu'il n'y a pas de changement de mémoire virtuelle, la commutation de contexte (context switch) entre deux threads est moins coûteuse en temps que la
commutation de contexte entre deux processus. On peut y voir un avantage de la programmation utilisant des threads multiples.

## Références

- [Thread (informatique)](https://fr.wikipedia.org/wiki/Thread_(informatique))
