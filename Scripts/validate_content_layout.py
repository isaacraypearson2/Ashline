#!/usr/bin/env python3
"""Validate Phase 2 content folder layout + ContentBindings vs Meta.json."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONTENT = ROOT / "Content" / "Ashline"
BINDINGS = CONTENT / "Data" / "ContentBindings.json"
META = CONTENT / "Data" / "Meta.json"
MANIFEST = CONTENT / "Data" / "ContentManifest.json"
MAT_BINDINGS = CONTENT / "Data" / "MaterialBindings.json"
FAB_PLAN = CONTENT / "Data" / "FabInstallPlan.json"

REQUIRED_DIRS = [
    "Characters/Hero",
    "Characters/Hero/Cosmetics",
    "Characters/Hero/Parts/Helmet",
    "Characters/Hero/Parts/Vest",
    "Characters/Hero/Parts/Pants",
    "Characters/Hero/Parts/Gloves",
    "Characters/Hero/Parts/Boots",
    "Characters/Hero/Parts/Face",
    "Characters/Hero/Parts/Charm",
    "Characters/AI",
    "Characters/MetaHuman",
    "Weapons/Meshes",
    "Weapons/Materials",
    "Weapons/Charms",
    "Materials/Cosmetics",
    "Materials/PBR",
    "Materials/PBR/Masters",
    "Materials/PBR/Instances",
    "Materials/Decals",
    "Materials/Glass",
    "Materials/Skin",
    "Materials/Libraries",
    "Environments/Shared",
    "Data/Kits",
    "FX/Muzzle",
    "Audio/Weapons",
]

KITS = [
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
]


def fail(msg: str) -> None:
    print(f"FAIL: {msg}")
    raise SystemExit(1)


def main() -> None:
    missing = [d for d in REQUIRED_DIRS if not (CONTENT / d).is_dir()]
    if missing:
        fail(f"missing folders {missing}")

    for slug in KITS:
        if not (CONTENT / "Environments" / slug).is_dir():
            fail(f"missing environment kit folder {slug}")

    bindings = json.loads(BINDINGS.read_text(encoding="utf-8"))
    meta = json.loads(META.read_text(encoding="utf-8"))
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    mat_bindings = json.loads(MAT_BINDINGS.read_text(encoding="utf-8"))
    fab_plan = json.loads(FAB_PLAN.read_text(encoding="utf-8"))

    cosmetic_ids = {c["id"] for c in meta["cosmetics"]}
    skin_ids = {s["id"] for s in meta["skins"]}
    bound_c = set(bindings["cosmetics"])
    bound_s = set(bindings["skins"])
    if cosmetic_ids != bound_c:
        fail(f"cosmetic id mismatch meta vs ContentBindings: {sorted(cosmetic_ids ^ bound_c)}")
    if skin_ids != bound_s:
        fail(f"skin id mismatch meta vs ContentBindings: {sorted(skin_ids ^ bound_s)}")

    if bindings["kits"] != manifest["kits"]:
        fail("kit list mismatch ContentBindings vs ContentManifest")
    if bindings["weapons"] != manifest["weapons"]:
        fail("weapon list mismatch ContentBindings vs ContentManifest")

    for item_id, spec in bindings["cosmetics"].items():
        slot = spec.get("slot")
        if slot in {"Voice"}:
            continue
        material = spec.get("material") or ""
        if material and f"M_{item_id}" not in material:
            fail(f"cosmetic material path must contain M_{item_id}: {material}")
        if spec.get("mesh") and f"SK_{item_id}" not in spec["mesh"]:
            fail(f"cosmetic mesh path must contain SK_{item_id}: {spec['mesh']}")

    for item_id, spec in bindings["skins"].items():
        material = spec["material"]
        if f"M_{item_id}" not in material:
            fail(f"skin material path must contain M_{item_id}: {material}")

    required_masters = {"Environment", "Weapon", "Character", "Decal", "Glass", "Skin"}
    if set(mat_bindings.get("masters", {})) != required_masters:
        fail(f"MaterialBindings masters mismatch: {sorted(mat_bindings.get('masters', {}))}")
    for key, path in mat_bindings["masters"].items():
        if "M_" not in path or "Master" not in path:
            fail(f"master path must be a Master asset: {key} {path}")
    if "eveningHours" not in fab_plan or len(fab_plan["eveningHours"]) < 4:
        fail("FabInstallPlan eveningHours incomplete")
    for name in ("Ultra", "Balanced", "SteamDeck"):
        if name not in fab_plan.get("streaming", {}):
            fail(f"FabInstallPlan missing streaming.{name}")

    print(
        f"OK: layout + bindings ({len(cosmetic_ids)} cosmetics, "
        f"{len(skin_ids)} skins, {len(KITS)} kits, "
        f"{len(mat_bindings['masters'])} masters)"
    )


if __name__ == "__main__":
    sys.exit(main())
