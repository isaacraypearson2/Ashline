# Characters

Canonical drop paths (see `UAshlineContentManifest`):

```
Hero/SK_AshlineHero
Hero/SK_AshlineHero_Face
MetaHuman/          ← Bridge / Creator export
AI/SK_AI_Rifleman
AI/SK_AI_Breacher
AI/SK_AI_Marksman
AI/SK_AI_Gunner
AI/SK_AI_Officer
AI/SK_AI_Scout
AI/SK_AI_Heavy
AI/SK_AI_Irregular
```

Until those exist, runtime uses GASP/TP mannequin paths if migrated, else a **humanoid blockout** (head/torso/limbs). Capsules are hidden. Clothing parts: `Hero/Parts/{Slot}/SM_{Id}`. Import: `Docs/PHASE2_FAB.md`.
