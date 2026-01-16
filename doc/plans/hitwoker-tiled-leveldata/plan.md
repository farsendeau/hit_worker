# Plan d'Implementation: hitwoker-tiled-leveldata

**Date**: 2026-01-15
**Status**: Planning
**Emplacement outil**: ~/hit_woker_tiled/main.cpp

---

## 1. Resume de l'Approche

Modifier l'outil hitwoker_tiled existant pour:
1. Accepter de nouveaux arguments CLI (--level-id, --next-level, --tileset)
2. Generer le code dans un namespace au lieu de globales
3. Creer une instance LevelData avec designated initializers
4. Supprimer les elements redondants (struct CameraZone, fonctions inline)

## 2. Fichiers a Modifier

| Fichier | Modifications |
|---------|---------------|
| `~/hit_woker_tiled/main.cpp` | Ajout parsing args, modification generation |

## 3. Structures de Donnees

### 3.1 Nouvelles variables pour les arguments

```cpp
struct GeneratorConfig {
    std::string inputFile;
    int levelId = 1;
    int nextLevelId = -1;
    std::string tilesetPath = "asset/level/tileset/1.jpg";
};
```

## 4. Taches d'Implementation

### Phase A: Parsing des Arguments

#### Tache 1: Ajouter le parsing des arguments CLI
- **Description**: Parser --level-id, --next-level, --tileset
- **Fichier**: ~/hit_woker_tiled/main.cpp
- **Dependances**: Aucune
- **Validation**: Les arguments sont correctement extraits

**Implementation:**
```cpp
GeneratorConfig parseArgs(int argc, char* argv[]) {
    GeneratorConfig config;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg.starts_with("--level-id=")) {
            config.levelId = std::stoi(arg.substr(11));
        } else if (arg.starts_with("--next-level=")) {
            config.nextLevelId = std::stoi(arg.substr(13));
        } else if (arg.starts_with("--tileset=")) {
            config.tilesetPath = arg.substr(10);
        } else if (!arg.starts_with("-")) {
            config.inputFile = arg;
        }
    }

    return config;
}
```

### Phase B: Modification de la Generation

#### Tache 2: Modifier le header guard et includes
- **Description**: Header guard dynamique LEVEL{N}_DATA_H, include LevelData.hpp
- **Fichier**: ~/hit_woker_tiled/main.cpp (fonction main, lignes ~740-750)
- **Dependances**: Tache 1
- **Validation**: Le header genere commence correctement

**Avant:**
```cpp
std::cout << "#ifndef LEVEL_DATA_H" << std::endl;
std::cout << "#define LEVEL_DATA_H" << std::endl;
std::cout << "#include <cstdint>" << std::endl;
```

**Apres:**
```cpp
std::cout << "#ifndef LEVEL" << config.levelId << "_DATA_H" << std::endl;
std::cout << "#define LEVEL" << config.levelId << "_DATA_H" << std::endl;
std::cout << "\n#include \"LevelData.hpp\"" << std::endl;
```

#### Tache 3: Ajouter le namespace
- **Description**: Wrapper tout le contenu dans namespace LevelN
- **Fichier**: ~/hit_woker_tiled/main.cpp
- **Dependances**: Tache 2
- **Validation**: Les donnees sont dans le namespace

**Ajouter apres les includes:**
```cpp
std::cout << "\nnamespace Level" << config.levelId << " {\n" << std::endl;
```

**Ajouter avant le #endif:**
```cpp
std::cout << "\n} // namespace Level" << config.levelId << std::endl;
```

#### Tache 4: Supprimer la struct CameraZone de la generation
- **Description**: Ne plus generer la struct CameraZone (deja dans LevelData.hpp)
- **Fichier**: ~/hit_woker_tiled/main.cpp (fonction printCameraZones, lignes ~485-491)
- **Dependances**: Tache 3
- **Validation**: Pas de struct CameraZone dans la sortie

**Supprimer ces lignes dans printCameraZones:**
```cpp
std::cout << "struct CameraZone {\n";
std::cout << "    int zone_id;\n";
// ...
std::cout << "};\n\n";
```

#### Tache 5: Supprimer les fonctions inline
- **Description**: Ne plus generer getVisualTileAt, isSolidAt, etc.
- **Fichier**: ~/hit_woker_tiled/main.cpp (fonction printUsageFunctions)
- **Dependances**: Tache 3
- **Validation**: Pas de fonctions inline dans la sortie

**Modifier:** Supprimer l'appel a `compressor.printUsageFunctions()` dans main()

#### Tache 6: Supprimer la fonction findCameraZone
- **Description**: Ne plus generer findCameraZone (deja dans LevelData.cpp)
- **Fichier**: ~/hit_woker_tiled/main.cpp (dans printCameraZones, lignes ~519-529)
- **Dependances**: Tache 4
- **Validation**: Pas de findCameraZone dans la sortie

#### Tache 7: Ajouter la generation de l'instance LevelData
- **Description**: Generer `inline const LevelData data {...}`
- **Fichier**: ~/hit_woker_tiled/main.cpp (nouvelle fonction ou dans main)
- **Dependances**: Taches 2-6
- **Validation**: Instance LevelData correctement initialisee

**Nouvelle fonction:**
```cpp
void printLevelDataInstance(const GeneratorConfig& config, const MultiLayerCompressor& compressor) {
    std::cout << "\n// ========================================" << std::endl;
    std::cout << "// LEVELDATA INSTANCE" << std::endl;
    std::cout << "// ========================================" << std::endl;

    std::cout << "\ninline const LevelData data {" << std::endl;
    std::cout << "    .mapWidthTiles = MAP_WIDTH_TILES," << std::endl;
    std::cout << "    .mapHeightTiles = MAP_HEIGHT_TILES," << std::endl;
    std::cout << "    .blocksWide = MAP_WIDTH_TILES / 2," << std::endl;
    std::cout << "    .blocksHigh = MAP_HEIGHT_TILES / 2," << std::endl;
    std::cout << "    .dataBlockVisual = dataBlockVisual," << std::endl;
    std::cout << "    .dataMapVisual = dataMapVisual," << std::endl;
    std::cout << "    .numBlocks = " << compressor.getNumBlocks() << "," << std::endl;
    // ... autres champs ...
    std::cout << "    .levelId = " << config.levelId << "," << std::endl;
    std::cout << "    .tilesetPath = \"" << config.tilesetPath << "\"," << std::endl;
    std::cout << "    .nextLevelId = " << config.nextLevelId << std::endl;
    std::cout << "};" << std::endl;
}
```

### Phase C: Accesseurs pour MultiLayerCompressor

#### Tache 8: Ajouter des getters a MultiLayerCompressor
- **Description**: Exposer les compteurs pour l'instance LevelData
- **Fichier**: ~/hit_woker_tiled/main.cpp (classe MultiLayerCompressor)
- **Dependances**: Aucune
- **Validation**: Getters fonctionnels

**Ajouter a la classe:**
```cpp
int getNumBlocks() const { return dataBlockVisual.size(); }
int getNumSolidTiles() const { return uniqueSolidTiles.size(); }
int getNumLadderTiles() const { return uniqueLadderTiles.size(); }
int getNumKillTiles() const { return uniqueKillTiles.size(); }
int getNumCameraZones() const { return numCameraZones_; }
uint8_t getMaxTileId() const { return maxTileId_; }
```

### Phase D: Tests et Validation

#### Tache 9: Recompiler l'outil
- **Description**: Compiler hitwoker_tiled avec les modifications
- **Commande**: `cd ~/hit_woker_tiled && cmake --build build && cp build/main hitwoker_tiled`
- **Dependances**: Taches 1-8
- **Validation**: Compilation sans erreur

#### Tache 10: Tester la generation
- **Description**: Generer level1Data.h et comparer avec la version manuelle
- **Commande**: `./hitwoker_tiled /path/to/map_test.tmx --level-id=1 --next-level=2 --tileset="asset/level/tileset/1.jpg"`
- **Dependances**: Tache 9
- **Validation**: Sortie identique ou equivalente a level1Data.h

#### Tache 11: Verifier la compilation du projet
- **Description**: S'assurer que le fichier genere compile dans hit_worker
- **Commande**: `cd /home/karigane/hit_worker && cmake --build build`
- **Dependances**: Tache 10
- **Validation**: Compilation sans erreur

## 5. Diagramme de Dependances

```
Tache 1 (Args parsing)
    |
    v
Tache 8 (Getters) ----+
    |                 |
    v                 |
Tache 2 (Header) -----+
    |                 |
    v                 |
Tache 3 (Namespace)   |
    |                 |
    +---> Tache 4 (Suppr CameraZone struct)
    |         |
    |         v
    |     Tache 6 (Suppr findCameraZone)
    |
    +---> Tache 5 (Suppr fonctions inline)
    |
    v
Tache 7 (LevelData instance) <--- (depend de toutes les precedentes)
    |
    v
Tache 9 (Recompiler)
    |
    v
Tache 10 (Tester generation)
    |
    v
Tache 11 (Verifier compilation projet)
```

## 6. Tests et Validation

### Compilation
- [ ] hitwoker_tiled compile sans erreurs
- [ ] hitwoker_tiled compile sans warnings

### Tests manuels
- [ ] `--level-id=1` genere namespace Level1
- [ ] `--level-id=2` genere namespace Level2
- [ ] `--next-level=3` met nextLevelId = 3
- [ ] `--tileset="custom/path.jpg"` met tilesetPath correct
- [ ] Header guard correspond au level-id
- [ ] Pas de struct CameraZone dans la sortie
- [ ] Pas de fonctions inline dans la sortie
- [ ] Instance LevelData presente avec designated initializers

### Test d'integration
- [ ] Fichier genere compile dans hit_worker
- [ ] LevelManager peut acceder a LevelN::data

## 7. Risques et Mitigations

| Risque | Probabilite | Impact | Mitigation |
|--------|-------------|--------|------------|
| Syntaxe designated initializers | Faible | Moyen | Verifier C++20 supporte |
| Oubli d'un champ LevelData | Moyenne | Eleve | Comparer avec level1Data.h |
| Chemin tileset avec espaces | Faible | Faible | Echapper les guillemets |

## 8. Documentation a Mettre a Jour

- [ ] doc/guide/hitwoker_tiled.md - Documenter les nouveaux arguments
