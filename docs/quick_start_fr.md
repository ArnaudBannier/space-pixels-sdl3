Space Pixels : Guide de démarrage rapide
========================================

> [!NOTE]
> Installez l'extension GitHub Markdown Preview sur VSCode pour lire correctement ce document.

Bienvenue dans le guide de démarrage rapide de **Space Pixels** !

Ce guide vous accompagne dans la découverte et la prise en main du basecode du jeu Space Pixels, un shoot'em up développé avec SDL3.
Que vous soyez novice en développement de jeux ou développeur expérimenté, ce guide structuré vous permettra de comprendre rapidement l'architecture du projet et de commencer à créer votre propre jeu.

--------------------------------
## Comment utiliser ce guide ?

Ce guide est organisé en **parcours thématiques** que vous pouvez suivre dans l'ordre ou consulter selon vos besoins :

### Parcours recommandé pour les débutants

1. **Introduction** : Comprenez l'architecture globale du projet
2. **Tutoriel du jeu** : Apprenez à créer et animer des objets de jeu (joueur, ennemis, projectiles)
3. **Présentation de l'UI** : Découvrez le système d'interface utilisateur
4. **Tutoriel de l'UI** : Créez une page de statistiques complète
5. **Sprites sheets** : Maîtrisez les fichiers JSON de description des textures

### Consultation rapide

Chaque page peut être consultée indépendamment si vous recherchez une information précise :
- Besoin de comprendre l'héritage en C ? → Introduction
- Comment créer un ennemi ? → Tutoriel du jeu
- Comment ajouter un bouton ? → Tutoriel de l'UI
- Format des fichiers `_desc.json` ? → Sprites sheets

### Navigation

- Toutes les pages contiennent des **liens de navigation** en bas de page
- Utilisez **"Retour à l'index"** pour revenir à cette page
- Les liens **"Page précédente"** et **"Page suivante"** suivent l'ordre recommandé

--------------------------------
## Table des matières

### Découverte du jeu
  * [Introduction et architecture du projet](./quick_start_fr_intro.md)  
    *Présentation générale, version démo, concepts fondamentaux (GameObject, héritage en C, moteur physique)*

  * [Tutoriel de prise en main du basecode](./quick_start_fr_game.md)  
    *Développement pas à pas du joueur et des ennemis (affichage, déplacement, tirs, animations)*

### Découverte de l'interface utilisateur
  * [Présentation du système UI](./quick_start_fr_ui_1.md)  
    *Architecture des pages, UIObjects, layouts, widgets, focus manager*

  * [Tutoriel UI : création d'une page de statistiques](./quick_start_fr_ui_2.md)  
    *Ajout de boutons, navigation entre pages, tableaux formatés*

### Compléments techniques
  * [Fichiers de description JSON pour les spritesheets](./quick_start_fr_sprites.md)  
    *Structure complète des fichiers `_desc.json`, géométrie, groupes, bordures 9-slice, exemples*

--------------------------------
## Prérequis

Avant de commencer ce guide, assurez-vous que :

- Vous avez récupéré le basecode du projet
- CMake est installé et configuré
- Le projet compile sans erreur
- Vous pouvez lancer le jeu et voir le menu principal

Si ce n'est pas le cas, consultez le fichier [README.md](../README.md) à la racine du projet pour les instructions d'installation.

--------------------------------
## Conventions utilisées dans ce guide

- **Gras** : Termes techniques importants ou actions à effectuer
- `Code` : Noms de fichiers, fonctions, variables ou extraits de code
- `// [TODO XXX]` : Commentaires dans le code indiquant les zones à modifier
- Fichier à modifier : Indique quel fichier ouvrir
- Attention : Remarques importantes
- Astuce : Conseils et bonnes pratiques

--------------------------------

Bon apprentissage et amusez-vous bien avec Space Pixels !