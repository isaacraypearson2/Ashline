# Overnight production pass — armory + operator + materials

Shipped on branch `cursor/overnight-armory-production-fdb2` off `cursor/aaa-content-art-pass-f576`. **Single-player only. No ads. `AshlineGrayboxBuilder` was not touched** (map scale lives on the other PR).

Runtime source of truth is C++ catalogs. Designer mirrors: `Content/Ashline/Data/{Weapons,Meta,AI,Equipment}.json`. Soft refs never assume Fab binaries exist (`LOAD_NoWarn | LOAD_Quiet`, Engine/StarterContent always tried).

## What shipped

| Domain | In this pass |
| --- | --- |
| Weapons | Full class set: AR / SMG / SG / SR / DMR / LMG / pistol / **launcher** / **melee**. Recoil patterns, ADS time/FOV, hip vs ADS vs move vs crouch spread, empty vs tactical reload, falloff, sway, fire modes (Semi/Burst/Auto, **B** / D-pad right), ammo types (FMJ/AP/HP/Tracer/Slug/Buckshot/HE). |
| Attachments | Optic / muzzle / underbarrel / mag / stock / laser / ammunition, including irons, heavy brake, short mag, no-stock, HE warhead. |
| Skins | Universal + per-gun collections. Soft mesh + MI. Prestige gilt / void / diamond. |
| Operator | Helmet, vest, pants, gloves, boots, camo, face, voice, charm. Collections per mission + prestige 1–2. |
| Equipment | Lethals / tacticals / field (frag, flash starters). Locker buy/equip. Throwables are data-ready; authored FX later. |
| AI | 11 archetypes (incl. Grenadier / Elite / Spotter). Tints, armor mul, threat, grenade count, voice lines, `DA_AI_{Slug}` presentation ids. **Spawn lists in GrayboxBuilder unchanged.** |
| Materials | Master helpers `M_WeaponMaster` / `M_SkinMaster` / `M_CharacterMaster` / `M_EnvironmentMaster` + `FAshlineMaterialParams` (roughness/metallic/emissive). |
| Graphics | `AshPCUltra` (9070 GRE 1440p default), `AshPCBalanced`, `AshPCPerf`, `AshSteamDeck` (1280×800 / 60 / RT off), `AshPCLow` / `AshPCMed`. Skin cache CVars. |
| Economy | Rank 1–50, class-scaled weapon upgrades (max 5), paid guns via `AshBuyWeapon`, attachments via `AshBuyAttachment`, crates roll cosmetics **or** skins **or** equipment. Prestige 1 gilt; prestige 2 diamond + heartbeat sensor. |

Starters remain **ASH-16 + M17-A**, **CAMO_FIELD**, **SKIN_FACTORY**, **EQ_FRAG + EQ_FLASH**. Prestige 1 still grants `CAMO_PRESTIGE`, `SKIN_GOLD`, `CHARM_SPINE`.

## Fab to install next (Windows `X3D_AMD`)

Nothing below is in git. Exact names: **`Docs/PHASE2_FAB.md`**. Soft refs already point at `ContentBindings.json`.

1. Starter Content (immediate PBR).
2. MetaHuman hero + 2–3 AI LODs; GASP / TP mannequin interim (`Scripts/assign_interim_meshes.py`).
3. Weapon pack → `SM_WPN_*` under `Weapons/Meshes/` (36 ids). Launchers + melee included.
4. Skin MIs → `Weapons/Materials/M_{SkinId}`.
5. Clothing parts → `Characters/Hero/Parts/{Slot}/SM_{Id}`.
6. Megascans kits per ASH-01…12. Leave `bOverrideMood` unchecked.
7. Niagara muzzle / equipment FX; MetaSounds into `Audio/Weapons/`.
8. Author `M_WeaponMaster` (and the other three masters) so MIDs stop tinting Engine default.

Missing packs **must not crash PIE**. If something 404s, search the bold name on Fab.

## Morning checklist (Isaac)

- [ ] Pull this branch, compile `AshlineEditor` Win64 Development on UE **5.8.2**.
- [ ] PIE campaign select. Confirm HUD: Rank / Credits / Lethal / Tactical / graphics preset.
- [ ] In ASH-01: fire, ADS, **B** cycles fire mode on ASH-16 (AUTO→BURST→SEMI). Reload hip vs empty.
- [ ] `AshPCUltra` / `AshPCPerf` / `AshSteamDeck` — confirm CVars, no fatal.
- [ ] `AshGrantCredits 5000` → `AshBuySkin SKIN_FDE` → `AshEquipSkin WPN_AR_ASH16 SKIN_FDE` → redeploy, gun tints.
- [ ] `AshBuyWeapon WPN_AR_M4K` (rank 3+) then `AshEquip` via loadout / next mission grant path.
- [ ] `AshSetRank 50` → `AshPrestige` → gilt + prestige camo + spine charm. Second prestige: diamond set.
- [ ] `AshListArmory` / `AshListMeta` dump ids. No GrayboxBuilder conflict with the map-scale PR.
- [ ] Ads still off. No listen-server / netcode objects.

Validators (this VM, no UE):

```
python3 Scripts/generate_overnight_catalogs.py
python3 Scripts/validate_meta_catalog.py
python3 Scripts/validate_content_layout.py
```

## Honesty

Fab / MetaHuman / Megascans / authored Niagara / UMG locker screens are **not** in this repo. The playable path is Canvas HUD + C++ catalogs + Engine/Starter fallbacks. That is enough to ship a 1–2 week vertical if packs land on the Windows box.
