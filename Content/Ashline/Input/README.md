# Input assets

Create in the Unreal Editor (Content Browser → Input):

**Mapping contexts**

- `IMC_Ashline_KBM`
- `IMC_Ashline_Gamepad`
- `IMC_Ashline_Touch`

**Actions**

- `IA_Move` (Axis2D)
- `IA_Look` (Axis2D)
- `IA_Jump`, `IA_Fire`, `IA_Aim`, `IA_Reload`, `IA_CameraToggle`, `IA_SwapWeapon`, `IA_Crouch` (Digital)

Assign the actions on `AAshlineCharacter` defaults and the contexts on `AAshlinePlayerController`.
