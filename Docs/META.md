# Ashline single-player meta

Call of Duty / Battlefield-style **campaign locker**: ranks, credits, operator clothing, weapon skins, prestige. **No multiplayer. No netcode.**

Runtime source of truth: `UAshlineMetaCatalog`  
Designer mirror: `Content/Ashline/Data/Meta.json`  
Save: slot `AshlineCampaign`, version **2.0.0** (`UAshlineSaveGame::MigrateIfNeeded` upgrades 1.x)

## What is real now vs Phase 2 art

| System | In this repo | Phase 2 on `X3D_AMD` |
| --- | --- | --- |
| Credits / mission payout / upgrade costs | Saved, awarded, spent | UMG storefront |
| Rank 1–50 curve + rank grants | `GrantXP` uses the table | Rank-up fanfare |
| Operator slots (helmet, vest, pants, gloves, boots, camo, face, voice, charm) | Owned + equipped in save | MetaHuman wardrobe meshes |
| Weapon skins (universal + per-gun) | Owned + equipped; tint applied to FPS gun | Fab material / unique meshes |
| Prestige at rank 50 | Resets rank, keeps locker, grants gilt + prestige camo + spine charm | Prestige emblem art |
| Play-earned crates | Tokens → rarity roll → cosmetic / skin / equipment id | Crate VFX |
| Equipment (lethal / tactical / field) | Owned + equipped in save | Niagara throwables |
| Soft mesh / material overrides | Auto-bound to `ContentBindings.json` paths; load if the file exists | Drop Fab / MetaHuman onto those names |

Canvas HUD remains the playable frontend. That is not a missing meta — the data path is live.

## Economy

- New operator starts with **750** credits and starter cosmetics / `SKIN_FACTORY`.
- Mission complete: `round((BaseXP * 0.45 + Stars * 80) * difficultyMul)` credits, plus XP and crate tokens.
- Difficulty multipliers: Recruit 0.75 / Regular 1.0 / Veteran 1.25 / Extreme 1.6.
- Rank-up: `XPToNext = 700 + Rank * 70`. Every 5 ranks +200 credits; every 10 ranks +1 crate. Rank 10 / 20 / 40 grant camo / vest / terminus camo.
- Weapon upgrade (tiers 0→5): **class-scaled** (`UAshlineWeaponCatalog::UpgradeCostForClass`). AR ≈ `250 + tier*300`; sniper/LMG/launcher higher; melee cheaper.
- **Service guns** (`CreditCost == 0`) auto-grant when rank ≥ unlock. **Paid guns** (`AshBuyWeapon`) after rank/prestige.
- Attachments: `AshBuyAttachment WPN_AR_ASH16 MAG_60`.
- Equipment: starters `EQ_FRAG` / `EQ_FLASH`. Buy/equip: `AshBuyEquipment` / `AshEquipEquipment Lethal EQ_SEMTEX`.

## Prestige

`PrestigeReset()` requires rank 50.

Keeps: owned cosmetics, skins, crate history, credits, equipment.  
Resets: rank, XP, upgrade tiers, non-starter attachments.  
Prestige 1 grants and equips: `CAMO_PRESTIGE`, `SKIN_GOLD` (all owned guns), `CHARM_SPINE`.  
Prestige 2+ also grants: `CAMO_SPECTRE`, `CAMO_DIAMOND`, `CHARM_DIAMOND`, `SKIN_VOID`, `SKIN_DIAMOND`, `EQ_HEARTBEAT`.

## Runtime apply

- `AAshlineCharacter::ApplyOperatorLook` tints the hero mesh or blockout parts from equipped slot ids, applies camo Mesh/Material overrides, and attaches clothing parts when `SM_{Id}` exists.
- `UAshlineWeaponComponent` stores `SkinId` on the runtime weapon and paints receiver / barrel / stock / mag.
- Equipped charm is a small mesh on the gun (placeholder until a Phase 2 charm mesh is assigned).

## Console (Output Log or `~`)

| Command | Effect |
| --- | --- |
| `AshGrantCredits 5000` | Add credits |
| `AshSetRank 50` | Rank cheat (needed before prestige) |
| `AshPrestige` | Prestige if rank 50 |
| `AshOpenCrate` | Spend one crate token |
| `AshBuySkin SKIN_FDE` | Purchase if rank / credits allow |
| `AshEquipSkin WPN_AR_ASH16 SKIN_FDE` | Equip owned skin |
| `AshBuyCosmetic CAMO_NIGHT` | Purchase cosmetic |
| `AshEquipCosmetic Camo CAMO_NIGHT` | Equip owned slot item |
| `AshBuyWeapon WPN_AR_M4K` | Purchase if rank / credits allow |
| `AshBuyAttachment WPN_AR_ASH16 MAG_60` | Unlock attachment on owned gun |
| `AshBuyEquipment EQ_SEMTEX` | Purchase lethal / tactical / field |
| `AshEquipEquipment Lethal EQ_SEMTEX` | Equip owned equipment slot |
| `AshUnlockMeta` | Own the catalog (prestige-gated items still need prestige) |
| `AshListMeta` | Dump cosmetic / skin ids |
| `AshListArmory` | Dump weapons + equipment |

Redeploy a mission after equipping so the pawn rebuilds visuals.

## Ads / multiplayer

Ads stay off. Do not add netcode here.
