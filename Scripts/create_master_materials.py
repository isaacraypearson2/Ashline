"""
Ashline editor utility — create master materials + surface MIs (UE 5.8.2).

Does NOT download Fab. Builds M_Env/Weapon/Character/Glass/Skin/Decal masters
with the parameter names UAshlineMaterialFactory stamps at runtime.

  Tools → Execute Python Script → Scripts/create_master_materials.py
"""

from __future__ import annotations

import unreal

MASTER_DIR = "/Game/Ashline/Materials/PBR/Masters"
INSTANCE_DIR = "/Game/Ashline/Materials/PBR/Instances"
GLASS_DIR = "/Game/Ashline/Materials/Glass"
SKIN_DIR = "/Game/Ashline/Materials/Skin"
DECAL_DIR = "/Game/Ashline/Materials/Decals"
LIB_DIR = "/Game/Ashline/Materials/Libraries"

SURFACE_INSTANCES = [
    ("MI_Env_Ground", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.78, 0.03),
    ("MI_Env_Concrete", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.74, 0.04),
    ("MI_Env_Metal", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.32, 0.88),
    ("MI_Env_Wood", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.70, 0.02),
    ("MI_Env_Sand", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.82, 0.00),
    ("MI_Env_Snow", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.78, 0.00),
    ("MI_Env_Water", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.08, 0.00),
    ("MI_Env_Foliage", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.68, 0.00),
    ("MI_Env_Emissive", MASTER_DIR + "/M_Env_Master", INSTANCE_DIR, 0.40, 0.10),
    ("MI_Char_Clothing", MASTER_DIR + "/M_Character_Master", INSTANCE_DIR, 0.55, 0.08),
    ("MI_Glass_Clear", GLASS_DIR + "/M_Glass_Master", GLASS_DIR, 0.04, 0.00),
    ("MI_Skin_Operator", SKIN_DIR + "/M_Skin_Master", SKIN_DIR, 0.55, 0.00),
]


def ensure_dir(path: str) -> None:
    unreal.EditorAssetLibrary.make_directory(path)


def _exists(path: str) -> bool:
    return unreal.EditorAssetLibrary.does_asset_exist(path)


def _load(path: str):
    if _exists(path):
        return unreal.EditorAssetLibrary.load_asset(path)
    return None


def _save(asset) -> None:
    if asset:
        unreal.EditorAssetLibrary.save_loaded_asset(asset)


def _mp(name: str):
    enum = getattr(unreal, "MaterialProperty", None)
    if enum is None:
        return None
    for candidate in (name, "MP_" + name, name.upper()):
        if hasattr(enum, candidate):
            return getattr(enum, candidate)
    return None


def _connect_prop(expr, output: str, prop) -> None:
    if expr is None or prop is None:
        return
    try:
        unreal.MaterialEditingLibrary.connect_material_property(expr, output, prop)
    except Exception as exc:
        unreal.log_warning(f"Ashline: connect {output} failed ({exc})")


def _tex_param(mat, name: str, x: int, y: int):
    expr = unreal.MaterialEditingLibrary.create_material_expression(
        mat, unreal.MaterialExpressionTextureSampleParameter2D, x, y
    )
    expr.set_editor_property("parameter_name", name)
    return expr


def _scalar(mat, name: str, value: float, x: int, y: int):
    expr = unreal.MaterialEditingLibrary.create_material_expression(
        mat, unreal.MaterialExpressionScalarParameter, x, y
    )
    expr.set_editor_property("parameter_name", name)
    expr.set_editor_property("default_value", value)
    return expr


def _vector(mat, name: str, color, x: int, y: int):
    expr = unreal.MaterialEditingLibrary.create_material_expression(
        mat, unreal.MaterialExpressionVectorParameter, x, y
    )
    expr.set_editor_property("parameter_name", name)
    expr.set_editor_property("default_value", color)
    return expr


def create_lit_master(name: str, directory: str, *, translucent: bool = False, subsurface: bool = False, decal: bool = False):
    asset_path = f"{directory}/{name}"
    if _exists(asset_path):
        unreal.log(f"Ashline: exists {asset_path}")
        return _load(asset_path)

    factory = unreal.MaterialFactoryNew()
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    mat = tools.create_asset(name, directory, unreal.Material, factory)
    if not mat:
        unreal.log_warning(f"Ashline: could not create {asset_path}")
        return None

    try:
        if translucent:
            mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
        if subsurface:
            try:
                mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_SUBSURFACE)
            except Exception:
                pass
        if decal:
            try:
                mat.set_editor_property("material_domain", unreal.MaterialDomain.DEFERRED_DECAL)
                mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
            except Exception:
                pass

        base = _tex_param(mat, "BaseColor", -800, -200)
        normal = _tex_param(mat, "Normal", -800, 80)
        orm = _tex_param(mat, "ORM", -800, 360)
        tint = _vector(mat, "Tint", unreal.LinearColor(1.0, 1.0, 1.0, 1.0), -800, -420)
        rough = _scalar(mat, "Roughness", 0.65, -500, 520)
        metal = _scalar(mat, "Metallic", 0.05, -500, 620)
        tiling = _scalar(mat, "Tiling", 1.0, -500, 720)
        emissive_s = _scalar(mat, "EmissiveStrength", 0.0, -500, 820)
        opacity = _scalar(mat, "Opacity", 1.0 if not translucent else 0.28, -500, 920)

        mul = unreal.MaterialEditingLibrary.create_material_expression(
            mat, unreal.MaterialExpressionMultiply, -280, -200
        )
        try:
            unreal.MaterialEditingLibrary.connect_material_expressions(base, "RGB", mul, "A")
            unreal.MaterialEditingLibrary.connect_material_expressions(tint, "", mul, "B")
        except Exception:
            pass

        _connect_prop(mul, "", _mp("BASE_COLOR") or _mp("MP_BASE_COLOR"))
        _connect_prop(normal, "RGB", _mp("NORMAL") or _mp("MP_NORMAL"))
        _connect_prop(rough, "", _mp("ROUGHNESS") or _mp("MP_ROUGHNESS"))
        _connect_prop(metal, "", _mp("METALLIC") or _mp("MP_METALLIC"))
        try:
            _connect_prop(orm, "R", _mp("AMBIENT_OCCLUSION") or _mp("MP_AMBIENT_OCCLUSION"))
        except Exception:
            pass
        _connect_prop(opacity, "", _mp("OPACITY") or _mp("MP_OPACITY"))

        emul = unreal.MaterialEditingLibrary.create_material_expression(
            mat, unreal.MaterialExpressionMultiply, -280, 820
        )
        try:
            unreal.MaterialEditingLibrary.connect_material_expressions(tint, "", emul, "A")
            unreal.MaterialEditingLibrary.connect_material_expressions(emissive_s, "", emul, "B")
            _connect_prop(emul, "", _mp("EMISSIVE_COLOR") or _mp("MP_EMISSIVE_COLOR"))
        except Exception:
            pass

        # Tiling is a scalar parameter for MIs; C++ stamps UVScale/Tiling even if unused in the graph.
        _ = tiling
        unreal.MaterialEditingLibrary.layout_all_material_expressions(mat)
        unreal.MaterialEditingLibrary.recompile_material(mat)
        _save(mat)
        unreal.log(f"Ashline: created master {asset_path}")
        return mat
    except Exception as exc:
        unreal.log_warning(f"Ashline: master graph {asset_path} incomplete ({exc}). C++ Engine fallbacks still apply.")
        _save(mat)
        return mat


def create_mi(name: str, directory: str, parent_path: str, roughness: float, metallic: float):
    asset_path = f"{directory}/{name}"
    if _exists(asset_path):
        return _load(asset_path)
    parent = _load(parent_path)
    if not parent:
        unreal.log_warning(f"Ashline: skip MI {name}, parent missing {parent_path}")
        return None
    try:
        factory = unreal.MaterialInstanceConstantFactoryNew()
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        mi = tools.create_asset(name, directory, unreal.MaterialInstanceConstant, factory)
        if not mi:
            return None
        unreal.MaterialEditingLibrary.set_material_instance_parent(mi, parent)
        try:
            unreal.MaterialEditingLibrary.set_material_instance_scalar_parameter_value(mi, "Roughness", roughness)
            unreal.MaterialEditingLibrary.set_material_instance_scalar_parameter_value(mi, "Metallic", metallic)
        except Exception:
            pass
        _save(mi)
        unreal.log(f"Ashline: created MI {asset_path}")
        return mi
    except Exception as exc:
        unreal.log_warning(f"Ashline: MI {asset_path} failed ({exc})")
        return None


def main():
    for folder in (MASTER_DIR, INSTANCE_DIR, GLASS_DIR, SKIN_DIR, DECAL_DIR, LIB_DIR):
        ensure_dir(folder)

    create_lit_master("M_Env_Master", MASTER_DIR)
    create_lit_master("M_Weapon_Master", MASTER_DIR)
    create_lit_master("M_Character_Master", MASTER_DIR)
    create_lit_master("M_Glass_Master", GLASS_DIR, translucent=True)
    create_lit_master("M_Skin_Master", SKIN_DIR, subsurface=True)
    create_lit_master("M_Decal_Master", DECAL_DIR, decal=True)

    # Impact alias so the runtime decal path resolves immediately after this script.
    if not _exists(f"{DECAL_DIR}/M_Impact_Bullet"):
        create_mi("M_Impact_Bullet", DECAL_DIR, f"{DECAL_DIR}/M_Decal_Master", 0.9, 0.0)

    for name, parent, directory, rough, metal in SURFACE_INSTANCES:
        create_mi(name, directory, parent, rough, metal)

    unreal.log("Ashline: master materials + surface MIs ready. Docs/MATERIALS.md is the parameter bible.")
    unreal.log("Fallback: Engine DefaultTexture / DefaultNormal if a sample is empty.")


if __name__ == "__main__":
    main()
