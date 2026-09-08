#!/usr/bin/env python3
"""No-UE release hygiene. Fails if DeviceProfiles self-parent or Shipping gates regress.

Named Ashline_* DeviceProfiles live in PR #8 — this fold only requires platform
roots and no self-parent. Do not fail when those named rows are absent.
"""

from __future__ import annotations

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]


def read(rel: str) -> str:
    path = ROOT / rel
    if not path.is_file():
        raise SystemExit(f"MISSING {rel}")
    return path.read_text(encoding="utf-8")


def parse_device_profiles(text: str) -> list[tuple[str, str | None]]:
    current = None
    base = None
    found: list[tuple[str, str | None]] = []
    for raw in text.splitlines():
        line = raw.split(";", 1)[0].strip()
        if not line:
            continue
        section = re.match(r"^\[(.+?) DeviceProfile\]$", line)
        if section:
            if current is not None:
                found.append((current, base))
            current = section.group(1)
            base = None
            continue
        if current is None:
            continue
        parent = re.match(r"^BaseProfileName=(.*)$", line)
        if parent:
            base = parent.group(1).strip()
    if current is not None:
        found.append((current, base))
    return found


def main() -> int:
    errors: list[str] = []
    warns: list[str] = []

    dp = read("Config/DefaultDeviceProfiles.ini")
    profiles = parse_device_profiles(dp)
    names = {name for name, _ in profiles}
    for name, base in profiles:
        if base and base == name:
            errors.append(f"DeviceProfile '{name}' self-parents (BaseProfileName={base})")

    required = {"Windows", "WindowsEditor", "Mac", "IOS"}
    missing = sorted(required - names)
    if missing:
        errors.append(f"Missing platform DeviceProfiles: {', '.join(missing)}")

    named = {
        "Ashline_Ultra",
        "Ashline_High",
        "Ashline_Balanced",
        "Ashline_Perf",
        "Ashline_Deck",
        "Ashline_PC_Ultra",
        "Ashline_PC_High",
        "Ashline_PC_Balanced",
        "Ashline_PC_Performance",
        "Ashline_SteamDeck",
        "Linux",
        "SteamDeck",
    }
    present_named = sorted(named & names)
    if present_named:
        print(f"Named / extra DeviceProfiles present (OK if from PR #8): {', '.join(present_named)}")
    else:
        warns.append(
            "Named Ashline_* DeviceProfiles are absent here — expected; they live in PR #8"
        )

    game = read("Config/DefaultGame.ini")
    if re.search(r"^DebugDisplay\s*=\s*Ashline", game, re.M):
        errors.append("DefaultGame.ini still forces DebugDisplay=Ashline (not Shipping-safe)")
    if "PPBC_Shipping" not in game:
        errors.append("DefaultGame.ini missing BuildConfiguration=PPBC_Shipping")
    if re.search(r"MapsToCook=.*ASH_Playable", game):
        errors.append("DefaultGame.ini MapsToCook lists ASH_Playable — cook fails when the optional map is missing")

    target = read("Source/Ashline.Target.cs")
    if "bUseLoggingInShipping = false" not in target:
        errors.append("Ashline.Target.cs must disable logging in Shipping")

    cheats = read("Source/Ashline/Player/AshlinePlayerController.cpp")
    if "AshlineShipping::CheatsAllowed" not in cheats:
        errors.append("PlayerController cheats are not gated on AshlineShipping::CheatsAllowed")
    for cmd in ("AshUnlockAll", "AshGrantCredits", "AshSetRank", "AshUnlockMeta", "AshFSR", "AshTSR"):
        if cmd not in cheats:
            errors.append(f"Missing exec implementation {cmd}")
    if "PLATFORM_LINUX" not in cheats:
        errors.append("Gamepad mapping context is not applied on PLATFORM_LINUX (Proton/native)")

    shipping = read("Source/Ashline/Core/AshlineShipping.h")
    if "UE_BUILD_SHIPPING" not in shipping:
        errors.append("AshlineShipping.h missing UE_BUILD_SHIPPING gate")

    soft = read("Source/Ashline/Presentation/AshlineLoad.h")
    if "CanAttemptLoad" not in soft or "LOAD_NoWarn" not in soft:
        errors.append("AshlineLoad.h missing quiet-load helpers")

    types = read("Source/Ashline/AshlineTypes.h")
    if "SteamDeck" not in types:
        errors.append("EAshlineGraphicsPreset missing SteamDeck")

    runtime = read("Source/Ashline/Input/AshlineRuntimeInput.cpp")
    if "IA_Ashline_Sprint" not in runtime:
        errors.append("Runtime input missing Sprint action")
    if "EDeadZoneType::Radial" not in runtime:
        errors.append("Runtime input missing radial stick deadzones")
    if "Gamepad_LeftShoulder" not in runtime:
        errors.append("Interact must stay on gamepad LeftShoulder (do not steal LB for swap)")

    gfx = read("Source/Ashline/Settings/AshlineGraphicsSettings.cpp")
    if 'TEXT("AshFSR")' not in gfx or 'TEXT("AshTSR")' not in gfx:
        errors.append("Graphics settings missing AshFSR / AshTSR console commands")

    scal = read("Config/DefaultScalability.ini")
    for group in ("@0", "@3"):
        if f"AntiAliasingQuality{group}" not in scal:
            errors.append(f"DefaultScalability.ini missing AntiAliasingQuality{group}")

    print(f"DeviceProfiles parsed: {len(profiles)}")
    for name, base in profiles:
        parent = base if base else "(root)"
        print(f"  {name:22} -> {parent}")

    for warn in warns:
        print(f"WARN  {warn}")
    for err in errors:
        print(f"FAIL  {err}")

    if errors:
        print(f"\nvalidate_release_config: {len(errors)} error(s)")
        return 1
    print("\nvalidate_release_config: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
