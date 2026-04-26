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
La partie précédente a permis de donner un contexte global au jeu en détaillant son architecture et le fonctionnement général des primitives.
Le but de cette section est de détailler les grandes étapes de la construction du joueur et des ennemis.
Après ce tutoriel, vous en aurez assez vu pour compléter le jeu selon vos désirs.

Pour chaque partie, une ligne de commentaire du type `// [TODO XXX]` est ajoutée dans le code et correspond à des lignes à décommenter et/ou modifier.
Identifez ces zones à l'aide d'un Ctrl+F (choisissez de rechercher dans la solution complète) et adapatez-les pour réaliser l'opration demandée.


## Développement partie joueur

Le joueur est défini au travers de la structure `Player` présente dans le fichier `player.h`.
Comme tous les autres éléments du jeu, son premier membre est un `GameObject` auquel on ajoute des informations sur les différentes animations le composant, des états...
Son développement passe par la création de l'objet et son affichage, son déplacement et la gestion de ses tirs.

### [TODO Player render] - Affichage du joueur

> [!NOTE]
> Fichier à modifier : game/level/player.c

Le player est formé de deux animations permanentes (le corps du vaisseau et les moteurs) et d'un bouclier qui s'affiche uniquement lorsque ce dernier est activé.
Le chargement des animations a déjà été réalisé grâce aux fichiers `common/assets.c/.h`.
Il ne reste donc plus qu'à récupérer les identifiants de chacune des animations, les mettre à jour et les afficher.
Commençons par l'affichage.

#### Fonction onRender() : envoi de la commande d'affichage

L'affichage à l'écran est réalisé dans la fonction `PlayerVM_onRender()` qui enregistre les commandes de rendu auprès du `GraphicsSystem`.
La commande désigne la texture à afficher ainsi que des informations de position, dimensions, ancrage, etc.
Observez la création de la commande par la déclaration de la variable `GraphicsCmd cmd = { 0 };` et son initialisation.
Le champ `transform` désigne la position de l'objet qui peut être récupérée avec `GameObject_getTransform()`. Cette position est actuellement dans le référentiel **monde**, mais elle sera automatiquement convertie en position en pixels lors du rendu avec le `GraphicsSystem`.
N'hésitez pas à modifier plus haut la valeur de l'ancrage (variable `anchor`) pour définir la position du sprite par rapport à sa position réelle (au-dessus du corps physique, au centre...).

> player.c -- onRender() : décommentez les lignes présentes pour récupérer la bonne animation et envoyer l'animation au `GraphicsSystem`.
> Observez que l'on a besoin d'envoyer une commande par animation, mais que nous avons seulement utilisé la variable locale `cmd`.
> Le pointeur vers la texture à afficher a été modifié à chaque fois pour envoyer correctement les trois animations composant le player.

#### Fonction onUpdate() : mise à jour des animations

Pour le moment, le sprite est bien trop gros et on ne voit aucune animation.
Corrigeons déjà le fait que les animations soient fixes.
Elles ont visiblement été correctement chargées en mémoire, mais une mise à jour est nécessaire à chaque frame pour indiquer quelle frame afficher au bon moment.
C'est le `SpriteAnimManager` qui se charge de calculer tout ça pour nous !
Il suffit de lui dire de se mettre à jour.

> player.c -- onUpdate() : décommenter les lignes de la fonction afin de mettre à jour les trois animations.

#### Fonction onCreate() : identifiant des animations

Il y a du mouvement, mais la dimension est toujours incohérente et ce n'est pas le bon sprite affiché (le bouclier est affiché à la place du corps du vaisseau) !
Ces éléments sont définis dans la fonction `create()`.
Elle permet d'attribuer le bon identifiant pour chaque animation du vaisseau au travers des membres de type `SpriteAnimState` de la structure `Player`.
Chacun d'entre eux contient, en plus de l'identifiant d'animation chargé dans les fichiers `common/assets.c/.h`, des informations qui permettent au `SpriteAnimManager` de savoir quelle frame afficher à l'instant t.

> player.c -- create() : modifier l'identifiant de l'animation en corrigeant le dernier paramètre de l'appel à la fonction `SpriteAnimState_init()`.
> N'hésitez pas à jeter un œil dans le fichier `common/assets.h` pour voir quels identifiants sont disponibles.
> Nous verrons plus tard comment ils sont reliés à des sprites.

#### Fonction onCreate() : dimensions

Enfin, il ne reste plus que les dimensions du sprite à corriger, toujours dans la fonction `create()`.
Le système de rendu nécessite de connaître la taille du sprite dans le monde du jeu.
Actuellement, on constate que le calcul de la dimension est réalisé avec un appel à la fonction `RenderDim_setWorldDimensions()`, permettant d'attribuer des largeur et hauteur de 5 × 5 unités (sur un jeu de dimensions 16 × 9 unités), ce qui ne correspond pas à la taille réelle du sprite.
Il faut utiliser `RenderDim_setPixelsPerUnit()` avec la constante `PIX_PER_UNIT` qui permet de convertir automatiquement les dimensions en pixels du sprite en unités du monde, garantissant ainsi un affichage proportionnel et cohérent. C'est particulièrement important pour les jeux en 2D de style Pixel Art.

> player.c -- create() : remplacer le calcul des dimensions en utilisant la fonction commentée à la place de `RenderDim_setWorldDimensions()`.

### [TODO Player movement] - Déplacement du joueur

> [!NOTE]
> Fichier à modifier : game/level/player.c

Maintenant que le vaisseau s'affiche correctement, il est temps de le faire bouger en fonction des entrées du joueur !
Le déplacement d'un objet dans le jeu se fait en deux temps : la mise à jour de la logique (position du player dans le monde) dans `onUpdate()` (appelée à chaque nouvelle frame) et l'application physique dans `onFixedUpdate()` (appelée exactement 100 fois par seconde).

#### Fonction onUpdate() : lecture des entrées et mise à jour de la direction

Dans la fonction `PlayerVM_onUpdate()`, nous allons lire les entrées du joueur pour mettre à jour les attributs `m_direction` et `m_speed` du `Player`.
Pour accéder aux entrées utilisateur, nous devons d'abord récupérer le `GameContext`, le point d'accès central à tous les systèmes du jeu, grâce à la méthode `GameObject_getContext()`.
Ce `GameContext` nous permet ensuite d'accéder aux entrées utilisateur via le pointeur `PlayerInput`.

La structure `PlayerInput` (définie dans `game/input.h`) contient toutes les entrées d'un joueur :
- `axis` (`Vec2`) : direction du joystick ou des touches directionnelles, normalisée entre -1 et 1 sur chaque axe. Un système de "deadzone" est appliqué pour éviter les petites valeurs parasites lorsque le joystick est au repos. La longueur du vecteur `axis` est limitée à 1 pour garantir une direction normalisée (même pour un déplacement diagonal).
- `triggerL` (float) : valeur de la gâchette gauche entre 0 et 1, utilisée ici pour moduler la vitesse.
- `shootDown`, `shootPressed`, etc. : booléens pour les différentes actions (tir, boost, pause...).

> player.c -- onUpdate() : décommentez les deux lignes permettant de récupérer la direction (`playerInput->axis`) et de calculer la vitesse que l'on veut donner au joueur.

Notez que la ligne `GameObject_updateTransform(self);` permet de synchroniser la position du joueur dans le monde à partir de la position du corps physique.
Cette dernière doit encore être mise à jour dans la fonction `onFixedUpdate()`.

#### Fonction onFixedUpdate() : application de la vélocité au corps physique

Une fois la direction et la vitesse calculées dans `onUpdate()`, il faut les appliquer au corps physique (`Body`) du joueur dans `onFixedUpdate()`.
Chaque `GameObject` possède un `Body` qui gère sa position, sa vélocité et ses collisions dans le moteur physique.
La vélocité (vitesse vectorielle) du `Body` détermine son déplacement à chaque pas de temps physique.

> player.c -- onFixedUpdate() : décommentez la ligne qui applique la vélocité au `body`.
> Observez que l'on calcule la vélocité en multipliant (`Vec2_scale`) le vecteur direction normalisé par la vitesse scalaire.

Compilez et testez : le vaisseau devrait maintenant répondre aux touches directionnelles et à la manette !

### [TODO Player shoot] - Tirs du joueur

> [!NOTE]
> Fichiers à modifier : game/level/player.h et game/level/player.c

Maintenant que le vaisseau se déplace, donnons-lui la capacité de tirer des projectiles.
Nous allons implémenter deux versions du système de tir : une version simple (V1) avec tir à chaque action du joueur, puis une version avancée (V2) avec cadence de tir automatique et limitée pour un meilleur équilibre du gameplay.

Le jeu utilise la structure `LinearBullet` pour représenter les projectiles qui se déplacent en ligne droite.
Lors de la création d'un projectile avec `LinearBullet_create()`, on spécifie entre autre le type de projectile (ici `LINEAR_BULLET_TYPE_PLAYER_DEFAULT`), sa position, vitesse et orientation initiale ainsi que l' identifiant du joueur qui tire (pour attribuer les points lors d'une destruction d'ennemi).

La fonction `Player_updateBullet()` est appelée à chaque frame depuis `PlayerVM_onUpdate()` pour vérifier si le joueur souhaite tirer et créer les projectiles en conséquence.
C'est donc dans cette fonction que nous ferons les modifications. Nous vous conseillons d'implémenter une sous-fonction `Player_updateXXX()` pour chaque type de bonus que vous souhaitez implémenter (space gun, auto-canons, rockets...) afin de garder une bonne organisation du code.
Cela permettra également de ne pas surcharger la fonction `Player_update()` qui deviendra rapidement illisible si elle gère tous les types de tir.

#### Version 1 : Tir simple

La première version implémente un système de tir basique : dès que le joueur appuie sur le bouton de tir, un projectile est créé.
Cela produit un tir unique et ne limite pas la cadence de tir.

> player.c -- Player_updateBullet()
>
> Décommentez le bloc de code V1 pour implémenter le tir simple.
> Observez que le test `if (playerInput->shootPressed)` vérifie si le joueur vient d'appuyer sur le bouton de tir lors de cette frame.
> On emet donc un tir à chaque fois que la touche de tir passe de l'état haut à bas.
> Comparez le résultat avec l'usage de shootDown.

#### Version 2  : Cadence de tir limitée avec accumulateur

La version plus adaptée introduit un système de cadence de tir qui limite le nombre de projectiles à 5 tirs par seconde maximum.
Pour cela, nous allons utiliser un accumulateur temporel (`m_accuBullet`) qui se décharge au fil du temps.

> player.h -- Structure Player
>
> Décommentez la ligne `float m_accuBullet;` pour ajouter l'attribut au joueur.

Cet accumulateur représente le temps restant avant de pouvoir tirer à nouveau.
Il est décrémenté dans la fonction `Player_updateBullet()` du temps qui s'est écoulé depuis la dernière frame (`dt`).
Lorsqu'il est négatif ou nul et que la touche de tir est enfoncée, le joueur peut tirer.
Après chaque tir, il est réinitialisé à `PLAYER_BULLET_TIME` (définition dans `common/common.h`).

> player.c -- Player_updateBullet()
>
> Commentez le bloc V1, puis décommentez le bloc V2 pour implémenter la cadence de tir.

Compilez et testez : le joueur tire désormais à une cadence contrôlée, même si le bouton reste enfoncé.
Un effet sonore (`AUDIO_SFX_PLAYER_BASIC`) accompagne chaque tir pour un meilleur retour audio.

## Développement des ennemis

Tous les ennemis du jeu partagent une structure de données commune appelée **`EnemyData`** (définie dans `enemy_data.h`), qui centralise les attributs et comportements de base communs à tous les types d'ennemis.
Cette structure contient l'état de l'ennemi, ses points de vie, les points attribués au joueur lors de sa destruction, un indicateur de bouclier, ainsi que les identifiants de ses animations.
Elle gère également le type d'objet bonus que l'ennemi peut faire apparaître à sa mort.

Le fichier `enemy_data.c` fournit un ensemble de fonctions utiles pour tous les ennemis :
* `Enemy_init()` pour l'initialisation (enregistrement dans le niveau, configuration du corps physique avec les masques de collision), 
* `Enemy_onUpdate()` pour la gestion des animations et des changements d'état, 
* `Enemy_onFixedUpdate()` pour la mise à jour de la position physique, 
* `Enemy_onTakeDamage()` pour gérer les dégâts subis et la mort, 
* `Enemy_onRender()` pour l'affichage, 
* `Enemy_onDrawGizmos()` pour le débogage visuel,
* `Enemy_onDestroy()` pour les libérations de mémoire.

Chaque type d'ennemi concret (comme `Fighter` ) hérite ainsi de `GameObject` et de ses méthodes virtuelles et contient une structure `EnemyData` pour bénéficier de ces fonctionnalités partagées.
Les méthodes virtuelles de l'ennemi appellent ensuite les fonctions `Enemy_*()` correspondantes en leur passant leur structure `EnemyData`.

### [TODO enemy] - Base d'un ennemi

> [!NOTE]
> Fichier à modifier : game/level/enemy_data.c

Avant de créer un ennemi concret comme le `Fighter`, nous devons d'abord implémenter les fonctionnalités de base partagées par tous les ennemis.
La fonction `Enemy_onUpdate()` gère la logique d'animation de tous les ennemis.
Elle contient déjà un système de changement d'animation basé sur l'état de l'ennemi (`ENEMY_STATE_FIRING` ou `ENEMY_STATE_DYING`), mais l'animation principale (`data->m_mainAnim`) n'est pas encore mise à jour à chaque frame.
Sans cette mise à jour, l'animation restera figée sur sa première image.

> enemy_data.c -- Enemy_onUpdate()
>
> Décommentez la ligne qui met à jour l'animation principale.

La fonction `Enemy_onRender()` enregistre les commandes de rendu pour afficher l'ennemi à l'écran, exactement comme nous l'avons fait pour le joueur.
Le principe est identique : on crée une commande `GraphicsCmd`, on la configure avec la position, l'ancrage, la texture et les dimensions, puis on l'envoie au `GraphicsSystem`.

> enemy_data.c -- Enemy_onRender()
>
> Décommentez le bloc de code complet pour envoyer la commande de rendu.

Observez que la commande utilise `GAME_LAYER_ENEMY` comme calque de tri pour garantir que les ennemis s'affichent au bon niveau par rapport aux autres éléments du jeu.
Ces deux modifications suffisent à rendre fonctionnel le système d'affichage et d'animation de base pour tous les ennemis.
Cependant, aucun ennemi ne s'affiche pour le moment. En effet, un ennemi est un simple `GameObject` avec un composant `EnemyData`. Pour obtenir les fonctionnalités développées dans `enemy_data.c`, il faut appeler les fonctions explicitement dans les méthodes de chaque ennemi. Il s'agit ici d'une composition et non d'un héritage.
Nous verrons dans la section suivante comment créer un ennemi concret (le Fighter) qui utilisera ces fonctionnalités.

### [TODO fighter] - Création d'un ennemi type fighter

> [!NOTE]
> Fichiers à modifier : common/assets.h, common/assets.c, game/level/fighter.c

Maintenant que les fonctionnalités de base des ennemis sont codées (`Enemy_onUpdate()` et `Enemy_onRender()`), nous allons créer notre premier ennemi concret : le `Fighter`.
Le Fighter est un petit vaisseau ennemi qui tire des projectiles horizontaux en direction du joueur.
Sa création nécessite trois étapes : le chargement des ressources graphiques (sprites et animations), la configuration de ses données (animations, points de vie, points), et l'activation de son affichage.

#### Étape 1 : Chargement des ressources du Fighter

Avant de pouvoir afficher le `Fighter`, il faut d'abord charger ses sprites et définir ses animations.
Le système de gestion des assets nécessite d'abord de déclarer des identifiants dans des énumérations, puis de les enregistrer auprès de l'`AssetManager` et du `SpriteAnimManager`.

Le fichier `assets.h` contient deux énumérations importantes : `GameSpriteId` (identifiants des atlas de sprites) et `GameAnimId` (identifiants des animations).
Les entrées correspondant au `Fighter` sont déjà présentes mais commentées.

> assets.h -- enum GameSpriteId
>
> Décommentez la ligne `SPRITE_FIGHTER`.

> assets.h -- enum GameAnimId
>
> Décommentez les quatre lignes d'animations du Fighter.

Le fichier `assets.c` contient la fonction `Game_addAssets()` qui enregistre toutes les ressources au démarrage du jeu.
Elle utilise le tableau `sheetsToLoad[]` pour associer chaque `GameSpriteId` à un atlas de textures et une description JSON, et le tableau `animsToLoad[]` pour associer chaque `GameAnimId` à ses paramètres d'animation.

> assets.c -- sheetsToLoad[]
>
> Décommentez la ligne du `Fighter` afin de charger le fichier `atlas/fighter.png` (texture) et `atlas/fighter_desc.json` (description du découpage en sprites) et de les associer à l'identifiant `SPRITE_FIGHTER`.

> assets.c -- animsToLoad[]
>
> Décommentez les quatre lignes des animations du Fighter afin d'associer leur identifiant à leurs paramètres.
> Notez que l'animation de mort (`ANIM_FIGHTER_DYING`) a un `loopCount` de `1` au lieu de `-1`, car elle ne doit se jouer qu'une seule fois avant la destruction de l'ennemi.

#### Étape 2 : Configuration des animations dans Fighter_create()

La fonction `Fighter_create()` initialise un nouvel ennemi Fighter en configurant sa structure `EnemyData`.
Comme pour le player, elle permet entre autres d'attribuer le bon identifiant d'animation à chaque membre de type `AnimId`.

> fighter.c -- Fighter_create()
>
> Remplacez les quatre identifiants d'animations qui utilisent actuellement ceux du player.

Observez également que les attributs `hp` (points de vie) et `points` (points attribués au joueur lors de la destruction) sont déjà configurés respectivement à `10.f` et `3.f`.
Ces valeurs définissent la résistance de l'ennemi et la récompense pour le joueur.

#### Étape 3 : Activation de l'affichage dans FighterVM_onRender()

La fonction `FighterVM_onRender()` est la méthode virtuelle appelée à chaque frame pour afficher le `Fighter`.
Elle doit déléguer le rendu à la fonction `Enemy_onRender()` que nous avons implémentée dans la section précédente.
Cette fonction générique gère l'affichage de tous les ennemis en utilisant les données de leur structure `EnemyData`.

> fighter.c -- FighterVM_onRender()
>
> Décommentez la ligne `Enemy_onRender(self, &self->m_enemyData);`.


#### Test et observation

Compilez et lancez le jeu.
Vous devriez maintenant voir apparaître des ennemis `Fighter` dans le niveau, avec leurs animations qui se jouent correctement.
Pour le moment, ils ne tirent pas encore et ne réagissent pas aux collisions, mais leur affichage et leurs animations fonctionnent.

### [TODO enemy gizmos] - Visualisation du collider de l'ennemi

> [!NOTE]
> Fichiers à modifier : game/level/enemy_data.c, game/level/fighter.c

Les **gizmos** sont des outils de visualisation utilisés en mode debug pour afficher des informations visuelles qui ne sont pas présentes dans le jeu final.
Ils permettent notamment de visualiser les **colliders** (zones de collision) des objets pour vérifier qu'ils correspondent bien aux sprites affichés.

Le système de gizmos pour les objets du jeu s'active avec la touche **F2**.
Lorsqu'un mode gizmo est actif, chaque objet possédant une méthode `onDrawGizmos()` peut enregistrer des commandes de dessin auprès du `GizmosSystem`.
Ces commandes permettent de dessiner des formes géométriques simples (cercles ou rectangles) par-dessus le rendu normal du jeu.

Puisque les ennemis partagent un composant `EnemyData`, c'est dans cette dernière que l'on retrouve la commande au moteur de rendu des gizmos.

> enemy_data.c -- Enemy_onDrawGizmos()
>
> Décommentez le bloc complet de création et d'envoi de la commande gizmos.

Cette fonction ne sera exécutée que lorsque le mode gizmos est actif (touche F2) et que le fighter aura appelé cette méthode.

> fighter.c -- FighterVM_onDrawGizmos()
>
> Décommentez l'appel à la fonction générique de dessin du gizmo.

Compilez et lancez le jeu, puis appuyez sur **F2** pour activer les gizmos des objets du jeu.
Vous devriez voir apparaître un **grand** cercle rose autour du `Fighter` désignant la zone de collision.
Le rayon est actuellement fixé à `2.0f` unités dans la fonction `Fighter_create()`, ce qui correspond à un diamètre de 4 unités (presque la moitié de la hauteur de l'écran !).
Il est essentiel que ce rayon soit ajusté **au plus près du sprite** pour obtenir des collisions précises et satisfaisantes pour le joueur.

> fighter.c -- Fighter_create()
>
> Modifiez le rayon du collider pour l'ajuster au mieux au sprite du fighter.

### [TODO fighter shoot] - Tir de l'ennemi

> [!NOTE]
> Fichier à modifier : game/level/fighter.c

Maintenant que le `Fighter` s'affiche correctement et que son collider est bien ajusté, il est temps de le rendre dangereux en lui donnant la capacité de tirer !
Contrairement au joueur qui tire en fonction d'une cadence temporelle (accumulateur `m_accuBullet`), les ennemis utilisent un système de tir basé sur les événements d'animation.
Cette approche permet de synchroniser parfaitement les tirs avec l'animation visuelle du vaisseau : les projectiles sont lancés précisément lorsque l'animation montre les canons en action.
C'est aussi la démarche qu'il faudra utiliser pour les canons automatiques du joueur (item auto-canon) afin de faire partir les projectiles au bon moment.

#### Le système d'événements d'animation

Le moteur d'animation du jeu génère automatiquement des **événements** (`SpriteAnimEvent`) à chaque changement de frame d'une animation.
Ces événements sont stockés dans la structure `SpriteAnimState` et peuvent être consultés à chaque frame dans la méthode `onUpdate()`.
Chaque événement contient un type (`ANIM_EVENT_FRAME_CHANGED` pour les changements de frame) et un index (le numéro de la frame qui vient d'être affichée).
Pour le Fighter, nous allons détecter les frames 1 et 3 de l'animation de tir (`ANIM_FIGHTER_FIRING`) pour créer des projectiles aux positions de ses deux canons.
Consulter le fichier `atlas/fighter_desc.json` vous permettra de voir la disposition des frames d'animation et de repérer les frames 1 et 3 qui montrent les canons en action.

La fonction `FighterVM_onUpdate()` contient déjà la structure de base pour écouter les événements d'animation.
Après avoir appelé `Enemy_onUpdate()` qui met à jour l'animation principale, la fonction vérifie que l'ennemi est bien en état de tir (`ENEMY_STATE_FIRING`), puis boucle sur tous les événements générés par l'animation courante.

Ce pattern de boucle filtre les événements pour ne traiter que les changements de frame (`ANIM_EVENT_FRAME_CHANGED`), puis utilise `event->index` pour identifier la frame concernée.

> fighter.c -- FighterVM_onUpdate()
>
> Décommentez le bloc complet du tir à la frame 1.
> Le vaisseau ennemi tire maintenant un projectile de son canon droit lorsque l'animation passe sur la frame 1.

Notons que le calcul de la position initiale du projectile est déterminé à partir de la position du fighter (`transform->position`).
On y ajoute un décalage en pixels converti en unités du monde avec `PIX_TO_WORLD`.
Le décalage `(-6.f, +6.f)` place le projectile légèrement à gauche et en haut par rapport au centre du vaisseau, correspondant à la position visuelle du canon gauche sur le sprite.

Le Fighter possède deux canons et l'animation de tir montre les deux en action, le premier sur la frame 1, le second sur la frame 3.
Pour un comportement cohérent, nous devons faire tirer le second canon à la frame 3 de l'animation.
De plus, il sera nécessaire de faire partir le projectile de la bonne position.
Attention, le sprite du fighter est pivoté de 90 degrés dans le sens trigonométrique par rapport à sa disposition originale dans l'atlas.
Prenez cette rotation en compte pour décaler votre position initiale de second projectile.

> fighter.c -- FighterVM_onUpdate()
>
> Dupliquez le bloc de code du tir à la frame 1 et modifiez-le pour tirer à la frame 3, en adaptant sa position de départ.

Utilisez la touche **F2** pendant le jeu pour activer les gizmos et observer la position exacte de création des projectiles.
Vous pouvez également appuyer sur la touche **F1** pour activer le mode pas à pas et **Tab** pour avancer d'un pas.

Pour le moment, ces projectiles ne causent pas encore de dégâts au joueur (la détection de collision sera implémentée dans la section suivante), mais le comportement visuel et audio du tir ennemi est fonctionnel.

### [TODO collisions] - Collisions et système de dégâts

> [!NOTE]
> Fichiers à modifier : game/level/linear_bullet.c, game/level/enemy_data.c

Maintenant que les projectiles sont créés et se déplacent correctement, il est temps de les rendre fonctionnels en activant le système de collision et de dégâts.
Pour le moment, les projectiles traversent les joueurs et les ennemis sans effet.

Le système de collision du moteur physique repose sur un système de **catégories** et de **masques** de bits :
- **`categoryBits`** : définit à quelle(s) catégorie(s) appartient un objet (ex : `GAME_CATEGORY_ENEMY`)
- **`maskBits`** : définit avec quelle(s) catégorie(s) l'objet peut entrer en collision (ex : `GAME_CATEGORY_PLAYER_BULLET`)

Deux objets entrent en collision si et seulement si :
- les `categoryBits` du premier objet intersectent les `maskBits` du second
- **ET** les `categoryBits` du second objet intersectent les `maskBits` du premier

Puisque les projectiles du joueur et des ennemis sont pour le moment tous des `LinearBullet`, c'est dans cette classe que sont initialisés les masques et catégories par défaut pour chaque type de projectile.
Une fois les masques et catégories des projectiles définis, il faudra récupérer les informations de collision lorsque ces dernières seront détectées.
Finalement viendra l'attribution de dégâts à l'objet touché et l'attribution de points au player dans le cas d'un ennemi détruit.

#### Étape 1 : Activation de la détection de collision dans LinearBulletVM_onFixedUpdate()

Une fois les masques correctement configurés, le moteur physique détecte automatiquement les collisions et les stocke dans le tableau `body->collisions`.
Il ne reste plus qu'à lire ce tableau et à appliquer les dégâts aux objets touchés.
Lorsqu'un corps `bodyA` entre en collision avec un corps `bodyB`, une entrée est ajoutée dans le tableau de collisions de `bodyA` ainsi que dans celui de `bodyB`.
Soyez donc attentif à ne pas appliquer les dégâts deux fois en traitant à la fois les collisions de `bodyA` et de `bodyB` pour un même contact. Le choix a été fait de ne traiter que les collisions uniquement dans la classe `LinearBullet` pour éviter ce problème. Cela évite également de devoir implémenter la logique de dégâts dans les classes `Player` et `Enemy` qui n'ont pas besoin de connaître les spécificités des projectiles.

> linear_bullet.c -- LinearBulletVM_onFixedUpdate()
>
> Décommentez le bloc complet de vérification des collisions (boucle `for` sur `body->collisionCount`).

Ce code parcourt toutes les collisions détectées lors du dernier pas de physique.
Pour chaque collision, il :
1. Récupère le `Body` de l'objet touché via `PhysicsEngine_getBody()`
2. Récupère le `GameObject` associé via `body->userData`
3. Applique les dégâts à l'objet via `GameObject_takeDamage()` en passant les dégâts (`self->m_damage`) et l'identifiant du joueur qui a tiré (`self->m_playerId`)
4. Détruit le projectile avec `GameObject_setToDestroy()`
5. Émet des particules visuelles avec `LinearBullet_emitParticles()`

La méthode `GameObject_setToDestroy()` marque l'objet pour destruction, qui sera effective à la fin de la frame en cours. Il n'est donc pas problématique de l'appeler au sein de la boucle de collision, même si le projectile entre en collision avec plusieurs objets en même temps.

Compilez et lancez le jeu.
Les projectiles du joueur devraient maintenant toucher les ennemis avec des particules visibles, mais sans effet sur leurs points de vie.
En revanche, les projectiles du fighter ne touchent pas encore le joueur.

#### Étape 2 : Configuration des masques pour les projectiles ennemis

Actuellement, les projectiles de type `LINEAR_BULLET_TYPE_FIGHTER` ont leurs masques désactivés dans la méthode `LinearBullet_create()`, ce qui les empêche de détecter toute collision.
Nous devons leur attribuer les bonnes valeurs pour qu'ils puissent toucher le joueur.

> linear_bullet.c -- LinearBullet_create()
>
> Dans le `switch` case `LINEAR_BULLET_TYPE_FIGHTER`, remplacez les lignes d'initialisation du masque et de catégorie par les valeurs appropriées.
> Inspirez-vous des lignes juste au dessus définissant ceux des projectiles du player.
> Les constantes de catégorie sont définies dans `common/common.h`.

Suivant les valeurs que vous avez attribuées aux masques et catégories des projectiles du fighter, il est possible que le fighter se touche lui-même avec ses propres tirs et ne semble donc pas tirer du tout.
N'hésitez pas à consulter les masques et catégories de tous les objets du jeu (player, ennemis, projectiles) pour vérifier que les collisions sont configurées correctement.

#### Étape 3 : Gestion des dégâts dans onTakeDamage()

Maintenant que les collisions sont correctement détectées, il faut retirer des points de vie à l'objet touché et le changer d'état lorsque les points de vie sont à zéro.
Implémentons cette logique de dégâts dans la fonction `Enemy_onTakeDamage()`.
Cette fonction est appelée automatiquement lorsqu'un projectile touche un ennemi (via `GameObject_takeDamage()`).

> enemy_data.c -- Enemy_onTakeDamage()
>
> Décommentez la ligne `data->hp -= damage;` pour diminuer les points de vie de l'ennemi.

Lorsque les points de vie atteignent zéro, l'ennemi doit passer en état `ENEMY_STATE_DYING` pour déclencher son animation de mort.
Il faut finalement attribuer des points au joueur qui vient de tuer l'ennemi.

> enemy_data.c -- Enemy_onTakeDamage()
>
> Dans le bloc `if (data->hp <= 0.f)`, décommentez la ligne `data->state = ENEMY_STATE_DYING;`.
> Décommentez également la ligne qui augmente le score du joueur.


#### Étape 4 : Destruction à la fin de l'animation et attribution des points

Après plusieurs tirs reçus, l'ennemi émet un item et arrête de tirer.
Cela correspond à la condition implémentée précédemment sur son changement d'état.
Cependant, les ennemis ne sont jamais détruits et restent figés à l'écran.

Nous devons donc lancer une animation de mort lors du changement d'état puis détruire l'objet à la fin de l'animation.
Le système d'événements d'animation génère un événement `ANIM_EVENT_ANIMATION_END` lorsqu'une animation se termine. Attention, cet événement n'est jamais généré pour les animations en boucle infinie (`loopCount` = -1). Vous pouvez utiliser l'événement `ANIM_EVENT_CYCLE_END` pour ce type d'animation, qui est généré à chaque fois que l'animation boucle.

> enemy_data.c -- Enemy_onUpdate()
>
> Décommentez le bloc `case ENEMY_STATE_DYING` qui change l'animation pour `data->dyingAnimId`.
> Cette animation est lancée au moment du passage à l'état dying.
> L'affectation de l'id permet de ne pas rentrer à nouveau dans ce case lors de la prochaine frame.
>
> Décommentez également le bloc complet de détection de fin d'animation afin de détruire l'ennemi lorsque son animation de mort est terminée (boucle `for` sur `mainAnim->eventCount`).

Félicitations ! Vous avez implémenté le cœur du gameplay de Space Pixels.
La boucle de jeu est maintenant fonctionnelle : tirer, toucher, détruire, marquer des points.
Il reste à finaliser la transition depuis la fin du niveau et à créer autant d'ennemis et de vagues que souhaité.

### [TODO level] - Niveaux et vagues d'ennemis

> [!NOTE]
> Fichiers à modifier : game/scene/level/level_01.c, game/scene/level_scene.c

Le système de niveau repose sur un mécanisme de vagues scriptées : chaque niveau est défini dans son propre fichier au travers d'une fonction `LevelScene_updateLevelX()` qui contient un `switch/case` où chaque `case` représente une vague d'ennemis à faire apparaître.
Le jeu appelle cette fonction à chaque fois que tous les ennemis de la vague précédente sont éliminés (détection via `g_gameConfig.level.enemyCount` qui est incrémenté dans `Enemy_init()` et décrémenté dans `Enemy_onDestroy()`).
Lorsque toutes les vagues sont complétées, la fonction `LevelScene_onLevelCompleted()` est appelée pour ouvrir l'écran de fin de niveau.
Si tous les joueurs sont éliminés, `LevelScene_onLevelFailed()` déclenche l'écran de game over.

#### Ajout d'une nouvelle vague d'ennemis sur le niveau 1

Le fichier `game/scene/level/level_01.c` contient la définition du premier niveau.
Le système de vagues fonctionne ainsi :
1. Au début du niveau, `level->waveIdx` est initialisé à 0
2. `LevelScene_updateGame()` appelle `LevelScene_updateLevel1()` lorsque `level->enemyCount` atteint zéro (tous les ennemis de la vague précédente sont détruits)
3. La fonction lit `level->waveIdx` pour savoir quelle vague spawner, crée les ennemis, puis incrémente `level->waveIdx`
4. Le `default:` du switch est atteint lorsque toutes les vagues sont terminées, déclenchant `LevelScene_onLevelCompleted()`

Notez que chaque appel à `Fighter_create()` incrémente automatiquement `level->enemyCount` dans `Enemy_init()`, et chaque destruction d'ennemi le décrémente dans `Enemy_onDestroy()`.
Ce compteur permet au système de savoir quand une vague est terminée sans avoir à parcourir manuellement tous les objets du jeu.

> level_01.c -- LevelScene_updateLevel1()
>
> Décommentez le bloc `case 2:` complet (4 lignes) pour activer une troisième vague.
> N'hésitez pas à modifier les positions pour tester d'autres placements.

Vous avez peut-être remarqué que le lancement du niveau 2 se soldait par une erreur.
C'est normal, puisqu'il n'existe aucun appel à la mise à jour de ce niveau.

> level_scene.c -- LevelScene_updateGame()
>
> Décommentez l'appel à la fonction `LevelScene_updateLevel2()` pour activer la mise à jour du niveau.

#### Étape 2 : Activation de la transition de victoire ou de défaite

Actuellement, lorsque toutes les vagues sont terminées, la fonction `LevelScene_onLevelCompleted()` est bien appelée, mais elle ne fait rien de visible : le joueur reste bloqué sur un écran vide sans indication de victoire.
Nous devons ouvrir la page de fin de niveau et **geler le temps** pour permettre au joueur de contempler son score sans que le jeu continue à tourner en arrière-plan.
L'**échelle de temps** (_time scale_) est un multiplicateur appliqué au temps écoulé (`dt`) transmis à toutes les fonctions `onUpdate()`.
Avec une échelle de 0, `dt` devient 0 et plus aucun objet ne bouge ni ne s'anime, créant un effet de pause visuel parfait pour contempler la victoire.

> level_scene.c -- LevelScene_onLevelCompleted()
>
> Décommentez les deux lignes dans le bloc [TODO level] pour ouvrir la page de fin et geler le temps.

> level_scene.c -- LevelScene_onLevelFailed()
>
> Décommentez les deux mêmes lignes dans le bloc [TODO level].

La gestion des pages est déléguée à la partie UI.
Vous pourrez retrouver le contenu de la page de victoire dans le fichier `game/ui/level_end_page.c`.
Nous étudierons la gestion de l'UI dans un autre document.



### [TODO new enemy] - Création d'un nouvel ennemi : la Frigate

> [!NOTE]
> Fichiers à créer : game/level/frigate.c, game/level/frigate.h
> Fichiers à modifier : common/assets.h, common/assets.c, game/scene/level/level_01.c

Vous avez maintenant tous les outils pour créer vos propres ennemis !
Pour vous aider à démarrer, le basecode fournit deux fichiers **template** (`template_enemy.c/.h`) qui contiennent la structure de base d'un ennemi prêt à être dupliqué et adapté.
Dans cette section, vous allez créer un nouvel ennemi appelé **Frigate**, un vaisseau plus imposant que le Fighter, avec un comportement plus complexe basé sur des **phases alternées** : une phase de tir intensif (3 salves de projectiles) suivie d'une phase défensive où le vaisseau active son bouclier pour se protéger.

#### Étape 1 : Duplication des fichiers template

La première étape consiste à créer les fichiers de la Frigate à partir des templates fournis.
Via l'explorateur de solution, créez deux nouveaux fichiers `frigate.c` et `frigate.h` en faisant un clic droit sur le dossier `level`.
Vérifiez que les nouveaux fichiers sont bien créés dans le dossier `game/level/` et non à la racine du projet. Vérifiez également que ces fichiers sont bien ajoutés au `CMakeLists.txt` du dossier `application/` pour être compilés.
Copiez ensuite le contenu des templates dans ces fichiers nouvellement créés.

Dans chaque fichier, vous devez maintenant remplacer toutes les occurrences de `TemplateEnemy` par `Frigate`.
Utilisez la fonction **Ctrl+H** (Rechercher/Remplacer) de votre éditeur pour effectuer ce remplacement automatiquement dans les deux fichiers.
- Cochez l'option "Respecter la casse" pour ne remplacer que les occurrences exactes de `TemplateEnemy` et pas d'autres mots qui pourraient contenir cette séquence de caractères.
- Ne cochez pas l'option "Mot entier" car les occurrences à remplacer ne sont pas des mots entiers (ex : `TemplateEnemyVM_onUpdate` doit devenir `FrigateVM_onUpdate`).
- Remplacez les occurrences fichier par fichier en sélectionnant "Document en cours" et non "Solution complète" pour éviter de remplacer des occurrences dans d'autres fichiers.
- Pensez à remplacer les chemins des includes !

> [!WARNING]
> **Attention** : Les fonctions **doivent rester `static`** dans le fichier `.c` (sauf `Frigate_create` qui est public).
> Ne supprimez pas le mot-clé `static` devant les méthodes virtuelles (`FrigateVM_onUpdate`, `FrigateVM_onRender`, etc.).
> Cela permet de limiter leur visibilité au seul fichier `frigate.c`.

#### Étape 2 : Chargement des ressources de la Frigate

Comme pour le Fighter, vous devez enregistrer les sprites et animations de la Frigate dans le système d'assets.

> assets.h
>
> Décommentez la ligne `SPRITE_FRIGATE` dans l'énumération `GameSpriteId`.
> Décommentez les quatre lignes d'animations de la Frigate dans l'énumération `GameAnimId`.

> assets.c
>
> Décommentez la ligne de la Frigate dans le tableau `sheetsToLoad[]`.
> Décommentez les quatre lignes d'animations de la Frigate dans le tableau `animsToLoad[]`.

Observez que l'animation `ANIM_FRIGATE_FIRING` a un paramètre `loopCount` de `3` au lieu de `-1`.
Cela signifie qu'elle se jouera **exactement 3 fois** avant de se terminer, générant un événement `ANIM_EVENT_ANIMATION_END` que vous pourrez détecter pour changer de phase.

#### Étape 3 : Configuration de la Frigate dans frigate.c

Maintenant que les ressources sont chargées, configurez les propriétés de la Frigate dans la fonction `Frigate_create()`.

> frigate.c -- Frigate_create()
>
> Dans le bloc d'initialisation de `EnemyData`, remplacez les identifiants d'animations par ceux de la Frigate.
> Modifiez également les points de vie et les points attribués pour refléter la puissance de la Frigate.
> Ajustez le rayon du collider (inspirez-vous de ce que vous avez fait pour le Fighter)

#### Étape 4 : Implémentation du système de phases

La Frigate alterne entre deux phases :
- **Phase de TIR** : L'animation `ANIM_FRIGATE_FIRING` se joue 3 fois (rotation du vaisseau), et à chaque frame 1 et 3, la Frigate tire des projectiles (comme le Fighter)
- **Phase de DÉFENSE** : Après les 3 cycles de tir, la Frigate passe en animation `ANIM_FRIGATE_IDLE` et active son bouclier (`ANIM_FRIGATE_SHIELD`). Elle reste ainsi pendant un certain temps avant de recommencer à tirer.

Pour gérer ces phases, vous aurez besoin d'ajouter des attributs à la structure `Frigate` :

> frigate.h -- Structure Frigate
>
> Ajoutez ces membres après `EnemyData m_enemyData;` :
> ```c
> float m_accuPhase;        // Timer pour la phase de défense
> bool m_isShieldActive;     // Indique si le bouclier est actif
> ```

> frigate.c -- Frigate_create()
>
> Initialisez ces nouveaux membres à la fin de la fonction, juste avant les appels aux méthodes VM :
> ```c
> self->m_accuPhase = 0.f;
> self->m_isShieldActive = false;
> ```

Maintenant, implémentez la logique de phases dans `FrigateVM_onUpdate()`.
Inspirez-vous du code du `FighterVM_onUpdate()` pour :
1. Appeler `Enemy_onUpdate()` pour mettre à jour l'animation courante
2. Écouter l'événement `ANIM_EVENT_ANIMATION_END` sur l'animation de tir pour détecter la fin des 3 cycles
3. Lorsque l'animation de tir se termine, activer le bouclier et démarrer le timer de phase défensive
4. Décrémenter le timer de phase à chaque frame (`self->m_accuPhase -= dt;`)
5. Lorsque le timer atteint zéro, désactiver le bouclier et relancer l'animation de tir

Pour les tirs pendant la phase de tir, utilisez la même logique que le Fighter (détection des bonnes frames pour l'animation de tir).

> [!TIP]
> **Indice pour le timer de phase** : Une durée de 3 secondes (`3.f`) pour la phase défensive offre un bon équilibre.
> Vous pouvez définir une constante en haut du fichier : `#define FRIGATE_DEFENSE_PHASE_DURATION 3.f`.

> [!TIP]
> **Activation du bouclier** : Pour activer le bouclier, vous devez modifier `enemyData->hasShield` à `true` et changer l'animation principale.
> Pour le désactiver, remettez `hasShield` à `false` et repassez à l'animation de tir.
> Regardez comment `Enemy_onUpdate()` gère l'affichage du bouclier dans `enemy_data.c` pour comprendre le mécanisme.

#### Étape 5 : Ajout de la Frigate au niveau

Une fois votre Frigate fonctionnelle, ajoutez-la dans une nouvelle vague du niveau 1.

> level_01.c
>
> N'oubliez pas d'ajouter l'include en haut du fichier
>
> Ajoutez un nouveau case dans le switch de `LevelScene_updateLevel1()` en appelant correctement la création de la Frigate.

---------------------------------------------------
# Pour aller plus loin

Vous avez maintenant la méthode complète pour créer n'importe quel type d'ennemi !
Voici quelques idées pour enrichir votre jeu :
- Créez un ennemi qui se déplace pendant qu'il tire
- Créez un ennemi "tank" très lent mais avec beaucoup de points de vie
- Créez un ennemi qui tire des missiles à tête chercheuse (`HomingMissile` est déjà disponible mais nécessite des ajustements)
- Ajoutez des patterns de tir plus complexes (tir en éventail, tir circulaire...)
- Créez un boss avec plusieurs phases et des points de vie élevés

Félicitations ! Vous maîtrisez maintenant l'ensemble du basecode de Space Pixels et pouvez créer le jeu de vos rêves. 🚀

