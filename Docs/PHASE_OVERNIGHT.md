# Overnight production pass — armory + operator + materials

Branch `cursor/overnight-armory-production-fdb2` **merged** `cursor/movable-runtime-lights-5a13` (PR #8) so DeviceProfiles / AAA graybox / Movable lights are **not duplicated**. This pass deepens the SP armory, operator locker, gunfeel, and master-material helpers on top of that world.

**Single-player only. No ads.** Do not thrash `AshlineGrayboxBuilder` here — map scale lives on PR #8.

Runtime source of truth is C++ catalogs. Designer mirrors: `Content/Ashline/Data/{Weapons,Meta,AI,Equipment}.json`. Soft refs never assume Fab binaries exist (`AshlineLoad` + `LOAD_NoWarn | LOAD_Quiet`). Engine **and** `/Game/StarterContent/` are always tried quietly.

## What shipped

| Domain | In this pass |
| --- | --- |
| Weapons | Full class set: AR / SMG / SG / SR / DMR / LMG / pistol / launcher / melee / **battle rifle** / **PDW**. Recoil patterns, ADS time/FOV, hip vs ADS vs move vs crouch spread, empty vs tactical reload, falloff, sway, fire modes (Semi/Burst/Auto, **B** / D-pad right), ammo types (FMJ/AP/HP/Tracer/Slug/Buckshot/HE/**Subsonic**/**Match**). Per-weapon gunfeel on every roster id. Includes FAL-E, P90-C, M4-C, G36C, M16-A4, G3-A, MP7-A, M82-A, M9-A. |
| Attachments | Optic / muzzle / underbarrel / mag / stock / laser / ammunition with **zeroed stat deltas** (no accidental default ADS/pen bleed). Includes irons, NV, IR laser, folding stock, heavy brake, short mag, no-stock, HE warhead, **subsonic**, plus LPVO / Delta / linear comp / handstop / mag couple / CQB stock / NGAL / Match. |
| Skins | Universal + **2–3 uniques per gun** (incl. FAL wood/tan, P90-C NATO/frost, M82 sand/night, ASH-16 woodland). Soft mesh + MI. Prestige gilt / void / diamond. Chrome / neon / glacier use clearcoat + emissive on the skin master. |
| Operator | Helmet, vest, pants, gloves, boots, camo, face, voice, charm, **headset**, **backpack**. Collections per mission + prestige 1–2. Starter assault pack. Extra camos (M81 / A-TACS / tiger / UCP / flecktarn / Pencott / CADPAT), Ops-Core / PASGT, JPC / MBAV, RAC / Peltor. |
| Equipment | Lethals / tacticals / field (frag, flash starters). Locker buy/equip. Throwables are data-ready; authored FX later. |
| AI | 13 archetypes (incl. Grenadier / Elite / Spotter / **RadioOp** / **CQB**). Tints, armor mul, threat, grenade count, voice lines, `DA_AI_{Slug}`. **Spawn lists stay on the graybox from PR #8.** |
| Materials | Master helpers `M_WeaponMaster` / `M_SkinMaster` / `M_CharacterMaster` / `M_EnvironmentMaster` + class MIs `MI_WPN_{AR,SMG,SR,SG,Pistol,DMR,LMG,GL,Melee,BR,PDW}`. `FAshlineMaterialParams` now includes NormalStrength / AO / ClearCoat / UVTiling. Surface slugs + `MasterMaterialCandidates`. Quiet Engine/Starter fallbacks. |
| Graphics | **From PR #8** (do not reinvent): `AshPCUltra` (9070 GRE 1440p default), `AshPCHigh`, `AshPCBalanced`, `AshPCPerf` → `PC_Performance`, `AshSteamDeck` (800p / FSR / 30-40-60), `AshLaptop`, `AshGfxAuto`, **F8** / `AshGfxCycle`, `AshFPS`. Plus `AshPCLow` / `AshPCMed`. |
| Economy | Rank 1–50, class-scaled weapon upgrades (max 5), paid guns via `AshBuyWeapon`, attachments via `AshBuyAttachment`, crates roll cosmetics **or** skins **or** equipment. Prestige 1 gilt; prestige 2 diamond + heartbeat sensor. |

Starters remain **ASH-16 + M17-A**, **CAMO_FIELD**, **SKIN_FACTORY**, **EQ_FRAG + EQ_FLASH**, **PACK_ASSAULT**. Prestige 1 still grants `CAMO_PRESTIGE`, `SKIN_GOLD`, `CHARM_SPINE`.

Catalog floors (this pass): **45 weapons, 64 attachments, 153 cosmetics, 140 skins, 16 equipment, 13 AI**.

## Fab to install next (Windows `X3D_AMD`)

Nothing below is in git. Exact names: **`Docs/PHASE2_FAB.md`**. Soft refs already point at `ContentBindings.json`.

1. Starter Content (immediate PBR).
2. MetaHuman hero + 2–3 AI LODs; GASP / TP mannequin interim (`Scripts/assign_interim_meshes.py`).
3. Weapon pack → `SM_WPN_*` under `Weapons/Meshes/` (ids in `Weapons.json`). Launchers + melee + FAL / P90-C included.
4. Skin MIs → `Weapons/Materials/M_{SkinId}`.
5. Clothing parts → `Characters/Hero/Parts/{Slot}/SM_{Id}` (Headset + Backpack folders exist).
6. Megascans kits per ASH-01…12. Leave `bOverrideMood` unchecked.
7. Niagara muzzle / equipment FX; MetaSounds into `Audio/Weapons/`.
8. Author `M_WeaponMaster` (and the other three masters) plus class MIs `MI_WPN_AR` / `MI_WPN_SMG` / … so MIDs stop tinting Engine default. Drop `MI_SkinMaster` / `MI_CharacterMaster` when ready.

Missing packs **must not crash PIE**. If something 404s, search the bold name on Fab.

## Morning checklist (Isaac)

- [ ] Pull this branch, compile `AshlineEditor` Win64 Development on UE **5.8.2**.
- [ ] PIE campaign select. Confirm HUD: Rank / Credits / Lethal / Tactical / graphics preset.
- [ ] In ASH-01: fire, ADS, **B** cycles fire mode on ASH-16 (AUTO→BURST→SEMI). Reload hip vs empty.
- [ ] `AshPCUltra` / `AshPCHigh` / `AshPCPerf` / `AshSteamDeck` / `AshLaptop` / **F8** — confirm CVars, no fatal.
- [ ] `AshGrantCredits 5000` → `AshBuySkin SKIN_FDE` → `AshEquipSkin WPN_AR_ASH16 SKIN_FDE` → redeploy, gun tints.
- [ ] `AshBuyWeapon WPN_BR_FAL` (rank 13+) / `AshBuyWeapon WPN_PDW_P90C`.
- [ ] `AshEquipCosmetic Headset HEAD_COMTAC` / `AshEquipCosmetic Backpack PACK_ASSAULT`.
- [ ] `AshSetRank 50` → `AshPrestige` → gilt + prestige camo + spine charm. Second prestige: diamond set.
- [ ] `AshListArmory` / `AshListMeta` dump ids. Graybox/DeviceProfiles came from PR #8 — do not rewrite them on this branch.
- [ ] Ads still off. No listen-server / netcode objects.

Validators (this VM, no UE):

```
python3 Scripts/generate_overnight_catalogs.py
python3 Scripts/validate_meta_catalog.py
python3 Scripts/validate_content_layout.py
```

## Honesty

Fab / MetaHuman / Megascans / authored Niagara / UMG locker screens are **not** in this repo. The playable path is Canvas HUD + C++ catalogs + Engine/Starter fallbacks. That is enough to ship a 1–2 week vertical if packs land on the Windows box.
