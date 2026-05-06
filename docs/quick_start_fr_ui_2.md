Space Pixels : Tutoriel de l'UI
===============================

[Retour à l'index](./quick_start_fr.md)

Une fois le tutoriel de prise en main du basecode terminé, nous allons ajouter une page de statistiques à la page de fin de niveau.
Actuellement, la page de fin n'affiche qu'un message de victoire ou de défaite avec un retour direct au menu principal.
Nous allons enrichir cette interface en ajoutant un bouton "Stats" sur la page de fin permettant d'ouvrir une nouvelle page de statistiques affichant les scores et points de vie des joueurs dans un tableau formaté.
Comme pour le tutoriel de prise en main du basecode, vous devrez décommenter et comprendre des blocs de code pour effectuer les modifications.

--------------------------------
# Vue d'ensemble de l'architecture

Le système d'interface du niveau repose sur un `LevelUIManager` qui gère les pages indépendantes suivantes :
- `LevelHUD` : l'affichage tête haute pendant le jeu (toujours actif durant le niveau)
- `LevelPausePage` : la page de pause (activée via la touche Entrée)
- `LevelEndPage` : la page de fin de niveau (activée à la victoire ou la défaite)
- `LevelStatsPage` *(à ajouter)* : la page de statistiques détaillées (activée depuis la page de fin)

Le `LevelUIManager` fonctionne comme un système de commandes : on ouvre ou ferme des pages via `LevelUIManager_openPage()` et `LevelUIManager_closePage()`, et le manager se charge de créer ou détruire les instances correspondantes dans sa méthode `update()`.
Cette architecture garantit qu'aucune page n'est détruite pendant son propre traitement, évitant ainsi les erreurs de pointeurs invalides.

--------------------------------
# [TODO level end page] - Amélioration de la page de fin

> [!NOTE]
> Fichier à modifier : `game/ui/level_end_page.c`

Actuellement, la page de fin (`LevelEndPage`) affiche uniquement un titre et attend une pression sur Entrée pour retourner au menu principal.
Nous allons remplacer ce comportement simpliste par une interface plus riche avec deux boutons : "Main Menu" pour revenir au titre, et "Stats" pour ouvrir la page de statistiques.

## Ajout du layout et des boutons dans create()

La première étape consiste à créer les boutons et les positionner sur la page.
Nous allons utiliser un `UIGridLayout` pour disposer proprement le titre et les deux boutons en trois lignes verticales.

> level_end_page.c -- LevelEndPage_create()
>
> Décommentez le bloc permettant de créer le layout formé de 3 lignes et 1 colonne.

Ce layout définit des lignes de 25 unités de hauteur avec un espacement de 5 unités entre elles, sauf entre la ligne 0 (titre) et la ligne 1 (premier bouton) où l'espacement est de 20 unités pour créer une séparation visuelle.

Le label "title-label" est actuellement attaché directement au `m_mainPanel`, ce qui le positionne manuellement au centre de l'écran.
Nous voulons qu'il soit géré par le layout pour un positionnement cohérent avec les boutons.

> level_end_page.c -- LevelEndPage_create()
>
> Remplacez le parent du label pour le rattacher au layout nouvellement créé au lieu du `mainPanel`.

Maintenant, créons les deux boutons et attachons-les au layout.

> level_end_page.c -- LevelEndPage_create() à la fin
>
> Décommentez le bloc complet de création des boutons en fin de fonction.

Observez le pattern de création des boutons : création des boutons, définition du texte, rattachement au callback, application du style, ajout au layout et relation avec le FocusManager.

## Ajout du callback pour les boutons

Nous devons maintenant définir une fonction de callback qui sera appelée lorsque l'utilisateur clique sur un bouton.
Cette fonction recevra un pointeur vers le `UISelectable` (le bouton cliqué) et devra identifier quel bouton a déclenché l'événement pour exécuter l'action appropriée.

> level_end_page.c -- LevelEndPage_onClick()
>
> Décommentez le bloc complet de la fonction.

Observez le fonctionnement du callback : récupération du pointeur sur les données utilisateurs, identification du nom du bouton qui a généré le callback, et action en accord avec le nom du bouton. 
L'action liée à la page de stats est encore en commentaires car la page n'existe pas encore.

## Suppression de l'ancien système de retour au menu

L'ancienne implémentation de `LevelEndPage_update()` écoutait directement les touches Entrée et Pause pour retourner au menu.
Maintenant que nous avons des boutons, cette logique n'est plus nécessaire : le `UIFocusManager` gère automatiquement la navigation et l'activation des boutons.

> level_end_page.c -- LevelEndPage_update()
>
> Commentez ou supprimez le bloc écoutant les actions utilisateur.

La fonction `LevelEndPage_update()` ne contient désormais plus qu'un seul appel : `UIFocusManager_update()`, qui suffit à gérer toute l'interaction utilisateur avec les boutons.
La page de fin est maintenant visuellement complète, mais pas encore fonctionnelle.
Il faut encore relier les boutons à une action spécifique et définir la page de stats.

--------------------------------
# [TODO stats page] Déclaration de la nouvelle page dans le manager

> [!NOTE]
> Fichiers à modifier : `game/ui/level_ui_manager.h`, `game/ui/level_ui_manager.c`

Maintenant que la page de fin possède un bouton "Stats" (qui ne fait rien pour le moment), nous devons créer la page de statistiques elle-même.
Avant cela, nous devons l'enregistrer auprès du `LevelUIManager`.
Cela implique quatre modifications : ajouter l'énumération de la page, déclarer le pointeur vers la structure, initialiser ce pointeur à `NULL` dans `create()`, et gérer son cycle de vie dans `applyPageChange()` et `update()`.

## Ajout de l'énumération et du pointeur

Le fichier `level_ui_manager.h` contient l'énumération `LevelUIPageId` qui liste toutes les pages disponibles, ainsi que la structure `LevelUIManager` qui contient les pointeurs vers chaque page.
Il faut donc ajouter à l'enum et au manager la présence de cette nouvelle page.

> level_ui_manager.h
>
> Décommentez les lignes permettant :
> - la définition de la structure de la `LevelStatsPage`,
> - la définition de l'ID de la nouvelle page dans l'enum,
> - la définition d'un pointeur sur la nouvelle page dans le Manager.

Observez que l'énumération se termine par `LEVEL_UI_PAGE_COUNT`, qui permet au manager de connaître le nombre total de pages sans avoir à le coder en dur.

## Initialisation et destruction de la page

Le fichier `level_ui_manager.c` implémente les méthodes du manager.
La fonction `LevelUIManager_create()` alloue la mémoire et initialise tous les pointeurs de page à `NULL`, et `LevelUIManager_destroy()` doit les détruire proprement.

> level_ui_manager.c -- LevelUIManager_create()
>
> Décommentez les lignes permettant :
> - d'inclure le header de la page de stats en haut du fichier ;
> - d'initialiser le pointeur à `NULL` dans la fonction `LevelUIManager_create()`.

> level_ui_manager.c -- LevelUIManager_destroy()
>
> Décommentez la ligne pour détruire la page lors de la destruction du manager.

## Gestion du cycle de vie dans applyPageChange()

La fonction `LevelUIManager_applyPageChange()` est le cœur du système de gestion des pages.
Elle est appelée au début de chaque `update()` et vérifie pour chaque page si elle doit être créée ou détruite.
Observez que le pattern est identique pour toutes les pages : test de `m_activePages[LEVEL_UI_PAGE_XXX]`, création conditionnelle avec appel à `XXXPage_create()`, puis destruction conditionnelle avec appel à `XXXPage_destroy()`.

> level_ui_manager.c -- LevelUIManager_applyPageChange()
>
> Décommentez le bloc permettant de gérer la création et la destruction de la page de statistiques.


## Appel de la mise à jour de la page

Enfin, la fonction `LevelUIManager_update()` doit appeler la méthode `update()` de chaque page active.

> level_ui_manager.c -- LevelUIManager_update()
>
> Décommentez le bloc permettant de vérifier l'existence de la page et de réaliser l'appel à la fonction de mise à jour de la page de stats.


## Activation du bouton Stats dans la page de fin

Maintenant que le manager connaît la page de statistiques, nous pouvons activer le bouton "Stats" sur la page de fin.

> level_end_page.c -- LevelEndPage_onClick()
>
> Décommentez les lignes permettant d'activer la transition vers la page de stats lors de l'activation du callback à travers le bouton de stats.


--------------------------------
# [TODO stats page] - Création de la page de statistiques

> [!NOTE]
> Fichiers à modifier : `game/ui/level_stats_page.h`, `game/ui/level_stats_page.c`

Nous allons maintenant créer la page de statistiques qui affiche un tableau avec les scores et points de vie des joueurs.
Contrairement aux pages précédentes, cette page ne contient aucun bouton : elle affiche uniquement des informations et attend une pression sur Entrée ou Échap pour revenir à la page de fin.

> level_stats_page.c et level_stats_page.h
>
> Décommentez l'intégralité des lignes des fichiers.

Observez que la structure `LevelStatsPage` est identique à celle des autres pages : elle contient un pointeur vers le `GameContext`, le `LevelUIManager`, un `UIFocusManager` (même si aucun élément interactif n'est présent, il est créé pour la cohérence), et le `m_mainPanel` qui servira de racine à l'arbre d'objets UI de la page.

## Détails de la fonction create()

Les premières lignes créent la structure `LevelStatsPage`, initialisent ses membres, créent le panneau principal et le fond semi-transparent.
Ce pattern est identique à toutes les pages.

### Layout principal (2 lignes)

Le layout principal est une grille de 2 lignes et 1 colonne :
- Ligne 0 : le titre "Level Stats"
- Ligne 1 : le tableau de statistiques

```c
UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 2, 1);
UIGridLayout_setRowSizes(layout, 25.0f);
UIGridLayout_setRowSpacing(layout, 0, 20.f);
```

### Titre

Un simple `UILabel` affiché en blanc avec la police titre, ajouté à la ligne 0 du layout.

### Fond du tableau

Un `UIImage` utilisant le sprite de bouton avec une teinte cyan est ajouté comme fond visuel du tableau.
Le `sortingLayer.orderInLayer = -1` garantit qu'il s'affiche derrière les textes.

### Layout du tableau (tableLayout)

Le layout du tableau est dynamique : il a 2 lignes (score et vie) et `playerCount + 1` colonnes (une pour les labels "Score" et "Life", puis une colonne par joueur).

```c
UIGridLayout* tableLayout = UIGridLayout_create(uiSystem, "table-layout", 2, playerCount + 1);
UIGridLayout_setColumnSize(tableLayout, 0, 75.0f);  // Colonne des labels plus large
UIGridLayout_setPadding(tableLayout, Vec2_set(20.f, 10.f));  // Marges intérieures
```

Notez l'utilisation de `UIGridLayout_getMinimumSize()` pour calculer la taille minimale requise par le tableau en fonction de son contenu, puis l'ajustement de la ligne 1 du layout principal pour correspondre exactement à cette taille.
Cette technique garantit que le fond du tableau (l'`UIImage`) s'adapte automatiquement au nombre de joueurs.

### Labels des lignes

Deux labels "Score" et "Life" sont créés et ajoutés à la colonne 0 (lignes 0 et 1).

### Valeurs pour chaque joueur

Une boucle sur `playerCount` crée les labels de valeurs pour chaque joueur.
Les valeurs sont extraites de `g_gameConfig.playerScores[]` et `g_gameConfig.playerHPs[]`, formatées avec `snprintf()`, et affichées dans les colonnes `i+1`.

## Détails des fonctions destroy() et update()

La fonction `destroy()` suit le pattern habituel : détruire le `UIFocusManager`, le panneau principal (qui détruit récursivement tous ses enfants), puis libérer la structure.

La fonction `update()` est très simple : elle appelle `UIFocusManager_update()` (même si aucun élément interactif n'est présent), puis vérifie si l'utilisateur a appuyé sur Échap (`cancelPressed`) ou Entrée (`validatePressed`).
Si c'est le cas, elle ferme toutes les pages et rouvre la page de fin avec `LevelUIManager_closePages()` et `LevelUIManager_openPage()`.


--------------------------------
# Aller plus loin

Maintenant que vous maîtrisez la création de pages UI, vous pouvez :
- Ajouter d'autres statistiques au tableau (ennemis détruits, précision, temps de jeu...)
- Créer une page de sélection de niveau avec des boutons pour chaque niveau
- Implémenter un système de sauvegarde des meilleurs scores
- Ajouter des animations de transition entre les pages
- Créer un HUD plus riche pendant le jeu (mini-carte, barre de vie graphique...)

Le système UI du moteur est suffisamment flexible pour supporter toutes ces fonctionnalités.
N'hésitez pas à explorer les fichiers d'en-tête dans `engine/src/ui/` pour découvrir toutes les options disponibles !

--------------------------------
- [Retour à l'index](quick_start_fr.md)
- [Page précédente](quick_start_fr_ui_1.md)
- [Page suivante](quick_start_fr_sprites.md)