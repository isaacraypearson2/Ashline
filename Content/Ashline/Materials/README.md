# Materials

```
PBR/       MI_Ashline{Slug} / M_Ashline{Slug} / M_{Slug}  (create_master_materials.py)
PBR/       M_AshlineMaster / M_AshlineWeapon / M_AshlineCharacter / M_AshlineGlass / M_AshlineSkin
Libraries/ optional Fab material packs
Decals/    M_Impact_Bullet
Cosmetics/ M_{CosmeticId}   ← locker MaterialOverride
```

Until those `.uasset` files exist, the builder tints Engine `DefaultMaterial` / `WorldGridMaterial` / `BasicShapeMaterial` and stamps Engine fallback textures. Starter Content is used **only if the pack is installed** — missing paths fail quiet.

Contract: `Docs/MATERIALS.md`. JSON: `Content/Ashline/Data/MaterialBindings.json`.
