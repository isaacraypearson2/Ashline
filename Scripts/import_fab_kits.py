"""
Ashline editor utility — Phase 2 kit scaffolding (UE 5.8.2).

Does NOT download Fab / Quixel / MetaHuman. It creates the Content folder
tree and empty DataAsset stubs so you can drop in licensed packs.

  Tools → Execute Python Script → Scripts/import_fab_kits.py
  py "Scripts/import_fab_kits.py"
"""

import json
import unreal
from pathlib import Path


KIT_ROOT = "/Game/Ashline/Data/Kits"
FOLDERS = [
    "/Game/Ashline/Characters/Hero",
    "/Game/Ashline/Characters/Hero/Cosmetics",
    "/Game/Ashline/Characters/Hero/Materials",
    "/Game/Ashline/Characters/Hero/Parts/Helmet",
    "/Game/Ashline/Characters/Hero/Parts/Vest",
    "/Game/Ashline/Characters/Hero/Parts/Pants",
    "/Game/Ashline/Characters/Hero/Parts/Gloves",
    "/Game/Ashline/Characters/Hero/Parts/Boots",
    "/Game/Ashline/Characters/Hero/Parts/Face",
    "/Game/Ashline/Characters/Hero/Parts/Charm",
    "/Game/Ashline/Characters/AI",
    "/Game/Ashline/Characters/MetaHuman",
    "/Game/Ashline/Weapons/Meshes",
    "/Game/Ashline/Weapons/Attachments",
    "/Game/Ashline/Weapons/Materials",
    "/Game/Ashline/Weapons/Charms",
    "/Game/Ashline/Weapons/Equipment",
    "/Game/Ashline/Environments/Shared",
    "/Game/Ashline/Materials/PBR",
    "/Game/Ashline/Materials/Decals",
    "/Game/Ashline/Materials/Cosmetics",
    "/Game/Ashline/Audio/Weapons",
    "/Game/Ashline/Audio/Footsteps",
    "/Game/Ashline/Audio/Music",
    "/Game/Ashline/Audio/Foley",
    "/Game/Ashline/FX/Muzzle",
    "/Game/Ashline/FX/Impacts",
    "/Game/Ashline/UI",
    KIT_ROOT,
]

MISSIONS = [
    (1, "WireCut", "Night dirt / concrete / chainlink / flood lights"),
    (2, "DustMarket", "Sand, adobe, market stalls, fabric"),
    (3, "Holdfast", "Sandbags, dirt berm, desert grass"),
    (4, "NightGlass", "Kiln brick, steel tower, night glass"),
    (5, "ConvoyGhost", "Asphalt, tanker metal, roadside rock"),
    (6, "AshHarbor", "Dock plates, crates, water, crane"),
    (7, "Whiteout", "Snow, pine, ice, beacon metal"),
    (8, "Catacomb", "Brick tunnel, pipes, server racks"),
    (9, "RidgeWire", "Cliff rock, alpine grass, antenna"),
    (10, "FalseFlag", "Campus concrete, glass, night signage"),
    (11, "LastTrain", "Rail, gravel, freight cars"),
    (12, "Ashline", "Brutalist concrete, emissive red trim"),
]

WEAPONS = [
    "WPN_AR_ASH16",
    "WPN_AR_M4K",
    "WPN_AR_AK74",
    "WPN_AR_SCARH",
    "WPN_AR_416C",
    "WPN_SMG_C9",
    "WPN_SMG_MPX",
    "WPN_SMG_VEC",
    "WPN_SMG_P90",
    "WPN_SHG_M870K",
    "WPN_SHG_M1014",
    "WPN_SHG_AA12",
    "WPN_SNP_G28L",
    "WPN_SNP_M2010",
    "WPN_SNP_AWM",
    "WPN_DMR_SASS",
    "WPN_DMR_MK14",
    "WPN_DMR_SVD",
    "WPN_LMG_M250",
    "WPN_LMG_M240",
    "WPN_LMG_PKM",
    "WPN_PIS_M17A",
    "WPN_PIS_G19",
    "WPN_PIS_DEAG",
    "WPN_PIS_MP443",
    "WPN_AR_AUG",
    "WPN_AR_FAMAS",
    "WPN_SMG_UMP",
    "WPN_SMG_UZI",
    "WPN_SHG_SPAS",
    "WPN_PIS_1911",
    "WPN_PIS_REV",
    "WPN_LCH_RPG",
    "WPN_LCH_AT4",
    "WPN_MEL_KNIFE",
    "WPN_MEL_TOMA",
]

AI_ARCHETYPES = (
    "Rifleman",
    "Breacher",
    "Marksman",
    "Gunner",
    "Officer",
    "Scout",
    "Heavy",
    "Irregular",
    "Grenadier",
    "Elite",
    "Spotter",
)

COSMETICS = [
    "CAMO_FIELD",
    "CAMO_NIGHT",
    "CAMO_DUST",
    "CAMO_WHITEOUT",
    "CAMO_ASHLINE",
    "CAMO_PRESTIGE",
    "HELM_PATROL",
    "HELM_FAST",
    "HELM_BOONIE",
    "VEST_PLATE",
    "VEST_HEAVY",
    "VEST_RECON",
    "PANT_FATIGUE",
    "PANT_CRYE",
    "GLOVE_NOMEX",
    "GLOVE_WINTER",
    "BOOT_COMBAT",
    "BOOT_DESERT",
    "FACE_00",
    "FACE_01",
    "CHARM_WIRE",
    "CHARM_SPINE",
]

SKINS = [
    "SKIN_FACTORY",
    "SKIN_FDE",
    "SKIN_OD",
    "SKIN_SNOW",
    "SKIN_GOLD",
    "SKIN_ASH16_NIGHT",
    "SKIN_C9_DUST",
    "SKIN_G28_GLASS",
    "SKIN_M870_BREACH",
    "SKIN_M17_SIDE",
    "SKIN_SASS_MARK",
    "SKIN_M250_SAW",
]

FAB_LIST = """
Ashline Fab / Bridge shopping list (license these yourself — not in git)
----------------------------------------------------------------------
Characters : MetaHuman Creator body + face → /Game/Ashline/Characters/MetaHuman/
Weapons    : modular military rifle / SMG / sniper / shotgun / pistol / DMR / LMG
ASH-01     : Megascans dirt, concrete wall, chainlink, flood light
ASH-02     : sand ground, adobe, market props
ASH-03     : sandbags, desert ground, grass clumps
ASH-04     : brick kiln, painted steel, night lights
ASH-05     : asphalt, tanker, roadside rock
ASH-06     : metal plate, crate, ocean water, crane
ASH-07     : snow, pine, ice
ASH-08     : brick, pipe, server rack
ASH-09     : cliff rock, mountain grass, lattice tower
ASH-10     : urban concrete, glass, signage
ASH-11     : rail, gravel, train car
ASH-12     : brutalist concrete, emissive trim
Starter    : Add Content Pack → Starter Content (immediate PBR upgrade)
FSR3       : AMD FidelityFX Super Resolution 3 plugin (optional; TSR works without it)
"""


def ensure_dir(path):
    unreal.EditorAssetLibrary.make_directory(path)


def create_data_asset(name, directory, class_path):
    asset_path = f"{directory}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log(f"Ashline: exists {asset_path}")
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    try:
        cls = unreal.load_class(None, class_path)
        if not cls:
            # Fallback: Primary Data Asset factory if the C++ class is not loaded yet.
            factory = unreal.DataAssetFactory()
            factory.set_editor_property("data_asset_class", unreal.PrimaryDataAsset)
            tools = unreal.AssetToolsHelpers.get_asset_tools()
            asset = tools.create_asset(name, directory, unreal.PrimaryDataAsset, factory)
            if asset:
                unreal.EditorAssetLibrary.save_loaded_asset(asset)
            unreal.log_warning(f"Ashline: created generic PrimaryDataAsset {asset_path} (assign class in editor).")
            return asset

        factory = unreal.DataAssetFactory()
        factory.set_editor_property("data_asset_class", cls)
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        asset = tools.create_asset(name, directory, cls, factory)
        if asset:
            unreal.EditorAssetLibrary.save_loaded_asset(asset)
            unreal.log(f"Ashline: created {asset_path}")
        return asset
    except Exception as exc:
        unreal.log_warning(f"Ashline: could not create {asset_path} ({exc})")
        return None


def catalog_from_bindings():
    try:
        path = unreal.Paths.project_dir() + "Content/Ashline/Data/ContentBindings.json"
        with open(path, encoding="utf-8") as handle:
            data = json.load(handle)
        return list(data["cosmetics"].keys()), list(data["skins"].keys()), list(data["weapons"]), list((data.get("equipment") or {}).keys())
    except Exception:
        return None


def main():
    cosmetics = COSMETICS
    skins = SKINS
    weapons = WEAPONS
    equipment = []
    loaded = catalog_from_bindings()
    if loaded:
        cosmetics, skins, weapons, equipment = loaded
    for folder in FOLDERS:
        ensure_dir(folder)
    for index, slug, _notes in MISSIONS:
        ensure_dir(f"/Game/Ashline/Environments/ASH{index:02d}_{slug}")

    for index, slug, notes in MISSIONS:
        name = f"DA_Kit_ASH{index:02d}"
        asset = create_data_asset(
            name,
            KIT_ROOT,
            "/Script/Ashline.AshlineEnvironmentKit",
        )
        if asset and hasattr(asset, "set_editor_property"):
            try:
                asset.set_editor_property("kit_id", f"ASH{index:02d}")
                asset.set_editor_property("fab_notes", notes)
            except Exception:
                pass

    create_data_asset("DA_Hero_Operator", KIT_ROOT, "/Script/Ashline.AshlineCharacterPresentation")
    for slug in AI_ARCHETYPES:
        asset = create_data_asset(f"DA_AI_{slug}", KIT_ROOT, "/Script/Ashline.AshlineCharacterPresentation")
        if asset:
            try:
                asset.set_editor_property("presentation_id", f"AI_{slug}")
                asset.set_editor_property("b_hero", False)
            except Exception:
                pass
    for weapon in weapons:
        asset = create_data_asset(f"DA_WPN_{weapon}", KIT_ROOT, "/Script/Ashline.AshlineWeaponVisual")
        if asset:
            try:
                asset.set_editor_property("weapon_id", weapon)
                asset.set_editor_property(
                    "world_mesh",
                    unreal.SoftObjectPath(f"/Game/Ashline/Weapons/Meshes/SM_{weapon}.SM_{weapon}"),
                )
            except Exception:
                pass
    for cosmetic in cosmetics:
        asset = create_data_asset(f"DA_COS_{cosmetic}", KIT_ROOT, "/Script/Ashline.AshlineCosmeticVisual")
        if asset:
            try:
                asset.set_editor_property("cosmetic_id", cosmetic)
                asset.set_editor_property(
                    "mesh_override",
                    unreal.SoftObjectPath(f"/Game/Ashline/Characters/Hero/Cosmetics/SK_{cosmetic}.SK_{cosmetic}"),
                )
                asset.set_editor_property(
                    "material_override",
                    unreal.SoftObjectPath(f"/Game/Ashline/Materials/Cosmetics/M_{cosmetic}.M_{cosmetic}"),
                )
                asset.set_editor_property(
                    "part_mesh",
                    unreal.SoftObjectPath(f"/Game/Ashline/Characters/Hero/Parts/SM_{cosmetic}.SM_{cosmetic}"),
                )
            except Exception:
                pass
    for skin in skins:
        asset = create_data_asset(f"DA_SKIN_{skin}", KIT_ROOT, "/Script/Ashline.AshlineCosmeticVisual")
        if asset:
            try:
                asset.set_editor_property("cosmetic_id", skin)
                asset.set_editor_property(
                    "material_override",
                    unreal.SoftObjectPath(f"/Game/Ashline/Weapons/Materials/M_{skin}.M_{skin}"),
                )
            except Exception:
                pass
    for eq in equipment:
        asset = create_data_asset(f"DA_EQ_{eq}", KIT_ROOT, "/Script/Ashline.AshlineCosmeticVisual")
        if asset:
            try:
                asset.set_editor_property("cosmetic_id", eq)
                asset.set_editor_property(
                    "part_mesh",
                    unreal.SoftObjectPath(f"/Game/Ashline/Weapons/Equipment/SM_{eq}.SM_{eq}"),
                )
            except Exception:
                pass

    unreal.log(FAB_LIST)
    unreal.log("Ashline: kit / weapon / cosmetic / AI DataAsset stubs ready.")
    unreal.log("Next: Scripts/assign_interim_meshes.py then Docs/PHASE2_FAB.md install order.")
    unreal.log(UAshline_fallback())


def UAshline_fallback():
    return (
        "Fallback chain: DataAssets → /Game/StarterContent → Engine materials. "
        "No Quixel binaries were downloaded."
    )


if __name__ == "__main__":
    main()
