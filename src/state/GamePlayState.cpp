#include "state/GamePlayState.hpp"

GamePlayState::GamePlayState()
{
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

    DEBUG_LOG("GamePlayState initialized\n");
    DEBUG_LOG("Level: %d\n", currentLevel);
    DEBUG_LOG("Player starting in zone: %d (x=%.0f, y=%.0f)\n", currentZoneId, startZone.x, startZone.y);
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

    // 6. Limiter le joueur aux bords de la zone
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

    // Dessiner le joueur
    player.render(camera.getX(), camera.getY());

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
    // Trouver la zone réelle du joueur basée sur sa position
    int actualZoneId = findCameraZone(player.getCenterX(), player.getCenterY());

    // Si pas de changement de zone, rien à faire
    if (actualZoneId == currentZoneId || actualZoneId < 0) {
        return;
    }

    const CameraZone& currentZone = cameraZones[currentZoneId];
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