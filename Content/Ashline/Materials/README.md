# Materials

```
PBR/Masters/     M_Env_Master, M_Weapon_Master, M_Character_Master
PBR/Instances/   MI_Env_* surface instances
Glass/           M_Glass_Master, MI_Glass_Clear
Skin/            M_Skin_Master, MI_Skin_Operator
Decals/          M_Decal_Master, M_Impact_Bullet
Libraries/       Fab material packs (not in git)
Cosmetics/       M_{CosmeticId}   ← locker MaterialOverride
```

Until then the builder stamps masters / Engine `DefaultMaterial` / Starter Content with Engine fallback textures (`DefaultTexture`, `DefaultNormal`). Missing Quixel never breaks PIE. See `Docs/MATERIALS.md`.
