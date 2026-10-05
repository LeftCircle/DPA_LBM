#!/usr/bin/env python3

import argparse
import math
import re
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def parse_scalar_float(s: str) -> float:
    return float(s.strip())


def parse_line(raw_line: str, fallback_time: float | None = None):
    line = raw_line.strip()
    if not line or line.startswith("#"):
        return None

    # Case 1: "mass(px, py)"  -> no explicit time
    # Case 2: "time mass(px, py)" -> explicit time
    # Case 3: "time mass px" -> older format
    parts = line.split()

    if len(parts) == 1:
        m = re.fullmatch(r"([+-]?(?:\d+\.\d*|\d*\.\d+|\d+)(?:[eE][+-]?\d+)?)\(([^,]+),\s*([^)]+)\)", parts[0])
        if not m:
            raise ValueError(f"Bad line format: {raw_line!r}")
        mass = parse_scalar_float(m.group(1))
        px = parse_scalar_float(m.group(2))
        py = parse_scalar_float(m.group(3))
        t = fallback_time if fallback_time is not None else 0.0
        return t, mass, math.hypot(px, py)

    if len(parts) == 2:
        # time mass(px, py)
        t = parse_scalar_float(parts[0])
        rest = parts[1]
        m = re.fullmatch(r"([+-]?(?:\d+\.\d*|\d*\.\d+|\d+)(?:[eE][+-]?\d+)?)\(([^,]+),\s*([^)]+)\)", rest)
        if not m:
            raise ValueError(f"Bad line format: {raw_line!r}")
        mass = parse_scalar_float(m.group(1))
        px = parse_scalar_float(m.group(2))
        py = parse_scalar_float(m.group(3))
        return t, mass, math.hypot(px, py)

    if len(parts) == 3:
        # time mass momentum
        t = parse_scalar_float(parts[0])
        mass = parse_scalar_float(parts[1])
        momentum = parse_scalar_float(parts[2])
        return t, mass, momentum

    raise ValueError(f"Bad line format: {raw_line!r}")


def parse_data_file(path: Path):
    times = []
    masses = []
    momenta = []

    with path.open("r", encoding="utf-8") as f:
        for line_number, raw_line in enumerate(f, start=1):
            parsed = parse_line(raw_line, fallback_time=len(times) if not times else None)
            if parsed is None:
                continue

            t, mass, momentum = parsed
            times.append(float(t))
            masses.append(float(mass))
            momenta.append(float(momentum))

    if not times:
        raise ValueError(f"No valid data found in {path}")

    return times, masses, momenta


def main():
    parser = argparse.ArgumentParser(
        description="Plot mass and momentum magnitude over time."
    )
    parser.add_argument("input_file", type=Path, help="Path to the input file")
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("mass_momentum_vs_time.png"),
        help="Output image path",
    )
    args = parser.parse_args()

    times, masses, momenta = parse_data_file(args.input_file)

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(times, masses, color="tab:blue", marker="o", linewidth=2, markersize=4, label="Mass")
    ax.plot(times, momenta, color="tab:orange", marker="s", linewidth=2, markersize=4, label="Momentum magnitude")

    ax.set_title("Mass and Momentum vs Time")
    ax.set_xlabel("Time")
    ax.set_ylabel("Value")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()

    args.output.parent.mkdir(parents=True, exist_ok=True)
    fig.tight_layout()
    fig.savefig(args.output, dpi=200)
    print(f"Saved plot to {args.output}")


if __name__ == "__main__":
    main()