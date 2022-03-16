# Injection de DLL

## Sommaire

- Introduction
- Windows
	- Diagramme de classe (Windows)

## Introduction

Une **injection de DLL** est une technique utilisée pour exécuter du code dans l' espace d'adressage d'un autre processus en
le forçant à charger une bibliothèque de liens dynamiques.

L'**injection de DLL** est souvent utilisée par des programmes externes pour influencer le comportement d'un autre programme d'une manière que
ses auteurs n'avais pas prévue.

Par exemple, le code injecté pourrait [accrocher les appels de fonction](hooking.md) système, ou lire le contenu des zones de texte de mot de passe,
ce qui ne peut pas être fait de la manière habituelle. Un programme utilisé pour injecter du code arbitraire dans des processus arbitraires est appelé
un **injecteur DLL**.

## Windows

Il existe plusieurs façons sur **Windows** de forcer un processus à charger et à exécuter du code d'un DLL, je vais vous en montrée une qui permet
de le faire dans un processus en cours d'exécution.

### Diagramme de classe (Windows)

![Injection de DLL](img/injectModule.png)
