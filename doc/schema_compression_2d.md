# SYSTÈME DE COMPRESSION 2D - METATILES

## Vue d'ensemble

Système de compression pour maps 2D utilisant des blocs 2×2 (metatiles) pré-définis.

---

## 1. TILESET (Bibliothèque de base)

```
ID:  0    1    2    3    4    5    6    7    8    9   10  ...
   ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐
   │  │ │██│ │▒▒│ │╔═│ │═╗│ │║ │ │░░│ │▓▓│ │╚═│ │═╝│ │──│
   └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘
 (Vide)(Mur)(Sol)(Coin)...                         (20 tiles)
```

**Taille :** 20 tiles maximum (5 bits par ID)

---

## 2. dataBlock (Blocs 2×2 pré-définis)

Chaque bloc contient 4 IDs de tiles, affichés dans l'ordre :
```
[0][1]  ← gauche à droite
[2][3]  ← haut en bas
```

### Exemples de blocs :

**dataBlock[0] = [0, 0, 0, 0]** - Bloc vide
```
┌──┬──┐
│ 0│ 0│
├──┼──┤
│ 0│ 0│
└──┴──┘
```

**dataBlock[1] = [3, 4, 1, 1]** - Coin supérieur
```
┌──┬──┐
│ 3│ 4│  (╔═ et ═╗)
├──┼──┤
│ 1│ 1│  (██ et ██)
└──┴──┘
```

**dataBlock[2] = [2, 2, 5, 5]** - Sol plat
```
┌──┬──┐
│ 2│ 2│  (▒▒ et ▒▒)
├──┼──┤
│ 5│ 5│  (║  et ║ )
└──┴──┘
```

**dataBlock[3] = [1, 1, 8, 9]** - Coin inférieur
```
┌──┬──┐
│ 1│ 1│  (██ et ██)
├──┼──┤
│ 8│ 9│  (╚═ et ═╝)
└──┴──┘
```

**Recommandation :** Définir 8-32 blocs selon les besoins

---

## 3. dataMap (Référence les blocs à afficher)

Pour une map **20×12 tiles** → **10 blocs large × 6 blocs haut = 60 IDs**

### Exemple de dataMap :

```
dataMap = [
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    ← Ligne 1 (ciel)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    ← Ligne 2 (ciel)
  0, 0, 1, 2, 2, 2, 3, 0, 0, 0,    ← Ligne 3 (plateforme)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    ← Ligne 4 (vide)
  1, 2, 2, 2, 2, 2, 2, 2, 2, 3,    ← Ligne 5 (sol)
  1, 2, 2, 2, 2, 2, 2, 2, 2, 3     ← Ligne 6 (sol)
]
```

Chaque valeur référence un bloc de dataBlock.

---

## 4. MAP FINALE RENDUE (20×12 tiles)

Chaque ID de dataMap se transforme en bloc 2×2 :

```
┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┐
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ Ligne 1-2
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤ Ciel (bloc 0)
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ Ligne 3-4
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤ Ciel
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤
│  │  │╔═│══│══│══│═╗│  │  │  │  │  │  │  │  │  │  │  │  │  │ Ligne 5-6
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤ Plateforme
│  │  │██│▒▒│▒▒│▒▒│██│  │  │  │  │  │  │  │  │  │  │  │  │  │ (blocs 1,2,2,2,3)
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ Ligne 7-8
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤ Vide
│  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤
│╔═│══│══│══│══│══│══│══│══│═╗│  │  │  │  │  │  │  │  │  │  │ Ligne 9-10
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤ Sol
│██│▒▒│▒▒│▒▒│▒▒│▒▒│▒▒│▒▒│▒▒│██│  │  │  │  │  │  │  │  │  │  │ (1,2,2,2,2,2,2,2,2,3)
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤
│╔═│══│══│══│══│══│══│══│══│═╗│  │  │  │  │  │  │  │  │  │  │ Ligne 11-12
├──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┼──┤ Sol
│██│▒▒│▒▒│▒▒│▒▒│▒▒│▒▒│▒▒│▒▒│██│  │  │  │  │  │  │  │  │  │  │
└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┘
```

---

## 5. ALGORITHME DE DÉCOMPRESSION

Pour afficher une tile à la position **(x, y)** :

### Étape 1 : Calculer l'index du bloc
```
blockX = x / 2
blockY = y / 2
blockIndex = blockY × blocksPerRow + blockX
```

### Étape 2 : Récupérer l'ID du bloc
```
blockID = dataMap[blockIndex]
```

### Étape 3 : Calculer la position dans le bloc
```
tileInBlockX = x % 2
tileInBlockY = y % 2
tileIndex = tileInBlockY × 2 + tileInBlockX
```

### Étape 4 : Récupérer la tile finale
```
tileID = dataBlock[blockID][tileIndex]
```

### Étape 5 : Afficher la tile
```
renderTile(tileID, x, y)
```

### Exemple concret

Position **(4, 10)** dans la map :

1. `blockX = 4/2 = 2`, `blockY = 10/2 = 5`
2. `blockIndex = 5 × 10 + 2 = 52`
3. `blockID = dataMap[52] = 2` (bloc "sol plat")
4. `tileInBlockX = 4%2 = 0`, `tileInBlockY = 10%2 = 0`
5. `tileIndex = 0×2 + 0 = 0`
6. `tileID = dataBlock[2][0] = 2` (tile "sol")
7. Afficher tile 2 à (4, 10)

---

## 6. TAUX DE COMPRESSION

### Map 20×12 = 240 tiles

#### SANS COMPRESSION :
```
240 tiles × 5 bits/tile = 1200 bits (150 bytes)
```

#### AVEC COMPRESSION (16 blocs dans dataBlock) :
```
- dataBlock : 16 blocs × 4 tiles × 5 bits = 320 bits
- dataMap   : 60 positions × 4 bits        = 240 bits
- TOTAL     : 560 bits (70 bytes)

→ ÉCONOMIE : 53% ! 💾
```

#### AVEC 32 BLOCS :
```
- dataBlock : 32 blocs × 4 tiles × 5 bits = 640 bits
- dataMap   : 60 positions × 5 bits       = 300 bits
- TOTAL     : 940 bits (117.5 bytes)

→ ÉCONOMIE : 22% 💾
```

---

## 7. OPTIMISATIONS POSSIBLES

### A. Blocs par zone
Diviser la map en régions, chaque région ayant son propre dataBlock (8-16 blocs/région).

### B. Flags de transformation
Ajouter des flags pour réutiliser les blocs :
```
dataMap[i] = {blockID: 5, flipX: true, flipY: false}
```
Permet de créer 4× plus de variations avec le même nombre de blocs !

### C. Blocs spéciaux
Réserver quelques IDs pour des tiles individuelles (détails, objets uniques).

### D. RLE (Run-Length Encoding)
Compresser les séquences répétées :
```
Au lieu de : [0,0,0,0,0,0,0,0]
Utiliser   : {blockID: 0, repeat: 8}
```

---

## 8. AVANTAGES DU SYSTÈME

✅ **Compression efficace** : 50-80% d'économie mémoire  
✅ **Décompression rapide** : Quelques opérations simples  
✅ **Édition facile** : Modifier un bloc met à jour toute la map  
✅ **Cohérence visuelle** : Patterns réutilisables garantissent l'homogénéité  
✅ **Typique des jeux NES/SNES** : Mega Man, Metroid, Castlevania, etc.

---

## 9. LIMITATIONS

⚠️ **Résolution divisée par 2** : 20×12 devient effectivement 10×6  
⚠️ **Variations limitées** : Nombre de blocs limité (8-64)  
⚠️ **Détails perdus** : Difficile pour décors complexes/uniques  

---

## 10. RECOMMANDATIONS

| Nombre de blocs | Usage recommandé |
|----------------|------------------|
| 8-16 blocs     | Niveaux simples, répétitifs (ex: zones souterraines) |
| 16-32 blocs    | Équilibre idéal flexibilité/compression |
| 32-64 blocs    | Niveaux variés, détaillés |
| 64+ blocs      | Perte de compression, envisager autre système |

---

## Structure de données (exemple en C)

```c
// Tileset (20 tiles max)
typedef struct {
    uint8_t tileData[16][16];  // 16x16 pixels par tile
} Tile;

Tile tileset[20];

// DataBlock (max 64 blocs avec 6 bits)
typedef struct {
    uint8_t tiles[4];  // 4 IDs de tiles (0-19)
} Block;

Block dataBlock[64];

// DataMap (60 positions pour map 20x12)
uint8_t dataMap[60];  // Chaque valeur = ID de bloc (0-63)

// Fonction de décompression
uint8_t getTileAt(int x, int y) {
    int blockX = x / 2;
    int blockY = y / 2;
    int blockIndex = blockY * 10 + blockX;
    
    uint8_t blockID = dataMap[blockIndex];
    
    int tileInBlockX = x % 2;
    int tileInBlockY = y % 2;
    int tileIndex = tileInBlockY * 2 + tileInBlockX;
    
    return dataBlock[blockID].tiles[tileIndex];
}
```

---

**Créé pour projet Megaman-style platformer**  
**Résolution virtuelle : 320×180 (20×12 tiles de 16×16)**


# Décompression

A chaque scrolling on décompressera une rangée de 2x2 blocs verticalement
