#!/usr/bin/env python3
"""Print the Ashline Fab evening install bible (no Unreal required)."""

from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLAN = ROOT / "Content" / "Ashline" / "Data" / "FabInstallPlan.json"
BINDINGS = ROOT / "Content" / "Ashline" / "Data" / "MaterialBindings.json"


def main() -> int:
    plan = json.loads(PLAN.read_text(encoding="utf-8"))
    mats = json.loads(BINDINGS.read_text(encoding="utf-8"))
    print("ASHLINE — one evening Fab / Megascans / MetaHuman install")
    print("=" * 68)
    print("License packs yourself. Nothing below is in git.")
    print("Masters + Engine fallbacks mean PIE works before any download.\n")
    for block in plan["eveningHours"]:
        print(f"[{block['block']}] {block['title']}")
        for step in block["steps"]:
            print(f"  - {step}")
        print()
    print("Streaming pools")
    for name, spec in plan["streaming"].items():
        print(
            f"  {name:12} pool={spec['poolMB']} MB  VT scale={spec['vtScale']}  aniso={spec['aniso']}"
        )
    print("\nMaster paths")
    for key, path in mats["masters"].items():
        print(f"  {key:12} {path}")
    print("\nDocs: Docs/PHASE2_FAB.md  Docs/MATERIALS.md  Docs/GRAPHICS.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
