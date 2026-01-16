# Diagrammes: boss-transition

## 1. Architecture des Composants

```mermaid
graph TB
    subgraph "Feature: boss-transition"
        BTS[BossTransitionState]
        BTD[Detection Trigger]
    end

    subgraph "Systemes Existants"
        GPS[GamePlayState]
        SM[StateManager]
        P[Player]
        CAM[Camera]
        LM[LevelManager]
        CZ[CameraZone]
    end

    GPS -->|detecte trigger| BTD
    BTD -->|condition remplie| SM
    SM -->|push| BTS
    BTS -->|bloque inputs| P
    BTS -->|teleporte| P
    BTS -->|verrouille| CAM
    BTS -->|query zone boss| LM
    LM -->|fournit| CZ
    BTS -->|pop apres fin| SM
```

## 2. State Machine - BossTransitionState

```mermaid
stateDiagram-v2
    [*] --> FADE_IN: push state

    FADE_IN --> TELEPORT: timer >= 120
    note right of FADE_IN
        alpha: 0 -> 255
        inputs: disabled
        duration: 120 frames
    end note

    TELEPORT --> PAUSE: instantane
    note right of TELEPORT
        player.x = bossZone.x
        camera.x = bossZone.x
        duration: 1 frame
    end note

    PAUSE --> FADE_OUT: timer >= 50
    note right of PAUSE
        ecran noir
        duration: 50 frames
    end note

    FADE_OUT --> DONE: timer >= 120
    note right of FADE_OUT
        alpha: 255 -> 0
        duration: 120 frames
    end note

    DONE --> [*]: pop state
    note right of DONE
        inputs: enabled
        camera: locked on boss zone
    end note
```

## 3. Sequence d'Execution

```mermaid
sequenceDiagram
    participant Input
    participant GPS as GamePlayState
    participant SM as StateManager
    participant BTS as BossTransitionState
    participant P as Player
    participant CAM as Camera
    participant LM as LevelManager

    Input->>GPS: right key pressed
    GPS->>GPS: checkBossTransitionTrigger()
    GPS->>LM: getBossZone()
    LM-->>GPS: CameraZone (last zone)
    GPS->>GPS: isPlayerAtZoneEdge()

    alt trigger conditions met
        GPS->>SM: push(BossTransitionState)
        SM->>BTS: enter()

        loop FADE_IN (120 frames)
            BTS->>BTS: alpha += 255/120
            BTS->>P: block inputs
        end

        BTS->>LM: getBossZone()
        LM-->>BTS: bossZone
        BTS->>P: setPosition(bossZone.x, y)
        BTS->>CAM: setPosition(bossZone.x, bossZone.y)
        BTS->>CAM: lockToBossZone()

        loop PAUSE (50 frames)
            BTS->>BTS: wait
        end

        loop FADE_OUT (120 frames)
            BTS->>BTS: alpha -= 255/120
        end

        BTS->>P: enable inputs
        BTS->>SM: requestPop()
        SM->>GPS: resume()
    end
```

## 4. Detection du Trigger

```mermaid
flowchart TD
    A[Player update] --> B{Zone actuelle?}
    B -->|Zone N-1| C{Input droite?}
    B -->|Autre zone| Z[Pas de transition]
    C -->|Non| Z
    C -->|Oui| D{Position X?}
    D -->|Pas au bord| Z
    D -->|Au bord droit| E{Zone boss existe?}
    E -->|Non| Z
    E -->|Oui| F[Declenchement BossTransitionState]

    style F fill:#f96
```

## 5. Layout des Zones

```mermaid
graph LR
    subgraph "Niveau"
        Z0[Zone 0] --> Z1[Zone 1]
        Z1 --> Z2[Zone 2]
        Z2 --> ZN1["Zone N-1<br/>(avant-derniere)"]
        ZN1 -->|"TRIGGER<br/>ici"| ZN["Zone N<br/>(BOSS)<br/>next_left=-1"]
    end

    style ZN1 fill:#ff9
    style ZN fill:#f66
```

## 6. Timeline de la Transition

```mermaid
gantt
    title Timeline Boss Transition (290 frames total)
    dateFormat X
    axisFormat %s

    section Inputs
    Disabled           :active, 0, 290

    section Visual
    Fade In (0->noir)  :crit, 0, 120
    Noir complet       :120, 170
    Fade Out (noir->0) :crit, 170, 290

    section Position
    Zone N-1           :0, 120
    Teleport           :milestone, 120, 120
    Zone Boss          :120, 290

    section Camera
    Suivant joueur     :0, 120
    Lock boss zone     :120, 290
```
