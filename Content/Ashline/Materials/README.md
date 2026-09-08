# Materials

```
PBR/     drop MI_Ashline* / M_Ashline* masters when authored (not in git)
Decals/M_Impact_Bullet
Cosmetics/M_{CosmeticId}   ← locker MaterialOverride
```

Until those `.uasset` files exist, the builder tints Engine `DefaultMaterial` / `WorldGridMaterial` / `BasicShapeMaterial`. Starter Content (`M_Ground_Grass`, `M_Concrete_Tiles`, `M_Metal_Steel`, `M_Water_Ocean`, `M_Glass`, `M_Rock_Marble`) is used **only if the pack is installed** — missing paths fail quiet.
