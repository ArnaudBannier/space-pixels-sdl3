Space Pixels : Guide de démarrage rapide
========================================

> [!NOTE]
> Installez l'extension GitHub Markdown Preview sur VSCode pour lire correctement ce document.

La première partie de ce document présente l'architecture globale du projet et les notions d'héritage utilisées.
La seconde partie est un tutoriel permettant une prise en main rapide du basecode afin d'y apporter toutes les modifications souhaitées.

--------------------------------
# Étude du basecode

Avant de faire l'étude détaillée du basecode, il est nécessaire de préciser quelques généralités concernant le développement de ce jeu.
Et pour cela, quoi de mieux que de demander à une IA d'analyser l'ensemble des sources et d'en faire un résumé (tout de même relu et adapté par vos enseignants) ?

> Space Pixels est un jeu de type shoot'em up dont les sources sont séparées en deux parties : le moteur générique (`engine/`) et la logique applicative (`application/`).
> Le moteur fournit une infrastructure technique généraliste pouvant être réutilisée pour d'autres jeux : système de rendu par commandes différées avec SDL3, gestionnaire d'animations sprites basé sur des atlas JSON, moteur physique 2D avec détection de collisions par broad-phase, système de particules, caméra, timer, et widgets UI préfabriqués.
> Le moteur physique utilise une approche de séparation des axes (SAT) pour la détection de collisions, avec des catégories et masques de collision permettant de définir précisément les interactions entre les différents types d'objets (joueurs, ennemis, projectiles, bonus).
> L'application exploite ce moteur pour implémenter le gameplay spécifique : elle définit une hiérarchie d'objets de jeu (`GameObject`) par composition où tous les éléments (joueur, ennemis, projectiles, background ...) héritent d'une structure commune avec des méthodes virtuelles (onUpdate, onFixedUpdate, onRender, onTakeDamage, etc.).
>
> Le jeu s'articule autour de deux scènes principales (menu titre et niveau de jeu) gérées par un `SceneManager`, avec une boucle de jeu à pas fixe (100 FPS pour la physique) et un `GameContext` central donnant accès aux systèmes partagés (assets, inputs, physique, rendu, audio).
> Les niveaux sont définis par des vagues d'ennemis scriptées.

## Version démo
La version démo permet de vous rendre compte des possibilités techniques atteignables à partir du basecode fourni.
On retrouve au lancement du jeu une première _scène_ composée d'une _page_ de titre et d'une _page_ d'options.
Une fois le jeu lancé au travers d'une nouvelle _scène_, deux joueurs sont présents et tirent des projectiles sur des ennemis qui apparaissent et disparaissent en vagues successives.
Une fois l'ensemble des ennemis éliminés, une page de fin permet de revenir au menu principal.

L'interface utilisateur est fonctionnelle au clavier, à la souris et à la manette. Les contrôles au clavier sont les suivants :
- **haut**, **bas**, **gauche**, **droite** pour les déplacements,
- **left ctrl** pour augmenter la vitesse de déplacement,
- **espace** pour tirer,
- **x** pour tirer avec canon spatial (si l'arme est disponible),
- **w** (en AZERTY) ou **z** (en QWERTY) pour tirer une salve de missiles (si l'arme est disponible),
- **enter** pour ouvrir/fermer le menu de pause.

Lors du déroulement du jeu, il est possible d'utiliser les touches F1, F2, F3 et Tabulation pour le débogage.
- La touche F1 permet de dérouler le jeu en mode pas à pas : l'appui sur la touche Tab permet de passer à la frame suivante. Il est ainsi possible d'observer précisément le comportement du jeu à chaque étape de son déroulement. Pour revenir au mode normal, il suffit de réappuyer sur F1.
- Les touches F2 et F3 permettent d'afficher les gizmos (informations de débogage visuelles). La touche F2 affiche les gizmos liés à la physique (boîtes de collision) tandis que la touche F3 affiche les gizmos liés à l'interface utilisateur (zones de clic, éléments interactifs...).

## Généralités

Le projet Space Pixels est architecturé selon une séparation claire entre le **moteur de jeu** (`engine/`) et l'**application** (`application/`).
Le dossier `engine/` contient l'infrastructure technique réutilisable.
Cette couche fondamentale n'a normalement pas besoin d'être modifiée.
Le dossier `application/` regroupe quant à lui tout le code spécifique au jeu.
C'est ce dossier qui doit recevoir l'ensemble de vos améliorations.
Étudions plus en détail le contenu de ces deux dossiers.

### Dossier `engine` : un moteur généraliste de jeu 2D

Le dossier `engine/` contient l'ensemble de l'infrastructure technique nécessaire au fonctionnement du jeu.
Séparé en trois dossiers principaux, ce moteur est réutilisable dans le cadre du développement d'autres jeux car il contient les briques élémentaires de toute application graphique.
Le dossier `core/` contient les primitives de base telles que la gestion des animations, des particules, de la caméra ou du temps.
Le dossier `ui/` est quant à lui spécialisé dans la gestion de l'interface utilisateur avec des composants préfabriqués (boutons, listes...), une gestion des entrées spécifique à l'interface et un système de style pour personnaliser l'apparence des éléments.

Les images servant à créer les animations du jeu sont basées sur des **atlas de textures**  (fichiers PNG regroupant plusieurs sprites) et des **fichiers de description JSON** (découpage de l'atlas en groupes de sprites nommés, par exemple : "idle", "firing", "dying", "shield").
Ces fichiers JSON spécifient la géométrie de la grille (nombre de lignes/colonnes, espacement) et associent chaque groupe de sprites définissant une animation à une liste d'indices de frames.
Le `SpriteAnimManager` centralise la gestion des animations : chaque animation est identifiée par un entier unique (`AnimId`), référence un groupe de sprites (`SpriteGroup`), possède une vitesse d'animation (FPS) et un nombre de cycles (boucle infinie si -1).
Une image seule sans animation comme le corps du vaisseau du joueur est alors considérée comme une animation avec une seule frame.

Le système de rendu fonctionne selon une approche par **commandes différées** plutôt que par affichage immédiat.
Concrètement, lorsqu'un objet souhaite s'afficher à l'écran, il ne dessine pas directement : il enregistre une **commande de rendu** auprès du `GraphicsSystem` en spécifiant sa texture, sa position (appelée transform), ses dimensions...
Toutes ces commandes sont accumulées dans le `GraphicsSystem` durant la phase de mise à jour du jeu.
Ensuite, en une seule passe, le système trie l'ensemble des commandes par calque (pour gérer l'ordre d'affichage : arrière-plan, objets, particules, interface) puis les exécute séquentiellement pour produire le rendu final.
Cette architecture garantit un contrôle précis de l'ordre d'affichage et optimise les appels au moteur de rendu SDL3.

### Dossier `application` : le coeur du jeu Space Pixels

Le dossier `application/` exploite les briques élémentaires du dossier `engine/` pour implémenter la logique du jeu Space Pixels.
Au cœur de cette logique se trouve le concept de **GameObject**, une structure de base dont héritent tous les éléments (physiques) du jeu : joueur, ennemis, projectiles, objets bonus, arrière-plans, etc.

#### Le concept de GameObject et l'héritage en C

Dans les langages orientés objet, une **classe** est définie comme une structure contenant des **attributs** (variables) et des **méthodes** (pointeurs sur fonction).
L'application crée des **objets** lors de l'allocation mémoire de classes et modifie leurs membres via les méthodes de l'objet (ou d'autres objets).
Il est également possible de réaliser un **héritage** d'une classe à partir d'une autre.
Ainsi, une classe fille dérivée d'une classe mère contient les attributs et méthodes de la classe mère, et en ajoute d'autres.
L'objectif est de simplifier le développement de la nouvelle classe en réutilisant les méthodes de la classe mère.
Le langage C ne supportant pas nativement l'héritage de classes, on utilise une technique de **composition par structure** : chaque objet spécifique (comme `Player`, `Fighter`, `LinearBullet`) contient en premier membre une structure `GameObject` appelée `m_base`.
Cette technique permet de **caster** n'importe quel pointeur vers un objet spécifique en un pointeur vers `GameObject` et d'utiliser directement ses méthodes.

Tout élément du jeu est donc à la base un `GameObject` enrichi d'attributs et méthodes spécifiques à ce nouvel élément.
Par exemple, un `Player` est défini ainsi :
```c
typedef struct Player
{
    GameObject m_base;  // DOIT être le premier membre
    Vec2 m_direction;
    float m_speed;
    SpriteAnimState m_mainAnim;
    // ... autres attributs spécifiques au joueur
} Player;
```

Grâce à cette organisation, un `Player*` peut être casté en `GameObject*` et inversement, permettant ainsi un traitement uniforme de tous les objets du jeu.

> [!WARNING]
> **Attention à l'ordre des membres** : il est impératif que `GameObject m_base` soit **toujours le premier membre** de toute structure héritant de `GameObject`.
> Si vous ajoutez des attributs avant `m_base`, les casts échoueront et le programme plantera.

#### Les méthodes virtuelles du GameObject

La structure `GameObject` définit un ensemble de **pointeurs de fonctions** (l'équivalent en C des méthodes virtuelles) que chaque type d'objet peut surcharger pour définir son comportement propre.
Ces méthodes sont appelées automatiquement par le moteur de jeu à différents moments du cycle de vie d'un objet :

- **`onUpdate(void* selfPtr, float dt)`** : appelée à **chaque frame** pour mettre à jour la logique de l'objet (animations, états, déclenchement d'actions en fonction de l'animation...).
  Le paramètre `dt` représente le temps écoulé depuis la dernière frame en secondes.
  Cette fonction est idéale pour lancer ou arrêter des animations, gérer les changements d'état et des actions dépendant du temps réel (comme le tir du joueur).

- **`onFixedUpdate(void* selfPtr, float timeStep)`** : appelée à fréquence fixe de **100 fois par seconde** (soit toutes les 10 ms) pour gérer la physique de l'objet (déplacements, collisions).
  Le paramètre `timeStep` vaut toujours 0.01 seconde.
  Cette méthode garantit un comportement physique déterministe et stable, indépendant du framerate d'affichage.

- **`onRender(void* selfPtr)`** : appelée à chaque frame pour enregistrer les commandes de rendu de l'objet auprès du `GraphicsSystem`.
  L'objet ne dessine pas directement à l'écran : il enregistre des commandes qui seront exécutées ultérieurement lors du rendu final.

- **`onTakeDamage(void* selfPtr, float damage, int playerId)`** : appelée lorsque l'objet subit des dégâts, par exemple suite à une collision avec un projectile.
  Le paramètre `playerId` permet d'identifier quel joueur a infligé les dégâts (utile pour attribuer les points). Par convention, les projectiles ennemis ont un `playerId` égal à -1.

- **`onDrawGizmos(void* selfPtr)`** : appelée lors du mode débogage (touche F2) pour afficher des informations visuelles de débogage (boîtes de collision, trajectoires, etc.).

- **`onDestroy(void* selfPtr)`** : appelée juste avant la destruction de l'objet pour libérer les ressources éventuellement allouées et effectuer les actions de nettoyage nécessaires.

Lors de la création d'un nouvel objet héritant de `GameObject`, le comportement natif de certaines méthodes n'est pas totalement adapté.
Il est alors nécessaire d'écraser une partie de ces méthodes virtuelles.
Par exemple, un projectile simple écrasera principalement `onUpdate` (pour synchroniser sa position d'affichage avec celle de son corps physique), `onFixedUpdate` (pour vérifier les collisions), `onRender` (pour l'afficher) et `onDestroy` (pour libérer sa mémoire), mais n'implémentera probablement pas `onTakeDamage` car il ne subit pas de dégâts.

À l'inverse, un ennemi écrasera toutes les méthodes : `onUpdate` pour gérer ses animations et sa logique de tir, `onFixedUpdate` pour ses déplacements, `onRender` pour son affichage, `onTakeDamage` pour gérer ses points de vie et sa destruction lorsqu'il est touché, `onDrawGizmos` pour le débogage et `onDestroy` pour notifier la scène de sa mort (cette information est utilisée pour déclencher une nouvelle vague d'ennemis).
Le choix des méthodes à écraser dépend donc entièrement du rôle et du comportement que vous souhaitez donner à votre objet dans le jeu.

#### Le GameContext : point d'accès central

Chaque `GameObject` possède un pointeur vers un **`GameContext`**, une structure centrale donnant accès à tous les systèmes partagés du jeu :
- `AssetManager` : gestionnaire des assets (textures, sons, polices)
- `SpriteAnimManager` : gestionnaire des animations sprites
- `AudioSystem` : système audio
- `GraphicsSystem` : système de rendu
- `PhysicsEngine` : moteur physique 2D
- `Input` : gestionnaire des entrées clavier/manette
- `Camera` : caméra de la scène
- `ParticleSystem` : système de particules
- `GizmosSystem` : système de débogage visuel

Grâce au `GameContext`, n'importe quel objet peut facilement accéder aux systèmes du jeu sans avoir besoin de passer de multiples paramètres ou de maintenir des références globales.
Ainsi, il est régulièrement nécessaire de récupérer le `GameContext` via un appel à la méthode `GameObject_getContext()` sur l'objet en question au début de nombreuses méthodes virtuelles redéfinies.

#### Les scènes du jeu

Le jeu s'articule autour de deux **scènes** principales gérées par le `SceneManager` :
- **`TitleScene`** : la scène du menu principal, composée de plusieurs pages (page de titre, page de sélection de niveau, page d'options).
  Cette scène gère l'interface utilisateur et les transitions vers la scène de jeu.
- **`LevelScene`** : la scène de jeu proprement dite, où se déroulent les niveaux avec les joueurs, ennemis, projectiles et arrière-plans.
  Cette scène gère la boucle de jeu, les vagues d'ennemis, le HUD (affichage tête haute) et les transitions vers les pages de pause ou de fin de niveau.

Le **`SceneManager`** est responsable de la gestion du cycle de vie des scènes : il initialise la scène courante lors de son premier démarrage en appelant sa méthode `onStart()`, met à jour son état à chaque frame via `onUpdate()` et `onFixedUpdate()`, puis la détruit proprement lorsqu'un changement de scène est demandé en appelant `onQuit()`.
Lors de sa destruction, qui a lieu uniquement lorsque l'utilisateur quitte l'application, il appelle `onDestroy()`.

Les transitions entre scènes (par exemple, du menu principal vers le jeu ou du jeu vers l'écran de fin de niveau) sont gérées de manière différée : lorsqu'un objet appelle `SceneManager_changeScene()`, le changement n'est pas effectué immédiatement mais reporté à la fin de la frame courante, garantissant ainsi la cohérence de l'état du jeu pendant toute la durée de la mise à jour.
Cette architecture permet d'éviter les changements d'état en pleine exécution qui pourraient causer des erreurs ou des comportements imprévisibles.


## Étude du dossier src/

Les fichiers sources à modifier sont regroupés dans le dossier `application/src`.
On y retrouve alors deux dossiers et deux fichiers : `main.c` contenant le point d'entrée du programme avec la boucle d'interaction principale et `settings.h` listant les bibliothèques communes aux autres fichiers sources.

### Dossier `common/`

Le fichier `common.h` permet de définir un ensemble de constantes permettant d'identifier et manipuler les différents objets et animations du jeu.

#### La paire `assets.c/.h` : gestion centralisée des ressources

Comme indiqué précédemment, le système de gestion des ressources du jeu (sprites et animations, audio, polices d'écriture) repose sur une identification numérique de chaque ressource.
Le fichier `assets.h` définit quatre énumérations principales qui servent d'identifiants pour toutes les ressources du jeu : `GameSpriteId` pour les atlas de sprites, `GameFontId` pour les polices de caractères, `GameAudioId` pour les musiques de fond et effets sonores, `GameAnimId` pour les animations du jeu.

Le fichier `assets.c` implémente la fonction `Game_addAssets()` qui est appelée une seule fois au démarrage du jeu pour enregistrer toutes les ressources auprès de l'`AssetManager` et du `SpriteAnimManager`.
Cette fonction utilise quatre tableaux statiques de structures décrivant les ressources à charger :
- **`sheetsToLoad[]`** : associe chaque `GameSpriteId` à un chemin vers un fichier PNG (atlas de textures) et un fichier JSON (description du découpage en sprites). Par exemple : `{ SPRITE_PLAYER, "atlas/player", "atlas/player_desc" }`.
- **`audiosToLoad[]`** : associe chaque `GameAudioId` à un chemin de fichier audio, un gain (volume), et un indicateur de pré-décodage pour optimiser les performances.
- **`fontsToLoad[]`** : associe chaque `GameFontId` à un chemin de fichier de police et une taille en pixels.
- **`animsToLoad[]`** : associe chaque `GameAnimId` à un `sheetId` (l'atlas source), un `groupName` (nom du groupe de sprites dans le JSON), une vitesse d'animation (`fps`), et un nombre de cycles (`loopCount`, -1 pour une boucle infinie).

#### La paire `ui_style.c/.h` : personnalisation de l'interface utilisateur

La paire `ui_style.c/.h` permet de donner un style commun aux différents éléments de l'interface utilisateur. Elle sera détaillée dans un autre document.

### Dossier game/
Le dossier `game/` contient quant à lui tous les fichiers spécifiques au jeu.
La paire `game_config.c/.h` permet de définir l'unique variable globale du jeu : `g_gameConfig`.
Sa définition dans le `.h` nous informe qu'elle contient la configuration globale du jeu avec le niveau en cours, le score des joueurs...
La paire `input.c/.h` permet d'écouter les actions utilisateurs (clavier, souris) et de les enregistrer dans la structure `PlayerInput`.
Cette dernière sera utilisée par le fichier implémentant le Player pour savoir quelles actions réaliser dans le jeu.


#### Dossier core/

Le dossier `core/` contient les éléments fondamentaux du jeu qui font le pont entre le moteur générique dans le dossier `engine/` et la logique applicative.
On y trouve la définition du `GameObject` (étudié précédemment), le `GameContext` (point d'accès central à tous les systèmes), l'`ObjectManager` (permettant l'ajout et la suppression mémoire des objets), le `PhysicsEngine` (moteur physique 2D avec collisions) et le `SceneManager` (gestion des scènes et transitions).
Ces fichiers forment l'infrastructure centrale du jeu et ne doivent généralement pas être modifiés.
En revanche, vous avez tout intérêt à étudier le contenu de ces fichiers pour vous familiariser avec le basecode.


#### Dossier level/

Le dossier `level/` contient tous les objets du jeu qui apparaissent durant une partie.
On y trouve d'abord le joueur (`player.c/.h`) qui gère les déplacements, animations et tirs.
Il n'y a pour l'instant qu'un seul type d'ennemi, le `fighter.c/.h`, mais il est prévu d'en ajouter d'autres (frigate, scout, battlecruiser...).
Tous les ennemis possèdent un composant `EnemyData` défini dans `enemy_data.c/.h` qui contient les propriétés communes à tous les ennemis (points de vie, animations de base, dégâts subis, etc.). Il s'agit ici d'une simple composition et non d'un héritage.
Les arrière-plans défilants sont gérés par `background_layer.c/.h`.
Tous ces objets héritent de `GameObject` et redéfinissent les méthodes virtuelles selon leurs besoins.

Les projectiles sont représentés par `linear_bullet.c/.h` (tirs en ligne droite utilisés par le joueur et les ennemis) et `homing_missile.c/.h` (missiles autoguidés vers une cible).
Le fichier `linear_bullet.c` contient un gros bloc `switch/case` dans sa fonction de création qui définit les masques de collision selon le type de projectile (joueur ou ennemi), déterminant ainsi avec quelles entités il peut interagir.
Les `item.c/.h` représentent les bonus récupérables (points de vie, armes améliorées...).

Les fichiers `template_enemy.c/.h` et `template_object.c/.h` sont des modèles à dupliquer pour créer rapidement de nouveaux types d'ennemis ou d'objets sans partir de zéro.
Ils contiennent la structure de base avec tous les pointeurs de fonctions virtuelles correctement initialisés et des indications sur les propriétés physiques à définir (catégorie de collision, masques d'interaction, position, orientation).
Il est recommandé de garder les fonctions `static` et d'utiliser `Ctrl+H` pour renommer proprement toutes les occurrences lors de la création d'un nouvel objet.


#### Dossier scene/

Le dossier `scene/` implémente les deux scènes principales du jeu évoquées précédemment : la `TitleScene` (menu principal avec pages de titre, sélection de niveau et options) et la `LevelScene` (scène de jeu avec joueurs, ennemis, projectiles et HUD).
Chaque scène hérite de la structure `Scene` présente dans le dossier `core/` et possède un gestionnaire d'interface utilisateur dédié (`TitleUIManager` et `LevelUIManager`).
La `LevelScene` expose des fonctions pour gérer les événements de gameplay (niveau terminé, échec, pause/reprise) et contient les définitions des niveaux scriptés (dans le sous-dossier `level/`) qui définissent les vagues d'ennemis.


#### Dossier ui/

Le contenu de ce dossier restera un mystère pour le moment !
Développez tout d'abord le contenu du jeu, l'interface utilisateur viendra plus tard !


## Compilation

Un code de départ vous est fourni pour la réalisation du TP afin que vous n'ayez pas à vous préoccuper des aspects liés à la création d'une fenêtre et à l'affichage dans cette dernière.
La création du projet de développement est réalisée par CMake.
Il est donc nécessaire d'installer ce logiciel dans un premier temps si ce n'est pas encore fait.
La création du projet est automatisée grâce à une série de lignes de commandes qui permettront de détecter le compilateur et l'IDE présents par défaut sur votre machine.
Sous Windows, il vous suffit de lancer Visual Studio depuis la racine du projet en faisant un clic droit dans la fenêtre de l'explorateur de documents.

Le lancement de l'exécutable produit provoquera l'ouverture d'une nouvelle fenêtre affichant le menu principal du jeu. Celui-ci est entièrement fonctionnel à l'exception du lancement du second niveau qui n'est pas encore implémenté.

> [!NOTE]
> Récupérez le basecode du projet, lancez CMake, compilez le projet et vérifiez que le programme se lance correctement.
> Si ce n'est pas le cas, demandez de l'aide rapidement à votre enseignant.


La fenêtre que vous avez vue apparaître est au format 16/9 et représente un monde ayant pour dimensions 16 × 9 unités.
Le point (0,0) est situé au centre de la fenêtre, le x étant positif vers la droite et le y positif vers le haut.

-------------------------------------------------------------------------
# Tutoriel de prise en main du basecode

Il est temps de plonger les mains dans le code !