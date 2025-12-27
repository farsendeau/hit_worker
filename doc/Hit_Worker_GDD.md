HIT WOKER

**Game Design Document**

*Platformer 2D - Style Mega Man*

Version MVP

Décembre 2024

Table des matières

1\. Vue d\'ensemble du projet

1.1 Concept général

Platformer 2D de type Mega Man sur NES avec scrolling principalement
horizontal (gauche ↔ droite) mais pouvant inclure des phases verticales
(bas ↔ haut).

1.2 Caractéristiques techniques

-   8 niveaux au total (7 niveaux normaux + 1 niveau final)

-   Maximum 20 minutes de gameplay par niveau

-   Un boss par niveau

-   Résolution : 1 écran = 20×12 tuiles de 16×16px (320×192px)

-   16 écrans par niveau

-   Points de respawn : milieu du niveau (écran 8) et avant le boss

2\. Univers et contexte

2.1 Histoire

Johnny, un homme beau et musclé, se voit enlever sa copine par un gang
de méchants se faisant appeler \"les Wokes\". Il va devoir traverser les
niveaux de 7 boss avant d\'affronter le niveau final avec le chef du
gang et délivrer sa copine.

2.2 Direction artistique

**Style visuel :** 8-bit, néo-rétro

**Palette de couleurs :**

-   #360185 - Violet foncé (couleur principale)

-   #8F0177 - Magenta

-   #DE1A58 - Rose

-   #F4B342 - Orange doré

**Environnements prévus :** Ville, usine, gratte-ciel, etc.

3\. Personnage joueur - Johnny

3.1 Capacités de déplacement

-   Déplacement horizontal (avant/arrière)

-   Saut

-   Montée/descente d\'échelles

3.2 Statistiques de Johnny

  ----------------------- -----------------------------------------------
  **Attribut**            **Valeur**

  **Vie maximale**        100 points

  **Vies de départ**      4 vies

  **Vitesse de            2px par frame
  déplacement**           

  **Hitbox**              14×14 pixels (plus petite que le sprite pour
                          gameplay permissif)

  **Invincibilité après   90 frames (1.5 sec à 60fps)
  hit**                   
  ----------------------- -----------------------------------------------

3.3 Système de saut et gravité

  ----------------------- -----------------------------------------------
  **Paramètre**           **Valeur**

  **Vélocité initiale**   6px/frame

  **Gravité**             0.4px/frame²

  **Hauteur max**         64px (4 tuiles)

  **Hauteur min**         24px (1.5 tuiles)

  **Contrôle aérien**     Oui, 2px/frame horizontal

  **Saut variable**       Oui (réduction 50% si touche relâchée tôt)

  **Durée saut complet**  30 frames (\~0.5 sec)
  ----------------------- -----------------------------------------------

3.4 Arsenal et armes

Johnny peut frapper avec ses poings ou utiliser diverses armes à
distance. La sélection de l\'arme se fait soit par le menu pause, soit
directement en appuyant sur une touche qui incrémente l\'arme en cours.

**Pour la MVP :** Poing (CaC), Pistolet (distance) et Grenade (zone)

  ----------------- ----------------- ----------------- -----------------
  **Arme**          **Dégâts**        **Coût**          **Type**

  **Poing**         20                Gratuit           CaC

  **Pistolet**      20                1 balle           Distance

  **Grenade**       50                1 grenade         Zone (2 tuiles)
  ----------------- ----------------- ----------------- -----------------

3.5 Système de vies et continues

Johnny possède au départ 4 vies. S\'il arrive à 0 vie, on propose au
joueur un \"Continue\" (continues infinis). Si le continue est accepté,
Johnny recommence depuis le début du niveau en cours avec 4 vies.

4\. Système de progression et items

4.1 Mécanique de drop

Les items sont récupérables après avoir tué un ennemi. Un système
aléatoire détermine si l\'ennemi drop un item de soin, un item de
munition, ou rien.

**Limite de stockage :** Maximum 255 pour tous les items type
munition/vie (1 octet).

4.2 Items de soin

  ----------------------- ----------------------- -----------------------
  **Item**                **Effet**               **Chance de drop**

  **Medium Life**         Remet 30% de vie        70%

  **Full Life**           Remet 100% de vie       25%

  **1-UP**                Vie supplémentaire      5%
  ----------------------- ----------------------- -----------------------

4.3 Items de munitions

  ----------------------- ----------------------- -----------------------
  **Item**                **Effet**               **Chance de drop**

  **Balles Pistolet**     +10 balles              90%

  **Grenade**             +1 grenade              10%
  ----------------------- ----------------------- -----------------------

***Note :** Les pourcentages devront être ajustés si de nouvelles
armes/items sont ajoutés.*

5\. Ennemis et adversaires

5.1 Règles générales des ennemis

-   Tous les ennemis ont 30 frames d\'invincibilité après avoir été
    touchés (0.5 sec à 60fps)

-   Tous les ennemis drop des items selon le système de pourcentage
    défini

-   Tous les ennemis mobiles sont affectés par la gravité (sauf ennemis
    volants)

-   Un ennemi est considéré comme actif dès qu\'il spawn dans la caméra

-   S\'ils sortent de la caméra, ils sont considérés comme détruits

**Catégories d\'ennemis :** Humanoïde, Mécanique, Animal

5.2 FIONEUR (Ennemi humanoïde)

Ennemi humanoïde qui court vers le joueur pour entrer au contact.

  ----------------------- -----------------------------------------------
  **Attribut**            **Valeur**

  **Type d\'ennemi**      Humain

  **Points de vie**       40

  **Dégâts**              10% (10 points)

  **Type de dégât**       CaC (à chaque contact)

  **Vitesse**             1px par frame

  **Pattern**             Court vers le joueur pour entrer au contact
                          (rayon d\'activation: 10 tuiles)

  **Sensibilité armes**   Poing 100%, Pistolet 100%, Grenade 150%

  **Pour tuer**           2 coups de poing, 2 balles de pistolet, 1
                          grenade
  ----------------------- -----------------------------------------------

5.3 TOURELLE GAD (Ennemi mécanique)

Tourelle fixe qui lance des projectiles en croix dans quatre directions.

  ----------------------- -----------------------------------------------
  **Attribut**            **Valeur**

  **Type d\'ennemi**      Mécanique

  **Points de vie**       60

  **Dégâts**              15% (15 points) - projectile et CaC tourelle

  **Type de dégât**       Distance et CaC

  **Vitesse**             0px par frame (immobile)

  **Cadence de tir**      Toutes les 2 secondes (120 frames à 60fps)

  **Pattern**             Tourelle fixe qui lance 4 projectiles en croix
                          (nord, sud, est, ouest)

  **Vitesse projectile**  3px par frame

  **Sensibilité armes**   Poing 50%, Pistolet 100%, Grenade 150%

  **Pour tuer**           6 coups de poing, 3 balles de pistolet, 1
                          grenade
  ----------------------- -----------------------------------------------

6\. Boss - SADIMAN

Boss humain du niveau MVP avec deux phases de combat distinctes.

  ----------------------- -----------------------------------------------
  **Attribut**            **Valeur**

  **Type d\'ennemi**      Humain (Boss)

  **Points de vie**       200

  **Dégâts**              20% (20 points)

  **Arme**                Fouet / Poing

  **Sensibilité armes**   Poing 90%, Pistolet 100%, Grenade 120%

  **Pour tuer**           12 coups de poing, 10 balles de pistolet, 5
                          grenades
  ----------------------- -----------------------------------------------

6.1 Phase 1 : Combat au corps-à-corps (100% - 50% HP)

-   Vitesse : 3px par frame

-   Attaque : Charge Johnny au CaC

-   Cooldown : 60 frames (1 sec)

6.2 Phase 2 : Combat à distance (50% - 0% HP)

-   Vitesse : 5px par frame

-   Comportement : Avance vers Johnny jusqu\'à 50px

-   Attaque : Coup de fouet (portée 80px)

-   Cooldown : 90 frames (1.5 sec)

7\. Système de collision

7.1 Philosophie générale

Les collisions sont calculées chaque frame à 60 FPS. Avec seulement 2
types d\'ennemis de base et un boss pour la MVP, le calcul des
collisions chaque frame ne devrait pas poser de problème de performance.
Cette approche évite les bugs (projectiles qui traversent, hits non
détectés) et simplifie le code. L\'optimisation \"1 frame sur 2\" est
gardée en réserve uniquement si des problèmes de performance surviennent
lors des tests.

7.2 Hitboxes

**Forme :** Rectangulaire pour toutes les entités

**Contrôle :** Vérification des 4 coins de chaque hitbox

  ----------------------- -----------------------------------------------
  **Entité**              **Configuration**

  **Johnny (Joueur)**     14×14 pixels (plus petite que le sprite pour
                          gameplay permissif)

  **Ennemis**             Identique à la taille du sprite (collision
                          cohérente)

  **Projectiles**         Identique à la taille du sprite
  ----------------------- -----------------------------------------------

7.3 Types de tuiles

-   Type 0 : Vide (air) - pas de collision

-   Type 1 : Bloc dur (sol, mur) - collision solide

-   Type 2 : Échelle - collision spéciale, permet montée/descente

-   Type 3 : Plateforme semi-solide (optionnel) - collision uniquement
    par le dessus

7.4 Collision joueur / décor

Collision verticale (gravité et plateformes)

-   Vérifier le type de tuile sous le joueur (position Y + hauteur)

-   Si tuile de type \"bloc dur\" : collision détectée, joueur ne tombe
    pas

-   Si tuile de type \"vide\" : joueur tombe (gravité appliquée)

-   Si tuile de type \"échelle\" ET flèche bas appuyée : init mouvement
    de descente

Collision horizontale (déplacement)

-   Vérifier la tuile devant le joueur (dans la direction du mouvement)

-   Si tuile de type \"mur\" : stop le déplacement du joueur

-   Si tuile de type \"vide\" : mouvement autorisé

Interaction échelles

-   Montée : si touche haut activée ET tuile au-dessus est de type
    \"échelle\" → init mouvement de montée

-   Descente : si touche bas activée ET tuile actuelle ou en-dessous est
    de type \"échelle\" → init mouvement de descente

-   Sur échelle : désactivation de la gravité, vitesse verticale
    contrôlée par input

7.5 Ordre de traitement des collisions (par frame)

1\. Collision joueur / décor (déplacement et gravité)

2\. Collision ennemis / décor (déplacement et gravité)

3\. Collision projectiles / décor

4\. Collision joueur / ennemis

5\. Collision projectiles joueur / ennemis

6\. Collision projectiles ennemis / joueur

8\. Interface utilisateur

8.1 HUD (Affichage en jeu)

-   En haut à gauche : Arme sélectionnée et nombre de munitions
    restantes

-   En dessous : Barre de vie sous forme de jauge verticale

-   La barre de vie descend en fonction des dégâts reçus

-   Si Johnny n\'a plus de vie, le joueur perd une vie

8.2 Menu pause

En appuyant sur une touche (Select), le joueur affiche un menu pause
contenant :

-   Récapitulatif des armes avec munitions restantes

-   Jauge de vie

-   Nombre de vies

-   Possibilité d\'arrêter le niveau en cours pour revenir au menu
    principal

8.3 Note MVP

Pour la MVP, il n\'y aura pas de système de sauvegarde.

9\. Contrôles

Les contrôles restent simplistes, inspirés des jeux NES classiques.

  ----------------------- -----------------------------------------------
  **Touche**              **Action**

  **Flèche Haut**         Déplacement vers le haut (menu) / Montée
                          d\'échelle

  **Flèche Droite**       Déplacement vers la droite

  **Flèche Bas**          Déplacement vers le bas (menu) / Descente
                          d\'échelle

  **Flèche Gauche**       Déplacement vers la gauche

  **Bouton A**            Tirer / Valider dans le menu

  **Bouton B**            Sauter

  **Select**              Ouvrir le menu en jeu (pause)

  **Start**               Valider
  ----------------------- -----------------------------------------------

10\. Audio et musique

10.1 Style audio

Le style de l\'audio sera en 8-bit, cohérent avec la direction
artistique néo-rétro du jeu.

10.2 Musiques

Chaque partie du jeu aura un thème musical distinct :

-   Écran de titre

-   Niveau (gameplay)

-   Écran de continue

-   Écran de fin

10.3 Effets sonores

Les effets sonores essentiels pour la MVP :

-   Tirs (différents sons pour poing, pistolet, grenade)

-   Dégâts reçus par le joueur

-   Dégâts infligés aux ennemis

-   Saut

-   Mort d\'un ennemi

-   Ramassage d\'item

11\. Définition du MVP (Minimum Viable Product)

11.1 Objectif du MVP

Le MVP vise à créer une version jouable et complète d\'un niveau entier,
avec tous les systèmes de jeu de base fonctionnels. Cette version
permettra de valider le gameplay, l\'équilibrage et la direction
artistique avant de développer les 7 autres niveaux.

11.2 Contenu du MVP

Écrans

-   Un écran d\'intro avec le titre du jeu

-   Un niveau entier (16 écrans)

-   Un écran de fin (victoire)

Niveau

-   Sprites simplifiés mais aux bonnes tailles

-   Un début et une fin clairs

-   Un point de respawn au milieu du niveau

-   Un type d\'ennemi de chaque catégorie (FIONEUR + TOURELLE GAD)

-   Un boss (SADIMAN)

-   Système de drop d\'items fonctionnel

11.3 Structure du niveau 1 (MVP)

Le niveau 1 sera structuré comme suit :

  -------------- --------------------------------------------------------
  **Écran**      **Contenu**

  **1**          Initialisation du personnage au centre

  **2, 6, 10,    Ennemis FIONEUR
  12**           

  **4, 15**      TOURELLE GAD

  **8**          CHECKPOINT (point de respawn)

  **15**         CHECKPOINT avant boss

  **16**         Boss SADIMAN
  -------------- --------------------------------------------------------

***Note :** Il faudra probablement ajouter plus d\'ennemis lors des
tests de gameplay pour optimiser la difficulté et le rythme.*

11.4 Fonctionnalités exclues du MVP

-   Système de sauvegarde

-   Menu de sélection de niveaux

-   Armes supplémentaires (fusil, etc.)

-   Cinématiques élaborées

-   Système de scoring

12\. Points à clarifier avant développement

Ces éléments peuvent être décidés pendant le prototypage, mais il est
recommandé de les définir pour éviter des changements majeurs en cours
de développement.

12.1 Boss SADIMAN - Phase 2

-   Quand il avance jusqu\'à 50px du joueur, que fait-il après avoir
    donné son coup de fouet ?

-   Option A : Recule à sa position de départ

-   Option B : Reste à cette distance et continue d\'attaquer

-   Le coup de fouet est-il une hitbox instantanée ou un projectile
    visible qui se déplace ?

12.2 TOURELLE GAD - Projectiles

-   Les 4 projectiles en croix partent-ils tous simultanément toutes les
    2 secondes ?

-   Ou y a-t-il un délai entre chaque projectile (ex: 0.1 sec entre
    chaque) ?

12.3 Durée de vie des projectiles

-   Les balles du pistolet et les projectiles des tourelles
    disparaissent après combien de distance ?

-   Option A : Traversent tout l\'écran jusqu\'à collision avec un mur

-   Option B : Portée limitée (ex: 15 tuiles, soit 240px)

12.4 Grenade - Mécanique

La grenade est définie comme ayant une \"zone de 2 tuiles\", mais la
mécanique exacte doit être précisée :

-   Trajectoire parabolique comme dans Metal Slug ?

-   Explosion instantanée au contact du sol ?

-   Délai avant explosion (ex: 2 secondes) ?

-   La zone de 2 tuiles est-elle un rayon (4 tuiles de diamètre total)
    ou un diamètre ?

12.5 Capacité maximale de munitions

Bien que la limite technique soit de 255, définir des maximums pratiques
peut aider le level design :

-   Balles pistolet max : 99 ? 150 ? 255 ?

-   Grenades max : 20 ? 50 ? 99 ?

13\. Notes de développement

13.1 Architecture technique

Le jeu utilise C++ avec Allegro 5 comme framework graphique. Un système
de compression par metatiles a été développé, réduisant l\'utilisation
mémoire d\'environ 53% en convertant les tuiles individuelles 16×16 en
patterns de blocs 2×2 réutilisables.

13.2 Outils de développement

-   Éditeur de code : Visual Studio Code

-   Éditeur de niveaux : Tiled Map Editor

-   Résolution virtuelle cible : 320×180

13.3 Framerate et performance

Le jeu cible 60 FPS constant. Toutes les valeurs de mouvement et de
timing sont calculées en fonction de ce framerate. Le système de
collision calcule toutes les collisions chaque frame pour éviter les
bugs, l\'optimisation \"1 frame sur 2\" étant gardée en réserve
uniquement si des problèmes de performance surviennent.

13.4 Prochaines étapes

Une fois le MVP complété et testé, les priorités seront :

-   Équilibrage du gameplay basé sur les tests

-   Création des 7 niveaux restants

-   Développement des 7 autres boss

-   Ajout d\'armes supplémentaires

-   Création des assets graphiques finaux

-   Composition de la bande sonore complète

-   Implémentation du système de sauvegarde

*--- Fin du document ---*
