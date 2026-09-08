"""
Ashline editor utility — Phase 2 kit scaffolding (UE 5.8.2).

Does NOT download Fab / Quixel / MetaHuman. It creates the Content folder
tree and empty DataAsset stubs so you can drop in licensed packs.

  Tools → Execute Python Script → Scripts/import_fab_kits.py
  py "Scripts/import_fab_kits.py"
"""

import unreal


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
    "/Game/Ashline/Environments/Shared",
    "/Game/Ashline/Materials/PBR",
    "/Game/Ashline/Materials/PBR/Masters",
    "/Game/Ashline/Materials/PBR/Instances",
    "/Game/Ashline/Materials/Decals",
    "/Game/Ashline/Materials/Cosmetics",
    "/Game/Ashline/Materials/Glass",
    "/Game/Ashline/Materials/Skin",
    "/Game/Ashline/Materials/Libraries",
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
    "WPN_SMG_C9",
    "WPN_SNP_G28L",
    "WPN_SHG_M870K",
    "WPN_PIS_M17A",
    "WPN_DMR_SASS",
    "WPN_LMG_M250",
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
Ashline Fab / Bridge evening install (license these yourself — not in git)
----------------------------------------------------------------------
0:00  Starter Content + import_fab_kits.py + create_master_materials.py + assign_interim_meshes.py
0:25  MetaHuman Creator/Bridge → /Game/Ashline/Characters/MetaHuman/  + GASP 5.8 AnimBP
1:20  Lyra (free) or Modern Firearms (paid) → SM_WPN_* + M_Weapon_Master skins
2:10  Megascans ASH-01, 02, 07, 12 first, then glass (04/10), then the rest
4:10  Audio/FX, AMD FSR 5.8, PIE all 12. Ultra pool 5600 / Deck pool 1600
Masters : M_Env / M_Weapon / M_Character / M_Glass / M_Skin / M_Decal
Fallback: Engine DefaultTexture/DefaultNormal — missing Quixel never breaks PIE
Docs    : Docs/PHASE2_FAB.md (bible)  Docs/MATERIALS.md (parameter contract)
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


def main():
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
                glass_path = f"/Game/Ashline/Environments/ASH{index:02d}_{slug}/M_Glass_ASH{index:02d}_{slug}.M_Glass_ASH{index:02d}_{slug}"
                asset.set_editor_property("glass_material", unreal.SoftObjectPath(glass_path))
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
    for weapon in WEAPONS:
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
    for cosmetic in COSMETICS:
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
    for skin in SKINS:
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

    unreal.log(FAB_LIST)
    unreal.log("Ashline: kit / weapon / cosmetic / AI DataAsset stubs ready.")
    try:
        import importlib.util
        import os

        project_dir = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
        script = os.path.join(project_dir, "Scripts", "create_master_materials.py")
        spec = importlib.util.spec_from_file_location("ashline_masters", script)
        if spec and spec.loader:
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            mod.main()
        else:
            unreal.log_warning("Ashline: create_master_materials.py not found — run it next.")
    except Exception as nested:
        unreal.log_warning(f"Ashline: create_master_materials.py skipped ({nested})")
    unreal.log("Next: Scripts/assign_interim_meshes.py then Docs/PHASE2_FAB.md evening order.")
    unreal.log(UAshline_fallback())


def UAshline_fallback():
    return (
        "Fallback chain: Masters/MIs → DataAssets → /Game/StarterContent → Engine DefaultTexture/DefaultMaterial. "
        "No Quixel binaries were downloaded. Missing Fab never breaks PIE/Shipping."
    )


if __name__ == "__main__":
    main()
