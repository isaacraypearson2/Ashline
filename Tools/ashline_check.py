#!/usr/bin/env python3
"""Ashline headless content/config validator.

Ashline is an Apple-first Unreal Engine 5.8.2 project (Mac/Metal + iOS), so the
editor and game cannot build or run on a Linux Cloud Agent host. What *can* be
exercised end to end without the engine is the data-driven layer the game reads
at runtime: the campaign/weapon JSON in Content/, the C++ catalogs that mirror
that data, and the engine .ini config. This tool loads all of it and checks:

  * every data JSON file parses;
  * Campaign.json is well-formed (12 missions, ids, maps, objectives) and each
    map path points at an existing map plan under Content/;
  * Weapons.json roster/slots use valid classes and slots;
  * the JSON data stays consistent with the C++ source of truth
    (UAshlineMissionCatalog / UAshlineWeaponCatalog and the enums in
    AshlineTypes.h) -- ids, titles, maps, classes, unlock levels;
  * no weapon references an attachment id that the attachment catalog does not
    define, and no attachment declares an unknown slot;
  * every Config/**/*.ini file is structurally valid (sections + key=value).

Exit code is 0 when everything passes and non-zero otherwise, so it doubles as a
CI-style smoke test for the environment.
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

# --- tiny reporting helpers -------------------------------------------------

_ERRORS: list[str] = []
_CHECKS = 0


def check(condition: bool, message: str) -> bool:
    global _CHECKS
    _CHECKS += 1
    if not condition:
        _ERRORS.append(message)
    return condition


def section(title: str) -> None:
    print(f"\n== {title} ==")


def ok(message: str) -> None:
    print(f"  ok  {message}")


def fail(message: str) -> None:
    print(f"  FAIL {message}")


# --- source-of-truth parsing (C++) -----------------------------------------

def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def parse_enum(header: str, enum_name: str) -> list[str]:
    """Return the member names of `enum class <enum_name> : ...` from a header."""
    m = re.search(
        r"enum\s+class\s+" + re.escape(enum_name) + r"\s*:[^\{]*\{(.*?)\}",
        header,
        re.DOTALL,
    )
    if not m:
        return []
    body = m.group(1)
    members = []
    for raw in body.split(","):
        token = raw.strip()
        if not token:
            continue
        name = token.split("=")[0].strip()
        name = name.split()[0] if name else name
        if name:
            members.append(name)
    return members


def parse_cpp_missions(cpp: str) -> list[dict]:
    """Extract (enum, code, title, theme, location, map) from each Make(...)."""
    pattern = re.compile(
        r"Make\(\s*EAshlineMissionId::(\w+)\s*,"
        r'\s*TEXT\("([^"]*)"\)\s*,'  # code
        r'\s*TEXT\("([^"]*)"\)\s*,'  # title
        r'\s*TEXT\("([^"]*)"\)\s*,'  # theme
        r'\s*TEXT\("([^"]*)"\)\s*,'  # location
        r'\s*TEXT\("([^"]*)"\)',      # map
        re.DOTALL,
    )
    out = []
    for m in pattern.finditer(cpp):
        out.append(
            {
                "enum": m.group(1),
                "code": m.group(2),
                "title": m.group(3),
                "theme": m.group(4),
                "location": m.group(5),
                "map": m.group(6),
            }
        )
    return out


def parse_cpp_weapons(cpp: str) -> list[dict]:
    pattern = re.compile(
        r'W\(\s*TEXT\("([^"]+)"\)\s*,'      # id
        r'\s*TEXT\("([^"]*)"\)\s*,'          # name
        r"\s*EAshlineWeaponClass::(\w+)\s*," # class
        r"\s*S\((.*?)\)\s*,"                  # stats
        r"\s*(\d+)\s*,"                       # unlock
        r"\s*\{(.*?)\}",                      # attachments
        re.DOTALL,
    )
    out = []
    for m in pattern.finditer(cpp):
        attachments = re.findall(r'TEXT\("([^"]+)"\)', m.group(6))
        out.append(
            {
                "id": m.group(1),
                "name": m.group(2),
                "class": m.group(3),
                "unlock": int(m.group(5)),
                "attachments": attachments,
            }
        )
    return out


def parse_cpp_attachments(cpp: str) -> list[dict]:
    pattern = re.compile(
        r'A\(\s*TEXT\("([^"]+)"\)\s*,'        # id
        r'\s*TEXT\("([^"]*)"\)\s*,'            # name
        r"\s*EAshlineAttachmentSlot::(\w+)\s*," # slot
        r"\s*(\d+)\s*,",                        # unlock
        re.DOTALL,
    )
    return [
        {"id": m.group(1), "name": m.group(2), "slot": m.group(3), "unlock": int(m.group(4))}
        for m in pattern.finditer(cpp)
    ]


# --- checks -----------------------------------------------------------------

def load_json(rel: str):
    path = REPO / rel
    if not check(path.is_file(), f"missing data file: {rel}"):
        return None
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
        ok(f"parsed {rel}")
        return data
    except json.JSONDecodeError as exc:
        check(False, f"invalid JSON in {rel}: {exc}")
        fail(f"{rel}: {exc}")
        return None


def check_campaign(types_header: str, mission_cpp: str) -> None:
    section("Campaign data <-> C++ catalog")
    data = load_json("Content/Ashline/Data/Campaign.json")
    if data is None:
        return

    enum_members = [m for m in parse_enum(types_header, "EAshlineMissionId") if m != "None"]
    cpp_missions = parse_cpp_missions(mission_cpp)
    check(len(enum_members) == 12, f"EAshlineMissionId has 12 missions (found {len(enum_members)})")
    check(len(cpp_missions) == 12, f"C++ catalog builds 12 missions (found {len(cpp_missions)})")

    missions = data.get("missions", [])
    check(isinstance(missions, list) and len(missions) == 12,
          f"Campaign.json has 12 missions (found {len(missions) if isinstance(missions, list) else 'n/a'})")

    cpp_by_code = {m["code"]: m for m in cpp_missions}
    seen_ids = set()
    for i, mission in enumerate(missions, start=1):
        mid = mission.get("id", "")
        expected = f"ASH-{i:02d}"
        check(mid == expected, f"mission #{i} id is {expected} (found {mid!r})")
        check(mid not in seen_ids, f"mission id {mid} is unique")
        seen_ids.add(mid)
        for field in ("title", "theme", "map", "objectives"):
            check(field in mission and mission[field], f"{mid} has non-empty '{field}'")
        objs = mission.get("objectives", [])
        check(isinstance(objs, list) and len(objs) >= 1, f"{mid} has >=1 objective")

        map_path = mission.get("map", "")
        check(map_path.startswith("/Game/Ashline/Maps/Campaign/"),
              f"{mid} map path is under /Game/Ashline/Maps/Campaign/ ({map_path!r})")
        map_name = map_path.rsplit("/", 1)[-1]
        map_plan = REPO / "Content/Ashline/Maps/Campaign" / f"{map_name}.md"
        check(map_plan.is_file(), f"{mid} map plan exists: Content/.../{map_name}.md")

        cpp = cpp_by_code.get(mid)
        if check(cpp is not None, f"{mid} present in C++ mission catalog"):
            check(cpp["title"] == mission.get("title"),
                  f"{mid} title matches C++ ({cpp['title']!r} vs {mission.get('title')!r})")
            check(cpp["map"] == map_path,
                  f"{mid} map matches C++ ({cpp['map']!r} vs {map_path!r})")
    if not _ERRORS:
        ok("all 12 missions consistent across JSON, C++ catalog, enum, and map plans")


def check_weapons(types_header: str, weapon_cpp: str) -> None:
    section("Weapon data <-> C++ catalog")
    data = load_json("Content/Ashline/Data/Weapons.json")
    if data is None:
        return

    weapon_classes = set(parse_enum(types_header, "EAshlineWeaponClass"))
    slots = parse_enum(types_header, "EAshlineAttachmentSlot")
    cpp_weapons = {w["id"]: w for w in parse_cpp_weapons(weapon_cpp)}
    cpp_attachments = {a["id"]: a for a in parse_cpp_attachments(weapon_cpp)}
    check(len(cpp_weapons) == 7, f"C++ roster has 7 weapons (found {len(cpp_weapons)})")
    check(len(cpp_attachments) >= 1, f"C++ attachment catalog non-empty (found {len(cpp_attachments)})")

    # JSON roster
    roster = data.get("roster", [])
    check(isinstance(roster, list) and len(roster) == len(cpp_weapons),
          f"Weapons.json roster count matches C++ ({len(roster)} vs {len(cpp_weapons)})")
    for w in roster:
        wid = w.get("id", "")
        check(w.get("class") in weapon_classes,
              f"{wid} class {w.get('class')!r} is a valid EAshlineWeaponClass")
        check(isinstance(w.get("unlock"), int) and w["unlock"] >= 1,
              f"{wid} unlock is a positive int")
        cpp = cpp_weapons.get(wid)
        if check(cpp is not None, f"{wid} present in C++ roster"):
            check(cpp["class"] == w.get("class"),
                  f"{wid} class matches C++ ({cpp['class']!r} vs {w.get('class')!r})")
            check(cpp["unlock"] == w.get("unlock"),
                  f"{wid} unlock matches C++ ({cpp['unlock']} vs {w.get('unlock')})")

    # JSON attachment slots vs enum
    json_slots = data.get("attachmentSlots", [])
    check(json_slots == slots,
          f"Weapons.json attachmentSlots match EAshlineAttachmentSlot enum ({json_slots} vs {slots})")

    # C++ referential integrity: every referenced attachment must be defined,
    # and every attachment slot must be a valid enum value.
    slot_set = set(slots)
    for a in cpp_attachments.values():
        check(a["slot"] in slot_set,
              f"attachment {a['id']} slot {a['slot']!r} is a valid EAshlineAttachmentSlot")
    dangling = []
    for wid, w in cpp_weapons.items():
        for att in w["attachments"]:
            if att not in cpp_attachments:
                dangling.append(f"{wid} -> {att}")
    check(not dangling, f"no weapon references an undefined attachment (dangling: {dangling})")
    if not _ERRORS:
        ok(f"roster of {len(cpp_weapons)} weapons and {len(cpp_attachments)} attachments fully consistent")


def check_ini() -> None:
    section("Engine config (.ini structure)")
    ini_files = sorted((REPO / "Config").rglob("*.ini"))
    check(len(ini_files) >= 1, "at least one Config/*.ini file exists")
    section_re = re.compile(r"^\[[^\]]+\]$")
    # UE keys may be prefixed with + . ! - and contain dots/underscores/slashes.
    kv_re = re.compile(r"^[+\-.!]?[\w.\-/ ]+=")
    for path in ini_files:
        rel = path.relative_to(REPO)
        bad = []
        has_section = False
        for lineno, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            line = raw.strip()
            if not line or line.startswith(";") or line.startswith("#"):
                continue
            if section_re.match(line):
                has_section = True
                continue
            if kv_re.match(line):
                continue
            bad.append(f"{rel}:{lineno}: {raw!r}")
        check(not bad, f"{rel} has only sections/key=value lines (offenders: {bad[:3]})")
        check(has_section, f"{rel} declares at least one [section]")
        if not bad and has_section:
            ok(f"{rel}")


def main() -> int:
    types_header = read(REPO / "Source/Ashline/AshlineTypes.h")
    mission_cpp = read(REPO / "Source/Ashline/Campaign/AshlineMissionCatalog.cpp")
    weapon_cpp = read(REPO / "Source/Ashline/Weapons/AshlineWeaponCatalog.cpp")

    check_campaign(types_header, mission_cpp)
    check_weapons(types_header, weapon_cpp)
    check_ini()

    print("\n" + "=" * 60)
    if _ERRORS:
        print(f"RESULT: FAIL — {len(_ERRORS)} problem(s) out of {_CHECKS} checks")
        for err in _ERRORS:
            print(f"  - {err}")
        return 1
    print(f"RESULT: PASS — all {_CHECKS} checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
