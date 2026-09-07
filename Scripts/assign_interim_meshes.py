"""
Ashline Editor Utility — assign Engine / Marketplace mannequin paths.

Replaces capsule/cube hero+AI visuals with the first available humanoid
skeletal mesh (TP mannequin, GASP UEFN, Engine SK_Mannequin, editor default).

Does NOT download Fab. Run after Starter Content / GASP / Third Person template
meshes exist in this project.

  Tools → Execute Python Script → Scripts/assign_interim_meshes.py
  py "Scripts/assign_interim_meshes.py"
"""

from __future__ import annotations

import unreal


KIT_ROOT = "/Game/Ashline/Data/Kits"

HUMANOID_CANDIDATES = [
    "/Game/Ashline/Characters/Hero/SK_AshlineHero",
    "/Game/Ashline/Characters/MetaHuman/SK_MetaHumanBody",
    "/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple",
    "/Game/Characters/Mannequins/Meshes/SKM_Manny",
    "/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple",
    "/Game/Characters/Mannequins/Meshes/SKM_Quinn",
    "/Game/Characters/Mannequins/Meshes/SK_Mannequin",
    "/Game/GameAnimationSample/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin",
    "/Game/GameAnimationSample/Characters/UEFN_Mannequin/Meshes/SKM_UEFN_Mannequin_Simple",
    "/Game/Characters/Heroes/Mannequin/Meshes/SK_Mannequin",
    "/Engine/EngineMeshes/SkeletalMesh/SK_Mannequin",
    "/Engine/EngineMeshes/SkeletalMesh/DefaultSkeletalMesh",
    "/Engine/EditorMeshes/AssetViewer/SkeletalMesh/DefaultSkeletalMesh",
    "/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP",
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

STARTER_MATERIALS = {
    "ground": [
        "/Game/StarterContent/Materials/M_Ground_Grass",
        "/Game/StarterContent/Materials/M_Ground_Moss",
    ],
    "wall": [
        "/Game/StarterContent/Materials/M_Concrete_Tiles",
        "/Game/StarterContent/Materials/M_Concrete_Poured",
    ],
    "trim": [
        "/Game/StarterContent/Materials/M_Metal_Steel",
        "/Game/StarterContent/Materials/M_Metal_Brushed",
    ],
    "foliage": [
        "/Game/StarterContent/Materials/M_Ground_Grass",
    ],
}


def _exists(path: str) -> bool:
    return unreal.EditorAssetLibrary.does_asset_exist(path)


def _load(path: str):
    if _exists(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    return None


def first_humanoid():
    for path in HUMANOID_CANDIDATES:
        asset = _load(path)
        if asset:
            unreal.log(f"Ashline: interim humanoid → {path}")
            return asset, path
    unreal.log_warning(
        "Ashline: no humanoid skeletal mesh found. Add Starter Content, "
        "migrate the Third Person mannequin, or add GASP. Capsules stay hidden; "
        "runtime uses the tinted blockout body."
    )
    return None, ""


def first_material(paths):
    for path in paths:
        asset = _load(path)
        if asset:
            return asset
    return None


def ensure_character_da(name: str, hero: bool, archetype: str = "Rifleman"):
    path = f"{KIT_ROOT}/{name}"
    if _exists(path):
        return unreal.EditorAssetLibrary.load_asset(path)

    cls = unreal.load_class(None, "/Script/Ashline.AshlineCharacterPresentation")
    factory = unreal.DataAssetFactory()
    if cls:
        factory.set_editor_property("data_asset_class", cls)
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset = tools.create_asset(
        name,
        KIT_ROOT,
        cls or unreal.PrimaryDataAsset,
        factory,
    )
    if asset:
        try:
            asset.set_editor_property("presentation_id", name)
            asset.set_editor_property("b_hero", hero)
        except Exception:
            pass
        unreal.EditorAssetLibrary.save_loaded_asset(asset)
    return asset


def assign_body(asset, mesh):
    if not asset or not mesh:
        return
    try:
        asset.set_editor_property("body_mesh", mesh)
        asset.set_editor_property("mesh_relative_location", unreal.Vector(0.0, 0.0, -96.0))
        asset.set_editor_property("mesh_relative_rotation", unreal.Rotator(0.0, -90.0, 0.0))
        unreal.EditorAssetLibrary.save_loaded_asset(asset)
        unreal.log(f"Ashline: assigned body mesh on {asset.get_path_name()}")
    except Exception as exc:
        unreal.log_warning(f"Ashline: could not assign body ({exc})")


def stamp_kit_materials():
    ground = first_material(STARTER_MATERIALS["ground"])
    wall = first_material(STARTER_MATERIALS["wall"])
    trim = first_material(STARTER_MATERIALS["trim"])
    foliage = first_material(STARTER_MATERIALS["foliage"])
    if not any((ground, wall, trim, foliage)):
        unreal.log("Ashline: no Starter Content materials — kit DAs left unbound.")
        return

    for index in range(1, 13):
        path = f"{KIT_ROOT}/DA_Kit_ASH{index:02d}"
        asset = _load(path)
        if not asset:
            continue
        try:
            if ground:
                asset.set_editor_property("ground_material", ground)
            if wall:
                asset.set_editor_property("wall_material", wall)
            if trim:
                asset.set_editor_property("trim_material", trim)
            if foliage:
                asset.set_editor_property("foliage_material", foliage)
            # Never stamp b_override_mood — C++ MoodForMission stays the lighting source.
            unreal.EditorAssetLibrary.save_loaded_asset(asset)
        except Exception as exc:
            unreal.log_warning(f"Ashline: kit material stamp failed on {path} ({exc})")
    unreal.log("Ashline: stamped Starter Content materials onto DA_Kit_ASH01…12 (mood still C++).")


def main():
    mesh, path = first_humanoid()

    hero = ensure_character_da("DA_Hero_Operator", True)
    assign_body(hero, mesh)
    for slug in AI_ARCHETYPES:
        ai = ensure_character_da(f"DA_AI_{slug}", False, slug)
        assign_body(ai, mesh)

    stamp_kit_materials()

    if mesh:
        unreal.log(
            "Ashline: interim hero+AI meshes assigned. PIE should show a mannequin, "
            f"not a capsule. Source: {path}"
        )
    else:
        unreal.log(
            "Ashline: no mesh to assign. Runtime HideCapsuleVisual + humanoid blockout still apply."
        )


if __name__ == "__main__":
    main()
