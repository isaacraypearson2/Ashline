#!/usr/bin/env python3
"""Print the one-evening Fab / master-material install plan (no UE required)."""

from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PLAN = ROOT / "Content" / "Ashline" / "Data" / "FabInstallPlan.json"


def main() -> None:
    data = json.loads(PLAN.read_text(encoding="utf-8"))
    print("Ashline one-evening Fab install (Windows X3D_AMD / UE 5.8.2)")
    print("Soft refs: MI_Ashline{Slug} → M_Ashline{Slug} → M_{Slug} → Engine")
    print("=" * 72)
    for block in data["eveningHours"]:
        print(f"\n[{block['block']}] {block['title']}")
        for step in block["steps"]:
            print(f"  - {step}")
    streaming = data.get("streaming", {})
    if streaming:
        print("\nTexture streaming (do not fight named-preset pools)")
        for name, spec in streaming.items():
            print(
                f"  {name}: pool {spec.get('poolMB')} MB  "
                f"VT scale {spec.get('vtScale')}  aniso {spec.get('aniso')}"
            )
    print("\nFull bible: Docs/PHASE2_FAB.md  ·  Docs/MATERIALS.md")


if __name__ == "__main__":
    main()
