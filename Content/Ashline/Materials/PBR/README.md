# PBR masters

Drop authored Nanite/Lumen-friendly masters here when you have them. **Nothing in this folder is a binary in git.**

```
M_AshlineConcrete / MI_AshlineConcrete
M_AshlineMetal    / MI_AshlineMetal
M_AshlineDirt     / MI_AshlineDirt
M_AshlineFoliage  / MI_AshlineFoliage
M_AshlineGlass    / MI_AshlineGlass
M_AshlineAsphalt  / MI_AshlineAsphalt
M_AshlineSkin     / MI_AshlineSkin
M_AshlineWood, M_AshlineSnow, M_AshlineWater, M_AshlineEmissive, M_AshlinePlastic
```

Runtime (`GetSurfaceMaterial`) tries `MI_Ashline{Slug}` then `M_Ashline{Slug}`, then Engine `BasicShapeMaterial` / `WorldGridMaterial` / `DefaultMaterial`, then Starter Content **only if that pack exists**. Missing paths fail quiet (`AshlineLoad`).
