#!/usr/bin/env python3
"""Validate Content/Ashline/Data/Meta.json against the SP meta contract."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
META = ROOT / "Content" / "Ashline" / "Data" / "Meta.json"

REQUIRED_COSMETICS = {
    "CAMO_FIELD",
    "CAMO_PRESTIGE",
    "HELM_PATROL",
    "VEST_PLATE",
    "PANT_FATIGUE",
    "GLOVE_NOMEX",
    "BOOT_COMBAT",
    "FACE_00",
    "VOICE_NEUTRAL",
    "CHARM_SPINE",
}
REQUIRED_SKINS = {
    "SKIN_FACTORY",
    "SKIN_GOLD",
    "SKIN_ASH16_NIGHT",
    "SKIN_M17_SIDE",
    "SKIN_FDE",
    "SKIN_CHROME",
    "SKIN_VOID",
    "SKIN_DIAMOND",
}
SLOTS = {"Helmet", "Vest", "Pants", "Gloves", "Boots", "Camo", "Face", "Voice", "Charm", "Headset", "Backpack"}


def fail(msg: str) -> None:
    print(f"FAIL: {msg}")
    raise SystemExit(1)


def main() -> None:
    data = json.loads(META.read_text(encoding="utf-8"))
    if data.get("startingCredits") != 750:
        fail("startingCredits must be 750")
    if data.get("maxRank") != 50:
        fail("maxRank must be 50")
    if data.get("slotVersion") != "2.0.0":
        fail("slotVersion must be 2.0.0")

    cosmetics = data["cosmetics"]
    skins = data["skins"]
    cosmetic_ids = [c["id"] for c in cosmetics]
    skin_ids = [s["id"] for s in skins]
    if len(cosmetic_ids) != len(set(cosmetic_ids)):
        fail("duplicate cosmetic id")
    if len(skin_ids) != len(set(skin_ids)):
        fail("duplicate skin id")
    missing_c = REQUIRED_COSMETICS - set(cosmetic_ids)
    missing_s = REQUIRED_SKINS - set(skin_ids)
    if missing_c:
        fail(f"missing cosmetics {sorted(missing_c)}")
    if missing_s:
        fail(f"missing skins {sorted(missing_s)}")

    starters_c = {c["id"] for c in cosmetics if c.get("starter")}
    starters_s = {s["id"] for s in skins if s.get("starter")}
    if "CAMO_FIELD" not in starters_c or "SKIN_FACTORY" not in starters_s:
        fail("starters must include CAMO_FIELD and SKIN_FACTORY")
    if "CAMO_PRESTIGE" in starters_c or "SKIN_GOLD" in starters_s:
        fail("prestige rewards must not be starters")

    for item in cosmetics:
        if item["slot"] not in SLOTS:
            fail(f"bad slot {item['slot']} on {item['id']}")
        if item.get("prestige", 0) < 0 or item.get("cost", 0) < 0:
            fail(f"bad economy on {item['id']}")

    weapons = {s["weapon"] for s in skins}
    if "*" not in weapons:
        fail("need at least one universal skin")
    for expected in (
        "WPN_AR_ASH16",
        "WPN_AR_M4K",
        "WPN_SMG_C9",
        "WPN_SMG_VEC",
        "WPN_SNP_G28L",
        "WPN_SNP_AWM",
        "WPN_SHG_M870K",
        "WPN_SHG_AA12",
        "WPN_PIS_M17A",
        "WPN_PIS_DEAG",
        "WPN_DMR_SASS",
        "WPN_DMR_MK14",
        "WPN_LMG_M250",
        "WPN_LMG_PKM",
        "WPN_AR_AUG",
        "WPN_LCH_RPG",
        "WPN_MEL_KNIFE",
        "WPN_BR_FAL",
        "WPN_PDW_P90C",
        "WPN_AR_M4C",
        "WPN_SNP_M82",
    ):
        if expected not in weapons:
            fail(f"missing per-weapon skin for {expected}")

    prestige = data["prestige"]
    grants = set(prestige.get("grantsAndEquips", []))
    if not {"CAMO_PRESTIGE", "SKIN_GOLD", "CHARM_SPINE"} <= grants:
        fail("prestige must grant camo, gold skin, spine charm")

    extra_clothing = {"HELM_BOONIE", "VEST_RECON", "PANT_CRYE", "GLOVE_WINTER", "BOOT_DESERT", "HELM_NVG", "VEST_RAID", "CAMO_URBAN", "CAMO_DIAMOND", "CHARM_DIAMOND", "HEAD_COMTAC", "PACK_ASSAULT"}
    if not extra_clothing <= set(cosmetic_ids):
        fail(f"missing Phase 2 clothing {sorted(extra_clothing - set(cosmetic_ids))}")

    print(f"OK: {len(cosmetics)} cosmetics, {len(skins)} skins, rank 1–{data['maxRank']}")
    if len(cosmetics) < 110:
        fail(f"expected a deep operator locker, got {len(cosmetics)} cosmetics")
    if len(skins) < 90:
        fail(f"expected a large skin catalog, got {len(skins)} skins")
    equipment = data.get("equipment") or []
    if len(equipment) < 12:
        fail(f"expected lethals/tacticals/field gear, got {len(equipment)} equipment")
    eq_ids = {e["id"] for e in equipment}
    if not {"EQ_FRAG", "EQ_FLASH"} <= eq_ids:
        fail("starters must include EQ_FRAG and EQ_FLASH")


if __name__ == "__main__":
    sys.exit(main())
