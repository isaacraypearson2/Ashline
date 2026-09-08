# Weapons

Drop Fab / Lyra meshes here as `Meshes/SM_{WeaponId}` for every id in `Content/Ashline/Data/Weapons.json` (AR/SMG/SG/SR/DMR/LMG/pistol/launcher/melee/battle-rifle/PDW).

Skin materials: `Materials/M_{SkinId}`. Equipment: `Equipment/SM_{EquipmentId}`.

Or assign soft refs on `DA_WPN_*` / `DA_EQ_*` in `/Game/Ashline/Data/Kits/`.

In-repo fallback: compound receiver/barrel/stock/mag built in `UAshlineWeaponComponent` (class-specific proportions). Muzzle light + impact decals already fire. Missing Fab never crashes.
