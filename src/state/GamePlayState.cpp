#include "state/GamePlayState.hpp"
#include "state/DeathState.hpp"
#include "core/StateManager.hpp"
#include "entity/DummyEnemy.hpp"

GamePlayState::GamePlayState(StateManager* sm)
    : stateManager(sm)
{
    // Link player to this game state (for weapon system)
    player.setGameState(this);

    std::string filename{"asset/level/tileset/" + std::to_string(currentLevel) + ".jpg"};
    setTileset(filename);

    // Trouver la zone de départ du joueur
    currentZoneId = findCameraZone(player.getCenterX(), player.getCenterY());

    // Positionner la caméra sur la zone de départ
    const CameraZone& startZone = cameraZones[currentZoneId];
    camera.setY(startZone.y);

    // Initialiser le premier respawn point si la zone de départ est une zone de respawn
    if (startZone.zone_respawn) {
        float respawnX = startZone.x + startZone.width / 2.0f - player.getWidth() / 2.0f;
        float respawnY = startZone.y + startZone.height / 2.0f - player.getHeight() / 2.0f;
        player.setRespawnPoint(respawnX, respawnY);
        lastRespawnZoneId = currentZoneId;
        DEBUG_LOG("Initial respawn zone set: Zone %d\n", currentZoneId);
    }

    // ========================================
    // ⚠️⚠️⚠️ TODO CRITIQUE - ABSOLUMENT À FAIRE ⚠️⚠️⚠️
    // ========================================
    // HARDCODE TEMPORAIRE: Les positions d'enemies sont hardcodées ci-dessous.
    //
    // OBJECTIF FINAL: Parser le layer object "enemy" depuis Tiled (.tmx)
    // - Modifier hitwoker_tiled/main.cpp pour parser les objets du layer "enemy"
    // - Générer une structure C++ avec les spawn points (id_type, x, y)
    // - Charger ces données dans GamePlayState
    //
    // RÉFÉRENCES:
    // - doc/guide/hitwoker_tiled.md : Documentation de l'outil Tiled
    // - TODO Phase 4-5, Itération 6 : Enemy Spawning System
    // - Layer Tiled: "enemy" avec propriété "id_type" (int ou string)
    //
    // IMPORTANT: Ce hardcode doit être supprimé à l'Itération 6!
    // ========================================

    // Spawner 1 DummyEnemy pour test (Itération 1)
    enemies[0] = std::make_unique<DummyEnemy>(200.0f, 100.0f);

    DEBUG_LOG("GamePlayState initialized\n");
    DEBUG_LOG("Level: %d\n", currentLevel);
    DEBUG_LOG("Player starting in zone: %d (x=%.0f, y=%.0f)\n", currentZoneId, startZone.x, startZone.y);
    DEBUG_LOG("⚠️ ENEMY POSITIONS HARDCODED - TODO: Parser depuis Tiled!\n");
}

GamePlayState::~GamePlayState()
{
}

void GamePlayState::update(const InputState &input)
{
    #ifdef DEBUG
    // ===== MODE FRAME BY FRAME (DEBUG) =====
    // Toggle du mode frame by frame avec la touche M
    if (input.debugFrameByFrame && !mKeyPreviouslyPressed) {
        frameByFrameMode = !frameByFrameMode;
        DEBUG_LOG("Frame by frame mode: %s\n", frameByFrameMode ? "ON" : "OFF");
    }
    mKeyPreviouslyPressed = input.debugFrameByFrame;

    // Si le mode frame by frame est activé
    if (frameByFrameMode) {
        // Avancer d'une frame seulement si RIGHT arrow est pressée
        if (input.debugNextFrame && !nextFrameKeyPreviouslyPressed) {
            nextFrameKeyPreviouslyPressed = input.debugNextFrame;
            DEBUG_LOG("Advancing one frame\n");
            // Continuer l'exécution normale pour cette frame
        } else {
            nextFrameKeyPreviouslyPressed = input.debugNextFrame;
            return; // Pause: ne pas exécuter cette frame
        }
    }
    #endif

    // 1. Update joueur (physique, input, collisions)
    if (!isTransitioning) {
        // Mode normal: physique + inputs actifs
        player.update(input, level);
    }
    // Pendant transition verticale: player.update() n'est PAS appelé!
    // → Pas de gravité, pas d'inputs, joueur "gelé"
    // → Son mouvement est contrôlé uniquement par updateVerticalTransition()

    // 1.5. Détection de la mort du joueur
    if (player.getState() == Player::State::DEAD && !isInDeathSequence) {
        isInDeathSequence = true;  // Flag pour éviter double-push
        DEBUG_LOG("Player died, pushing DeathState\n");

        // Pusher le DeathState avec les paramètres actuels
        stateManager->push(new DeathState(this, stateManager, lastRespawnZoneId, player.getLives(), currentLevel));
        return;  // Arrêter le traitement de cette frame
    }

    // 2. Si en transition verticale
    if (isTransitioning) {

        updateVerticalTransition();  // Scroll la caméra Y + déplace le joueur

        // Continuer à suivre le joueur horizontalement
        camera.setX(player.getCenterX() - VIRTUAL_WIDTH / 2.0f);

        // Limiter X aux bords de la zone courante
        const CameraZone& zone = cameraZones[currentZoneId];
        if (camera.getX() < zone.x) {
            camera.setX(zone.x);
        }
        if (camera.getX() + VIRTUAL_WIDTH > zone.x + zone.width) {
            camera.setX(zone.x + zone.width - VIRTUAL_WIDTH);
        }

        // Appliquer les boundaries du joueur
        applyZoneBoundaries();
        return;
    }

    // 3. Détection des transitions de zone
    detectZoneChange();

    // 4. Si une transition vient de commencer
    if (isTransitioning) {
        return;
    }

    // 5. Mode normal: caméra suit le joueur horizontalement
    const CameraZone& currentZone = cameraZones[currentZoneId];

    // Suivre le joueur horizontalement
    camera.follow(player);

    // Limiter la caméra selon les zones adjacentes
    // Si pas de zone à droite, bloquer au bord droit de la zone actuelle
    float maxCameraX;
    if (currentZone.next_zone_right >= 0) {
        // Zone à droite existe, scrolling libre jusqu'au bout du niveau
        maxCameraX = (MAP_WIDTH_TILES * TILE_SIZE) - VIRTUAL_WIDTH;
    } else {
        // Pas de zone à droite, bloquer à cette zone
        maxCameraX = currentZone.x + currentZone.width - VIRTUAL_WIDTH;
    }

    // Si pas de zone à gauche, bloquer au bord gauche de la zone actuelle
    float minCameraX;
    if (currentZone.next_zone_left >= 0) {
        // Zone à gauche existe, scrolling libre jusqu'au début
        minCameraX = 0;
    } else {
        // Pas de zone à gauche, bloquer à cette zone
        minCameraX = currentZone.x;
    }

    // Appliquer les limites
    if (camera.getX() < minCameraX) camera.setX(minCameraX);
    if (camera.getX() > maxCameraX) camera.setX(maxCameraX);

    // Fixer Y à la position de la zone
    camera.setY(currentZone.y);

    // 6. Update projectiles
    updateProjectiles(input);

    // 6.5. Update enemies
    updateEnemies(input);

    // 6.6. Collision projectiles vs enemies
    checkProjectileEnemyCollisions();

    // 7. Limiter le joueur aux bords de la zone
    applyZoneBoundaries();
}

void GamePlayState::render()
{
    // Vérifier que le tileset est chargé
    if (!tileset) {
        return;  // Ne rien dessiner si le tileset n'est pas chargé
    }

    // Calculer la zone visible (culling)
    int startX = static_cast<int>(camera.getX()) / TILE_SIZE;
    int startY = static_cast<int>(camera.getY()) / TILE_SIZE;
    int endX = startX + (320 / TILE_SIZE) + 1;  // 320 = largeur écran
    int endY = startY + (192 / TILE_SIZE) + 1;  // 192 = hauteur écran

    // Limiter aux bords de la map
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > MAP_WIDTH_TILES) endX = MAP_WIDTH_TILES;
    if (endY > MAP_HEIGHT_TILES) endY = MAP_HEIGHT_TILES;

    // Dessiner chaque tile visible
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            uint8_t tileID = getVisualTileAt(x, y);

            // Position à l'écran (en tenant compte de la caméra)
            int screenX = x * TILE_SIZE - static_cast<int>(camera.getX());
            int screenY = y * TILE_SIZE - static_cast<int>(camera.getY());

            // Dessiner la tile depuis le tileset
            al_draw_bitmap_region(
                tileset,
                tileID * TILE_SIZE, 0,     // Source X, Y dans le tileset
                TILE_SIZE, TILE_SIZE,      // Taille source
                screenX, screenY,          // Position destination
                0                          // Flags
            );
        }
    }

    // Dessiner les enemies (avant le joueur pour layering)
    renderEnemies(camera.getX(), camera.getY());

    // Dessiner le joueur
    player.render(camera.getX(), camera.getY());

    // Dessiner les projectiles
    renderProjectiles(camera.getX(), camera.getY());

    // Dessiner la grille de débogage
    #ifdef DEBUG
        ALLEGRO_COLOR gridColor = al_map_rgba(139, 69, 19, 10);  // Marron semi-transparent

        // Lignes verticales
        for (int x = startX; x <= endX; x++) {
            int screenX = x * TILE_SIZE - static_cast<int>(camera.getX());
            al_draw_line(screenX, 0, screenX, 192, gridColor, 1.0f);
        }

        // Lignes horizontales
        for (int y = startY; y <= endY; y++) {
            int screenY = y * TILE_SIZE - static_cast<int>(camera.getY());
            al_draw_line(0, screenY, 320, screenY, gridColor, 1.0f);
        }

        // Indicateur du mode frame by frame
        if (frameByFrameMode && g_debugFont) {
            ALLEGRO_COLOR textColor = al_map_rgb(255, 255, 0);  // Jaune
            al_draw_text(g_debugFont, textColor, 5, 5, 0, "FRAME BY FRAME MODE");
            al_draw_text(g_debugFont, textColor, 5, 15, 0, "Press RIGHT ARROW to advance");
        }
    #endif
}

void GamePlayState::handleInput()
{
}

int GamePlayState::getCurrentLevel()
{
    return currentLevel;
}

void GamePlayState::setCurrentLevel(int idLevel)
{
    currentLevel = idLevel;
}

void GamePlayState::setTileset(std::string &filename)
{
    tileset = al_load_bitmap(filename.c_str());

    if (!tileset) {
        fprintf(stderr, "ERREUR: Impossible de charger le tileset '%s'\n", filename.c_str());
        fprintf(stderr, "Vérifiez que le fichier existe et que le chemin est correct.\n");
    }
}

/**
 * Détecte les changements de zone (horizontal ET vertical)
 * Utilise findCameraZone() pour détecter la vraie zone du joueur
 */
void GamePlayState::detectZoneChange()
{
    const CameraZone& currentZone = cameraZones[currentZoneId];
    float playerX = player.getCenterX();
    float playerY = player.getCenterY();

    // === Vérifications des limites de la zone actuelle ===
    float zoneBottom = currentZone.y + currentZone.height;
    float zoneTop = currentZone.y;

    // PRIORITÉ 1: Vérifier si le joueur est hors limites VERTICALEMENT
    // Cas 1: Joueur tombé en dessous de la zone actuelle
    if (playerY > zoneBottom) {
        if (currentZone.next_zone_down == -1) {
            // Pas de zone en dessous: mort instantanée
            if (player.getState() != Player::State::DEAD) {
                DEBUG_LOG("Player tombé hors limites bas (zone %d, next_zone_down=-1): MORT\n", currentZoneId);
                player.takeDamage(player.getHp());
            }
            return;
        }
        // Sinon, il y a une zone en dessous, continuer avec la détection normale
    }

    // Cas 2: Joueur monté au-dessus de la zone actuelle
    if (playerY < zoneTop) {
        if (currentZone.next_zone_up == -1) {
            // Pas de zone au-dessus: bloquer le joueur au bord haut
            DEBUG_LOG("Player bloqué au bord haut (zone %d, next_zone_up=-1)\n", currentZoneId);
            player.setY(zoneTop);
            player.setVelocityY(0);
            return;
        }
        // Sinon, il y a une zone au-dessus, continuer avec la détection normale
    }

    // PRIORITÉ 2: Trouver la nouvelle zone du joueur
    int actualZoneId = findCameraZone(playerX, playerY);

    #ifdef DEBUG
    if (actualZoneId != currentZoneId && actualZoneId >= 0) {
        DEBUG_LOG("[detectZoneChange] Player (%.1f, %.1f) zone %d -> actualZone %d\n",
                  playerX, playerY, currentZoneId, actualZoneId);
    }
    #endif

    // Si pas de changement de zone, rien à faire
    if (actualZoneId == currentZoneId || actualZoneId < 0) {
        return;
    }

    // PRIORITÉ 3: Vérifier si findCameraZone() a retourné la zone 0 par défaut
    // Si oui, c'est que le joueur est hors de toutes les zones définies
    const CameraZone& potentialNewZone = cameraZones[actualZoneId];
    bool playerReallyInNewZone = (playerX >= potentialNewZone.x &&
                                   playerX < potentialNewZone.x + potentialNewZone.width &&
                                   playerY >= potentialNewZone.y &&
                                   playerY < potentialNewZone.y + potentialNewZone.height);

    if (!playerReallyInNewZone) {
        // findCameraZone() a retourné une zone par défaut (zone 0)
        // mais le joueur n'est PAS vraiment dans cette zone
        // → Le joueur est tombé dans le vide
        if (player.getState() != Player::State::DEAD) {
            DEBUG_LOG("Player hors de toutes les zones (faux actualZoneId=%d): MORT\n", actualZoneId);
            player.takeDamage(player.getHp());
        }
        return;
    }

    const CameraZone& newZone = cameraZones[actualZoneId];

    // Vérifier si la nouvelle zone est une zone de respawn
    // Ne l'activer que si son ID est supérieur à la dernière zone de respawn activée
    // (évite de réactiver une zone précédente si le joueur revient en arrière)
    if (newZone.zone_respawn && actualZoneId > lastRespawnZoneId) {
        // Calculer le centre de la zone pour le respawn
        float respawnX = newZone.x + newZone.width / 2.0f - player.getWidth() / 2.0f;
        float respawnY = newZone.y + newZone.height / 2.0f - player.getHeight() / 2.0f;

        player.setRespawnPoint(respawnX, respawnY);
        lastRespawnZoneId = actualZoneId;
        DEBUG_LOG("Zone de respawn activée: Zone %d (progression)\n", actualZoneId);
    }

    // Déterminer si c'est horizontal ou vertical
    if (currentZone.y == newZone.y) {
        // Même hauteur Y = transition horizontale (instantané)
        changeZoneHorizontal(actualZoneId);
    } else {
        // Hauteur différente = transition verticale (scroll automatique)
        bool goingDown = newZone.y > currentZone.y;
        startVerticalTransition(actualZoneId, goingDown);
    }
}

/**
 * Change instantanément de zone horizontalement
 * Pas de scroll automatique, juste update de currentZoneId
 * La caméra continuera de suivre le joueur normalement
 */
void GamePlayState::changeZoneHorizontal(int newZoneId)
{
    currentZoneId = newZoneId;
}

/**
 * Démarre une transition verticale (scroll automatique)
 * bool goingDown: true = vers le bas, false = vers le haut
 * Définit la vitesse et direction du scroll
 */
void GamePlayState::startVerticalTransition(int newZoneId, bool goingDown)
{
    isTransitioning = true;
    targetZoneId = newZoneId;
    transitionDirection = TransitionDirection::VERTICAL;

    // NE PAS téléporter le joueur! Il reste à sa position actuelle
    // et se déplacera progressivement pendant updateVerticalTransition()

    #ifdef DEBUG
    DEBUG_LOG("Vertical transition: Zone %d → Zone %d (%s), Player Y: %.1f\n",
              currentZoneId, targetZoneId, goingDown ? "DOWN" : "UP", player.getY());
    #endif
}

/**
 * Scroll la caméra verticalement à 4px/frame
 * Vérifie si on a atteint la zone cible
 */
void GamePlayState::updateVerticalTransition()
{
    if (!isTransitioning) return;

    const CameraZone& targetZone = cameraZones[targetZoneId];
    float targetCameraY = targetZone.y;
    float currentCameraY = camera.getY();

    // Déterminer la direction (haut ou bas)
    if (currentCameraY < targetCameraY) {
        // Scroll vers le bas (caméra descend)
        camera.setY(currentCameraY + VERTICAL_SCROLL_SPEED);

        // Le joueur NE BOUGE PAS en absolu (pas de player.update() appelé)
        // Résultat visuel: le joueur "remonte" sur l'écran (du bas vers le haut)

        if (camera.getY() >= targetCameraY) {
            camera.setY(targetCameraY);
            finishTransition();
        }
    } else if (currentCameraY > targetCameraY) {
        // Scroll vers le haut (caméra monte)
        camera.setY(currentCameraY - VERTICAL_SCROLL_SPEED);

        // Le joueur NE BOUGE PAS en absolu (pas de player.update() appelé)
        // Résultat visuel: le joueur "descend" sur l'écran (du haut vers le bas)

        if (camera.getY() <= targetCameraY) {
            camera.setY(targetCameraY);
            finishTransition();
        }
    } else {
        // Déjà à la bonne position
        finishTransition();
    }
}

/**
 * Termine proprement la transition verticale
 * Reset les flags
 */
void GamePlayState::finishTransition()
{
    // NE PAS repositionner le joueur! Il reste où il est naturellement
    // après le scroll (en haut pour scroll DOWN, en bas pour scroll UP)

    // Vérifier que le joueur est bien dans la zone cible
    const CameraZone& targetZone = cameraZones[targetZoneId];
    float playerCenterY = player.getCenterY();

    // Si le joueur est hors de la zone cible, le repositionner au bord
    if (playerCenterY < targetZone.y) {
        // Trop haut, mettre en haut de la zone
        player.setY(targetZone.y + 5.0f);
    } else if (playerCenterY >= targetZone.y + targetZone.height) {
        // Trop bas, mettre en bas de la zone
        player.setY(targetZone.y + targetZone.height - player.getHeight() - 5.0f);
    }

    currentZoneId = targetZoneId;
    isTransitioning = false;
    targetZoneId = -1;

    #ifdef DEBUG
    DEBUG_LOG("Transition finished. Current zone: %d, Player Y: %.1f\n", currentZoneId, player.getY());
    #endif
}

/**
 * Bloque le joueur aux bords de la zone
 * Important: mort si chute sans next_zone_down
 */
void GamePlayState::applyZoneBoundaries()
{
    const CameraZone& zone = cameraZones[currentZoneId];

    // Limites horizontales - bloquer seulement s'il n'y a pas de zone adjacente
    float zoneLeft = zone.x;
    float zoneRight = zone.x + zone.width;

    // Bloquer à gauche seulement si pas de zone à gauche
    if (player.getX() < zoneLeft && zone.next_zone_left < 0) {
        player.setX(zoneLeft);
    }

    // Bloquer à droite seulement si pas de zone à droite
    if (player.getX() + player.getWidth() > zoneRight && zone.next_zone_right < 0) {
        player.setX(zoneRight - player.getWidth());
    }

    // Limites verticales
    float zoneTop = zone.y;
    float zoneBottom = zone.y + zone.height;

    // Haut: bloquer seulement si pas de zone en haut
    if (player.getY() < zoneTop && zone.next_zone_up < 0) {
        player.setY(zoneTop);
    }

    // Bas: mort si pas de next_zone_down
    if (player.getY() > zoneBottom) {
        if (zone.next_zone_down < 0) {
            // Pas de zone en dessous = mort (comme tomber dans le vide)
            player.takeDamage(player.getHp());

            #ifdef DEBUG
            DEBUG_LOG("Player fell out of zone (no next_zone_down)\n");
            #endif
        }
        // Sinon la transition se déclenchera au prochain frame
    }
}

/**
 * Reset le niveau au dernier point de respawn activé
 * Appelé par DeathState après le fade out
 */
void GamePlayState::resetToRespawn(int zoneId, int lives)
{
    DEBUG_LOG("Resetting to respawn zone %d with %d lives\n", zoneId, lives);

    // Récupérer la zone de respawn
    const CameraZone& zone = cameraZones[zoneId];

    // Calculer position de respawn (centre de la zone)
    float respawnX = zone.x + zone.width / 2.0f - player.getWidth() / 2.0f;
    float respawnY = zone.y + zone.height / 2.0f - player.getHeight() / 2.0f;

    // Reset du joueur
    player.setX(respawnX);
    player.setY(respawnY);
    player.setVelocityX(0.0f);
    player.setVelocityY(0.0f);
    player.setState(Player::State::IDLE);
    player.setHp(100);
    player.setLives(lives);
    player.setInvincibilityFrames(INVINCIBILITY_FRAMES);
    player.setOnGround(false);

    // Reset de la caméra
    currentZoneId = zoneId;
    camera.setX(zone.x);
    camera.setY(zone.y);

    // Reset des flags de transition
    isTransitioning = false;
    isInDeathSequence = false;
    targetZoneId = -1;

    DEBUG_LOG("Reset complete - Player at (%.1f, %.1f)\n", respawnX, respawnY);
}

/**
 * Get first inactive projectile from pool
 */
Projectile* GamePlayState::getInactiveProjectile()
{
    for (auto& projectile : projectilePool) {
        if (!projectile.isActive()) {
            return &projectile;
        }
    }

    DEBUG_LOG("WARNING: Projectile pool exhausted!\n");
    return nullptr;  // Pool is full
}

/**
 * Spawn a new projectile (from pool)
 */
void GamePlayState::spawnProjectile(ProjectileType type, float x, float y,
                                   float velX, float velY, int damage)
{
    Projectile* proj = getInactiveProjectile();
    if (proj) {
        proj->spawn(type, x, y, velX, velY, damage, true);
        DEBUG_LOG("Spawned projectile at (%.1f, %.1f) with velocity (%.1f, %.1f)\n", x, y, velX, velY);
    }
}

/**
 * Update all active projectiles
 */
void GamePlayState::updateProjectiles(const InputState& input)
{
    for (auto& projectile : projectilePool) {
        if (projectile.isActive()) {
            projectile.update(input, level);
        }
    }
}

/**
 * Render all active projectiles
 */
void GamePlayState::renderProjectiles(float cameraX, float cameraY) const
{
    for (const auto& projectile : projectilePool) {
        if (projectile.isActive()) {
            projectile.render(cameraX, cameraY);
        }
    }
}
/**
 * Update tous les ennemis actifs
 */
void GamePlayState::updateEnemies(const InputState& input)
{
    for (auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->update(input, level);
        }
    }
}

/**
 * Render tous les ennemis vivants
 */
void GamePlayState::renderEnemies(float cameraX, float cameraY) const
{
    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->render(cameraX, cameraY);
        }
    }
}

/**
 * Collision projectiles vs enemies
 * Teste tous les projectiles actifs contre tous les enemies vivants
 * AABB (Axis-Aligned Bounding Box) collision
 */
void GamePlayState::checkProjectileEnemyCollisions()
{
    for (auto& projectile : projectilePool) {
        if (!projectile.isActive()) {
            continue;
        }

        // Ignorer les projectiles d'enemies (pour l'instant)
        // TODO Itération 4: Gérer collision projectiles enemies vs player
        if (!projectile.isActive()) {
            continue;
        }

        // Hitbox du projectile
        float projLeft = projectile.getX();
        float projRight = projLeft + projectile.getWidth();
        float projTop = projectile.getY();
        float projBottom = projTop + projectile.getHeight();

        // Tester contre chaque enemy vivant
        for (auto& enemy : enemies) {
            if (!enemy || !enemy->isAlive() || enemy->isInvincible()) {
                continue;
            }

            // Hitbox de l'enemy
            float enemyLeft = enemy->getX();
            float enemyRight = enemyLeft + enemy->getWidth();
            float enemyTop = enemy->getY();
            float enemyBottom = enemyTop + enemy->getHeight();

            // AABB collision test
            bool collision = (projRight > enemyLeft &&
                            projLeft < enemyRight &&
                            projBottom > enemyTop &&
                            projTop < enemyBottom);

            if (collision) {
                // Infliger dégâts à l'enemy
                enemy->takeDamage(projectile.getDamage());

                // Désactiver le projectile
                projectile.deactivate();

                DEBUG_LOG("Projectile hit enemy! Damage: %d\n", projectile.getDamage());
                break;  // Un projectile ne peut toucher qu'un seul enemy
            }
        }
    }
}
