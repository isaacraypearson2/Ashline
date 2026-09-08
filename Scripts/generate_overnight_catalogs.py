#!/usr/bin/env python3
"""Generate designer-mirror JSON from the C++ catalogs (runtime remains C++)."""

from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "Source" / "Ashline"
DATA = ROOT / "Content" / "Ashline" / "Data"

WEAPON_CPP = SRC / "Weapons" / "AshlineWeaponCatalog.cpp"
META_CPP = SRC / "Meta" / "AshlineMetaCatalog.cpp"
AI_CPP = SRC / "AI" / "AshlineAICatalog.cpp"
EQ_CPP = SRC / "Meta" / "AshlineEquipmentCatalog.cpp"


def ids_from(pattern: str, text: str) -> list[str]:
    return re.findall(pattern, text)


def main() -> None:
    weapons_src = WEAPON_CPP.read_text(encoding="utf-8")
    meta_src = META_CPP.read_text(encoding="utf-8")
    ai_src = AI_CPP.read_text(encoding="utf-8")
    eq_src = EQ_CPP.read_text(encoding="utf-8")

    weapon_ids = ids_from(r'W\(TEXT\("([^"]+)"\)', weapons_src)
    attachment_ids = ids_from(r'A\(TEXT\("([^"]+)"\)', weapons_src)
    cosmetic_calls = re.findall(
        r'C\(TEXT\("([^"]+)"\),\s*TEXT\("([^"]+)"\),\s*EAshlineCosmeticSlot::(\w+),\s*EAshlineLootRarity::(\w+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(true|false)',
        meta_src,
    )
    skin_calls = re.findall(
        r'S\(TEXT\("([^"]+)"\),\s*TEXT\("([^"]+)"\),\s*TEXT\("([^"]*)"\),\s*EAshlineLootRarity::(\w+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(true|false)',
        meta_src,
    )

    class_by_id = {}
    class_matches = re.findall(
        r'W\(TEXT\("([^"]+)"\),\s*TEXT\("([^"]+)"\),\s*EAshlineWeaponClass::(\w+)',
        weapons_src,
    )
    unlock_matches = re.findall(
        r'W\(TEXT\("([^"]+)"\),.*?,\s*(\d+),\s*\n\s*\{\}',
        weapons_src,
        flags=re.S,
    )
    # Simpler unlock: after class line the unlock int is the 5th numeric after S(
    weapon_defs = []
    for wid, name, cls in class_matches:
        class_by_id[wid] = (name, cls)
        weapon_defs.append({"id": wid, "name": name, "class": cls, "unlock": 1})

    # unlock levels from W( ... S(...), UNLOCK
    for m in re.finditer(
        r'W\(TEXT\("([^"]+)"\),.*?S\([^)]+\),\s*(\d+)',
        weapons_src,
        flags=re.S,
    ):
        wid, unlock = m.group(1), int(m.group(2))
        for row in weapon_defs:
            if row["id"] == wid:
                row["unlock"] = unlock
                break

    cosmetics = []
    for cid, name, slot, rarity, rank, cost, prestige, starter in cosmetic_calls:
        cosmetics.append(
            {
                "id": cid,
                "name": name,
                "slot": slot,
                "rarity": rarity,
                "rank": int(rank),
                "cost": int(cost),
                "prestige": int(prestige),
                "starter": starter == "true",
                "meshPath": f"/Game/Ashline/Characters/Hero/Cosmetics/SK_{cid}",
                "materialPath": f"/Game/Ashline/Materials/Cosmetics/M_{cid}",
            }
        )

    skins = []
    for sid, name, weapon, rarity, rank, cost, prestige, starter in skin_calls:
        skins.append(
            {
                "id": sid,
                "name": name,
                "weapon": weapon if weapon else "*",
                "rarity": rarity,
                "rank": int(rank),
                "cost": int(cost),
                "prestige": int(prestige),
                "starter": starter == "true",
                "materialPath": f"/Game/Ashline/Weapons/Materials/M_{sid}",
                "meshPath": f"/Game/Ashline/Weapons/Meshes/SM_{sid}",
            }
        )

    eq_calls = re.findall(
        r'E\(TEXT\("([^"]+)"\),\s*TEXT\("([^"]+)"\),\s*EAshlineEquipmentSlot::(\w+),\s*EAshlineLootRarity::(\w+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(true|false)',
        eq_src,
    )
    equipment = []
    for eid, name, slot, rarity, rank, cost, prestige, starter in eq_calls:
        equipment.append(
            {
                "id": eid,
                "name": name,
                "slot": slot,
                "rarity": rarity,
                "rank": int(rank),
                "cost": int(cost),
                "prestige": int(prestige),
                "starter": starter == "true",
                "meshPath": f"/Game/Ashline/Weapons/Equipment/SM_{eid}",
                "materialPath": f"/Game/Ashline/Weapons/Materials/M_{eid}",
            }
        )

    ai_names = re.findall(
        r'Add\(EAshlineAIArchetype::(\w+),\s*TEXT\("([^"]+)"\)',
        ai_src,
    )
    ai_slug = {
        "MachineGunner": "Gunner",
        "CivilianIrregular": "Irregular",
        "CQBSpecialist": "CQB",
    }
    ai_archetypes = []
    for enum_name, display in ai_names:
        slug = ai_slug.get(enum_name, enum_name)
        ai_archetypes.append(
            {
                "enum": enum_name,
                "name": display,
                "presentation": f"/Game/Ashline/Data/Kits/DA_AI_{slug}.DA_AI_{slug}",
                "mesh": f"/Game/Ashline/Characters/AI/SK_AI_{slug}.SK_AI_{slug}",
            }
        )

    weapons_json = {
        "note": "Designer mirror. Runtime source of truth is UAshlineWeaponCatalog.",
        "roster": weapon_defs,
        "attachmentSlots": ["Optic", "Muzzle", "Underbarrel", "Magazine", "Stock", "Laser", "Ammunition"],
        "classes": ["AssaultRifle", "SMG", "Sniper", "Shotgun", "Sidearm", "DMR", "LMG", "Launcher", "Melee", "BattleRifle", "PDW"],
        "attachments": attachment_ids,
        "gunfeel": {
            "recoilPatterns": "Per-class TArray<FVector2D> on FAshlineWeaponStats.RecoilPattern",
            "ads": "ADSTimeSeconds + ADSFov + hip/ADS spread + sway lerp",
            "fireModes": "Semi / Burst / Auto via B / D-pad right",
            "ammoTypes": ["FMJ", "AP", "HP", "Tracer", "Slug", "Buckshot", "HE", "Subsonic"],
            "falloff": "DamageAtRange(FalloffStart/End, MinDamageMul)",
            "reload": "EmptyReloadMul vs TacticalReloadMul",
        },
        "upgrade": {
            "maxTier": 5,
            "cost": "UAshlineWeaponCatalog::UpgradeCostForClass",
        },
    }

    meta_json = {
        "campaign": "Ashline",
        "note": "Designer mirror. Runtime source of truth is UAshlineMetaCatalog. Soft mesh/material refs are Phase 2 hooks — do not assume Fab binaries exist.",
        "saveSlot": "AshlineCampaign",
        "slotVersion": "2.0.0",
        "startingCredits": 750,
        "maxRank": 50,
        "rankCurve": {
            "xpToNext": "700 + Rank * 70 (0 at rank 50)",
            "creditGrant": "200 every 5 ranks, else 50",
            "crateGrant": "1 every 10 ranks",
            "rankUnlocks": {"5": "CAMO_WOODLAND", "10": "CAMO_NIGHT", "15": "HELM_FAST", "20": "VEST_HEAVY", "25": "SKIN_CARBON", "30": "CHARM_FLAG", "35": "SKIN_CHROME", "40": "CAMO_ASHLINE", "45": "CAMO_GHOST"},
        },
        "economy": {
            "missionCredits": "round((BaseXP * 0.45 + Stars * 80) * difficultyMul)",
            "difficultyMul": {"Recruit": 0.75, "Regular": 1.0, "Veteran": 1.25, "Extreme": 1.6},
            "weaponUpgradeCost": "class-scaled via UpgradeCostForClass (AR 250+300*tier … sniper/LMG/launcher 400+420*tier; melee 120+140*tier)",
            "maxUpgradeTier": 5,
            "weaponPurchase": "CreditCost==0 service guns auto-grant on rank. Paid guns: AshBuyWeapon after rank/prestige.",
            "equipment": "Lethals / tacticals / field. Starters EQ_FRAG + EQ_FLASH. AshBuyEquipment / AshEquipEquipment.",
        },
        "prestige": {
            "requiredRank": 50,
            "keeps": ["cosmetics", "skins", "crate history", "credits", "equipment"],
            "resets": ["rank", "xp", "non-starter attachments", "weapon upgrade tiers"],
            "grantsAndEquips": ["CAMO_PRESTIGE", "SKIN_GOLD", "CHARM_SPINE"],
            "prestige2": ["CAMO_SPECTRE", "CAMO_DIAMOND", "CHARM_DIAMOND", "SKIN_VOID", "SKIN_DIAMOND", "EQ_HEARTBEAT"],
        },
        "cosmetics": cosmetics,
        "skins": skins,
        "equipment": equipment,
        "crateTable": "Runtime rolls UAshlineMetaCatalog::CratePoolIds(rarity) — all non-starter, non-prestige items of that rarity.",
        "phase2Hooks": {
            "cosmeticMeshOverride": "FAshlineCosmeticDefinition.MeshOverride — auto-bound to /Game/Ashline/Characters/Hero/Cosmetics/SK_{Id}",
            "cosmeticMaterialOverride": "FAshlineCosmeticDefinition.MaterialOverride — auto-bound to /Game/Ashline/Materials/Cosmetics/M_{Id}",
            "skinMaterialOverride": "FAshlineWeaponSkinDefinition.MaterialOverride — auto-bound to /Game/Ashline/Weapons/Materials/M_{Id}",
            "skinMeshOverride": "FAshlineWeaponSkinDefinition.MeshOverride — auto-bound to /Game/Ashline/Weapons/Meshes/SM_{Id}",
            "dataAssets": "DA_COS_{Id} / DA_SKIN_{Id} overlay catalog paths when present",
            "bindings": "Content/Ashline/Data/ContentBindings.json",
            "umg": "Canvas HUD is playable. Operator locker / armory UMG is still optional.",
        },
    }

    ai_json = {
        "note": "Designer mirror. Runtime source of truth is UAshlineAICatalog.",
        "archetypes": ai_archetypes,
    }

    bindings = {
        "note": "Canonical soft-ref map. C++ UAshlineContentManifest + UAshlineMetaCatalog bind the same paths. Binaries are not in git — drop Fab/MetaHuman/Megascans onto these names and MeshOverride/MaterialOverride resolve.",
        "hero": {
            "body": "/Game/Ashline/Characters/Hero/SK_AshlineHero.SK_AshlineHero",
            "face": "/Game/Ashline/Characters/Hero/SK_AshlineHero_Face.SK_AshlineHero_Face",
            "presentation": "/Game/Ashline/Data/Kits/DA_Hero_Operator.DA_Hero_Operator",
            "masters": {
                "weapon": "/Game/Ashline/Materials/PBR/M_WeaponMaster.M_WeaponMaster",
                "skin": "/Game/Ashline/Materials/PBR/M_SkinMaster.M_SkinMaster",
                "character": "/Game/Ashline/Materials/PBR/M_CharacterMaster.M_CharacterMaster",
                "environment": "/Game/Ashline/Materials/PBR/M_EnvironmentMaster.M_EnvironmentMaster",
            },
        },
        "ai": {row["enum"] if row["enum"] != "MachineGunner" else "Gunner": row["mesh"] for row in ai_json["archetypes"]},
        "pathConventions": {
            "cosmeticSkeletal": "/Game/Ashline/Characters/Hero/Cosmetics/SK_{Id}.SK_{Id}",
            "cosmeticStatic": "/Game/Ashline/Characters/Hero/Parts/SM_{Id}.SM_{Id}",
            "cosmeticPart": "/Game/Ashline/Characters/Hero/Parts/{Slot}/SM_{Id}.SM_{Id}",
            "cosmeticMaterial": "/Game/Ashline/Materials/Cosmetics/M_{Id}.M_{Id}",
            "cosmeticDataAsset": "/Game/Ashline/Data/Kits/DA_COS_{Id}.DA_COS_{Id}",
            "skinMaterial": "/Game/Ashline/Weapons/Materials/M_{Id}.M_{Id}",
            "skinMesh": "/Game/Ashline/Weapons/Meshes/SM_{Id}.SM_{Id}",
            "skinDataAsset": "/Game/Ashline/Data/Kits/DA_SKIN_{Id}.DA_SKIN_{Id}",
            "weaponMesh": "/Game/Ashline/Weapons/Meshes/SM_{Id}.SM_{Id}",
            "weaponDataAsset": "/Game/Ashline/Data/Kits/DA_WPN_{Id}.DA_WPN_{Id}",
            "equipmentMesh": "/Game/Ashline/Weapons/Equipment/SM_{Id}.SM_{Id}",
            "equipmentMaterial": "/Game/Ashline/Weapons/Materials/M_{Id}.M_{Id}",
            "equipmentDataAsset": "/Game/Ashline/Data/Kits/DA_EQ_{Id}.DA_EQ_{Id}",
            "kitGround": "/Game/Ashline/Environments/{Slug}/M_Ground_{Slug}.M_Ground_{Slug}",
            "kitWall": "/Game/Ashline/Environments/{Slug}/M_Wall_{Slug}.M_Wall_{Slug}",
            "kitTrim": "/Game/Ashline/Environments/{Slug}/M_Trim_{Slug}.M_Trim_{Slug}",
            "kitDataAsset": "/Game/Ashline/Data/Kits/DA_Kit_ASH{NN}.DA_Kit_ASH{NN}",
        },
        "cosmetics": {},
        "skins": {},
        "weapons": [w["id"] for w in weapon_defs],
        "equipment": {},
        "kits": [
            "ASH01_WireCut",
            "ASH02_DustMarket",
            "ASH03_Holdfast",
            "ASH04_NightGlass",
            "ASH05_ConvoyGhost",
            "ASH06_AshHarbor",
            "ASH07_Whiteout",
            "ASH08_Catacomb",
            "ASH09_RidgeWire",
            "ASH10_FalseFlag",
            "ASH11_LastTrain",
            "ASH12_Ashline",
        ],
    }

    bindings["ai"] = {
        {
            "MachineGunner": "Gunner",
            "CivilianIrregular": "Irregular",
            "CQBSpecialist": "CQB",
        }.get(row["enum"], row["enum"]): row["mesh"]
        for row in ai_archetypes
    }

    for item in cosmetics:
        spec = {
            "slot": item["slot"],
            "mesh": f"/Game/Ashline/Characters/Hero/Cosmetics/SK_{item['id']}.SK_{item['id']}",
            "material": f"/Game/Ashline/Materials/Cosmetics/M_{item['id']}.M_{item['id']}",
        }
        if item["slot"] in {"Helmet", "Vest", "Pants", "Gloves", "Boots", "Face", "Headset", "Backpack"}:
            spec["part"] = f"/Game/Ashline/Characters/Hero/Parts/{item['slot']}/SM_{item['id']}.SM_{item['id']}"
        if item["slot"] == "Charm":
            spec["part"] = f"/Game/Ashline/Weapons/Charms/SM_{item['id']}.SM_{item['id']}"
        if item["slot"] == "Voice":
            spec["mesh"] = ""
            spec["material"] = ""
        bindings["cosmetics"][item["id"]] = spec

    for item in skins:
        spec = {"material": f"/Game/Ashline/Weapons/Materials/M_{item['id']}.M_{item['id']}"}
        if item["weapon"] != "*":
            spec["weapon"] = item["weapon"]
        bindings["skins"][item["id"]] = spec

    for item in equipment:
        bindings["equipment"][item["id"]] = {
            "slot": item["slot"],
            "mesh": f"/Game/Ashline/Weapons/Equipment/SM_{item['id']}.SM_{item['id']}",
            "material": f"/Game/Ashline/Weapons/Materials/M_{item['id']}.M_{item['id']}",
        }

    manifest = json.loads((DATA / "ContentManifest.json").read_text(encoding="utf-8"))
    manifest["weapons"] = [w["id"] for w in weapon_defs]
    manifest["masters"] = bindings["hero"]["masters"]
    manifest["note"] = "Binaries are not in git. Paths are resolved by UAshlineContentManifest. Cosmetic/skin overrides: ContentBindings.json."

    DATA.mkdir(parents=True, exist_ok=True)
    (DATA / "Weapons.json").write_text(json.dumps(weapons_json, indent=2) + "\n", encoding="utf-8")
    (DATA / "Meta.json").write_text(json.dumps(meta_json, indent=2) + "\n", encoding="utf-8")
    (DATA / "AI.json").write_text(json.dumps(ai_json, indent=2) + "\n", encoding="utf-8")
    (DATA / "Equipment.json").write_text(json.dumps({"note": "Designer mirror. Runtime source of truth is UAshlineEquipmentCatalog.", "roster": equipment}, indent=2) + "\n", encoding="utf-8")
    (DATA / "ContentBindings.json").write_text(json.dumps(bindings, indent=2) + "\n", encoding="utf-8")
    (DATA / "ContentManifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

    print(
        f"OK generated {len(weapon_defs)} weapons, {len(attachment_ids)} attachments, "
        f"{len(cosmetics)} cosmetics, {len(skins)} skins, {len(equipment)} equipment, {len(ai_names)} AI defs"
    )
    if not weapon_defs:
        raise SystemExit("failed to parse weapons")
    if len(cosmetics) < 20:
        raise SystemExit(f"too few cosmetics parsed: {len(cosmetics)}")
    if len(skins) < 20:
        raise SystemExit(f"too few skins parsed: {len(skins)}")


if __name__ == "__main__":
    main()
