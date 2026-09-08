"""
Optional Unreal Editor utility (UE 5.8.2).

Runtime Play already works without this script: AAshlineGrayboxBuilder +
UAshlineRuntimeInput spawn the campaign and binds on whatever host world loads.

Run this only if you want a project .umap + authored IA/IMC assets:

  Tools → Execute Python Script → Scripts/create_ashline_play_assets.py

or from the editor Output Log:

  py "Scripts/create_ashline_play_assets.py"
"""

import unreal


INPUT_PATH = "/Game/Ashline/Input"
MAP_PATH = "/Game/Ashline/Maps/ASH_Playable"


def _save_asset(asset):
    if not asset:
        return
    unreal.EditorAssetLibrary.save_loaded_asset(asset)


def create_input_action(name, value_type):
    asset_path = f"{INPUT_PATH}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    try:
        factory = unreal.InputActionFactory()
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        action = asset_tools.create_asset(name, INPUT_PATH, unreal.InputAction, factory)
        if action:
            action.set_editor_property("value_type", value_type)
            _save_asset(action)
        return action
    except Exception as exc:
        unreal.log_warning(f"Ashline: could not create {name} ({exc}). Runtime input still works.")
        return None


def create_mapping_context(name):
    asset_path = f"{INPUT_PATH}/{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        return unreal.EditorAssetLibrary.load_asset(asset_path)

    try:
        factory = unreal.InputMappingContextFactory()
        asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
        context = asset_tools.create_asset(name, INPUT_PATH, unreal.InputMappingContext, factory)
        _save_asset(context)
        return context
    except Exception as exc:
        unreal.log_warning(f"Ashline: could not create {name} ({exc}). Runtime input still works.")
        return None


def create_playable_map():
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.log(f"Ashline: map already exists at {MAP_PATH}")
        return

    unreal.EditorLevelLibrary.new_level(MAP_PATH)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        settings = world.get_world_settings()
        if settings:
            settings.set_editor_property("force_no_precomputed_lighting", True)
    subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    if subsystem:
        subsystem.save_current_level()
    unreal.log(f"Ashline: created {MAP_PATH}. Set World Settings GameMode to AshlineGameMode.")


def main():
    unreal.EditorAssetLibrary.make_directory(INPUT_PATH)
    unreal.EditorAssetLibrary.make_directory("/Game/Ashline/Maps")

    create_input_action("IA_Move", unreal.InputActionValueType.AXIS2D)
    create_input_action("IA_Look", unreal.InputActionValueType.AXIS2D)
    for digital in ("IA_Jump", "IA_Fire", "IA_Aim", "IA_Reload", "IA_CameraToggle", "IA_SwapWeapon", "IA_Crouch"):
        create_input_action(digital, unreal.InputActionValueType.BOOLEAN)

    create_mapping_context("IMC_Ashline_KBM")
    create_mapping_context("IMC_Ashline_Gamepad")
    create_mapping_context("IMC_Ashline_Touch")
    create_playable_map()
    unreal.log("Ashline: optional editor assets created. Runtime input/graybox still works if you skip assigning them.")


if __name__ == "__main__":
    main()
