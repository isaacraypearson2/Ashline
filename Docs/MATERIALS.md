# Ashline master materials — parameter contract

Runtime factory: `UAshlineMaterialFactory`. Presentation helpers: `UAshlinePresentationLibrary::MakeTexturedMaterial` / `MakeWeaponMaterial` / `MakeCharacterMaterial` / `MakeSkinMaterial` / `MakeGlassMaterial`.

**Soft refs match PR #8 / `UAshlineContentManifest::MasterMaterialCandidates`.** Do not invent `M_Env_Master` or `MI_Env_*` as primary resolve paths.

```
/Game/Ashline/Materials/PBR/MI_Ashline{Slug}.MI_Ashline{Slug}
/Game/Ashline/Materials/PBR/M_Ashline{Slug}.M_Ashline{Slug}
/Game/Ashline/Materials/PBR/M_{Slug}.M_{Slug}
```

`SurfaceSlug`: Ground/Sand/Dirt → **Dirt**. Then Concrete, Metal, Wood, Snow, Water, Foliage, Emissive, Plastic, Glass, Asphalt, Skin.

**Missing Quixel/Fab never breaks PIE or Shipping.** Empty texture slots stamp Engine fallbacks (`DefaultTexture`, `DefaultNormal`, `WhiteSquareTexture`, `Black`). Loads go through `AshlineLoad` (quiet).

## Master graphs (create in-editor)

Run **Tools → Execute Python Script → `Scripts/create_master_materials.py`** after the first editor open (`import_fab_kits.py` calls it).

| Master | Path | Used for |
| --- | --- | --- |
| Environment | `/Game/Ashline/Materials/PBR/M_AshlineMaster` | Parent for dirt/concrete/metal/wood/snow/foliage/emissive/asphalt/plastic |
| Weapon | `/Game/Ashline/Materials/PBR/M_AshlineWeapon` | Guns, charms, attachments |
| Character | `/Game/Ashline/Materials/PBR/M_AshlineCharacter` | Clothing, camos, AI uniforms |
| Skin | `/Game/Ashline/Materials/PBR/M_AshlineSkin` | Faces, MetaHuman body SSS (`MI_AshlineSkin`) |
| Glass | `/Game/Ashline/Materials/PBR/M_AshlineGlass` | Windows, Night Glass, harbor (`MI_AshlineGlass`) |
| Decal | `/Game/Ashline/Materials/PBR/M_AshlineDecal` | Bullet / dirt / signage |

Surface instances live next to the masters: `MI_AshlineConcrete`, `MI_AshlineMetal`, `MI_AshlineDirt`, `MI_AshlineGlass`, `MI_AshlineSkin`, … JSON map: `Content/Ashline/Data/MaterialBindings.json`.

## Texture parameter names (stamp every alias)

`StampTextureSet` writes **all** of these so Megascans, Fab military packs, and Engine materials all bind.

| Slot | Parameter aliases | Engine fallback |
| --- | --- | --- |
| Base color | `BaseColor`, `Base Color`, `Albedo`, `Diffuse`, `Color`, `BaseColorMap` | `/Engine/EngineResources/DefaultTexture` |
| Normal | `Normal`, `NormalMap`, `Normalmap` | `/Engine/EngineResources/DefaultNormal` |
| Packed ORM | `ORM`, `ARM`, `Mask`, `OcclusionRoughnessMetallic` | DefaultTexture (neutral) |
| Roughness | `Roughness`, `RoughnessMap`, `RoughnessValue` | scalar default per surface |
| Metallic | `Metallic`, `MetallicMap`, `MetallicValue` | scalar default per surface |
| AO | `AmbientOcclusion`, `AO`, `Occlusion` | white |
| Emissive | `Emissive`, `EmissiveMap`, `EmissiveColor`, `EmissiveStrength` | Black |
| Height | `Height`, `Displacement`, `HeightMap` | Black |
| Opacity | `Opacity`, `OpacityMask`, `OpacityValue` | white / 1.0 |
| UV | `Tiling`, `UVScale`, `TextureScale` | 1–2.5 by surface |
| Glass | `IOR` (1.52), `Opacity` (0.28) | — |
| Skin | `Subsurface`, `SubsurfaceColor` | — |

Megascans packed maps are **ORM** (R=AO, G=Roughness, B=Metallic). Some Fab weapon packs use **ARM**. Both names are stamped.

## Runtime resolve order (surfaces)

1. Mission kit soft ptr (`DA_Kit_ASH##` Ground/Wall/Trim/Foliage/**Glass**/Decal)
2. Canonical kit file `M_Ground_<Slug>` / `M_Wall_*`
3. Authored `MI_Ashline{Slug}` then `M_Ashline{Slug}` then `M_{Slug}`
4. Master graph + texture set (Fab maps if assigned, else Engine)
5. Starter Content (`M_Ground_Grass`, `M_Concrete_Tiles`, `M_Metal_Steel`, …) **only if the pack exists**
6. Engine `DefaultMaterial` / `WorldGridMaterial` / `BasicShapeMaterial`

Weapons: locker `MaterialOverride` → `DA_WPN_*.SkinMaterial` / `TextureSet` → `M_AshlineWeapon` → metal Engine tint.

Characters: camo `MaterialOverride` → `DA_Hero_Operator.BodyMaterialOverride` / `SkinMaterial` → `M_AshlineCharacter` / `M_AshlineSkin` → Engine tint.

## After Fab import (no C++ change)

1. Duplicate or rename the Megascans material onto `MI_Ashline{Slug}` **or** assign the soft ptr on `DA_Kit_ASH##`.
2. If the pack uses different parameter names, create an MI of `M_AshlineMaster` and plug the textures into the aliases above.
3. PIE. Graybox layout numbers are unchanged; only the material chain upgrades.

Do **not** commit Quixel/Fab/MetaHuman binaries unless the license allows **and** you intend Git LFS.
