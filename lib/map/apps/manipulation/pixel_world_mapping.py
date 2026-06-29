#!/usr/bin/env python3
"""
Pixel <-> world coordinate mapping for the UFO top-down renderer.

The renderer shoots vertical rays (direction [0,0,-1]) from four corner
world coordinates, bilinearly interpolated across the image grid.
Ray origin x,y == hit x,y since the direction is straight down.
"""

import argparse
import tomllib
from pathlib import Path

import numpy as np


def load_corners_from_config(config_path: str) -> dict:
    with open(config_path, "rb") as f:
        cfg = tomllib.load(f)
    r = cfg["renderer"]
    return {
        "top_left":     tuple(r["top_left"]),
        "top_right":    tuple(r["top_right"]),
        "bottom_left":  tuple(r["bottom_left"]),
        "bottom_right": tuple(r["bottom_right"]),
        "width":        r["width"],
        "height":       r["height"],
        "elevation":    r["elevation"],
        "far_clip":     r["far_clip"],
    }


def pixel_to_world(col: float, row: float, corners: dict) -> tuple[float, float]:
    """
    Map image pixel (col, row) to world (x, y).

    Matches the bilinear interpolation in Renderer::Renderer() exactly:
        top    = lerp(TL, TR, col / W)
        bottom = lerp(BL, BR, col / W)
        origin = lerp(top, bottom, row / H)
    """
    W = corners["width"]
    H = corners["height"]
    TL = corners["top_left"]
    TR = corners["top_right"]
    BL = corners["bottom_left"]
    BR = corners["bottom_right"]

    u = col / W
    v = row / H

    x = (1 - v) * (1 - u) * TL[0] + (1 - v) * u * TR[0] + v * (1 - u) * BL[0] + v * u * BR[0]
    y = (1 - v) * (1 - u) * TL[1] + (1 - v) * u * TR[1] + v * (1 - u) * BL[1] + v * u * BR[1]
    return x, y


def world_to_pixel(wx: float, wy: float, corners: dict) -> tuple[float, float]:
    """
    Map world (x, y) to image pixel (col, row).

    Solves the bilinear system numerically via Newton iteration.
    The quad is nearly rectangular so this converges in 2-3 steps.
    Returns float pixel coordinates (not rounded).
    """
    W = corners["width"]
    H = corners["height"]
    TL = np.array(corners["top_left"])
    TR = np.array(corners["top_right"])
    BL = np.array(corners["bottom_left"])
    BR = np.array(corners["bottom_right"])
    target = np.array([wx, wy])

    # Bilinear position and its Jacobian wrt (u, v)
    def f_and_J(u, v):
        pos = (1 - v) * (1 - u) * TL + (1 - v) * u * TR + v * (1 - u) * BL + v * u * BR
        dp_du = (1 - v) * (TR - TL) + v * (BR - BL)
        dp_dv = (1 - u) * (BL - TL) + u * (BR - TR)
        return pos, np.column_stack([dp_du, dp_dv])

    # Initial guess: affine approximation
    u, v = 0.5, 0.5
    for _ in range(20):
        pos, J = f_and_J(u, v)
        residual = pos - target
        if np.linalg.norm(residual) < 1e-9:
            break
        delta = np.linalg.solve(J, residual)
        u -= delta[0]
        v -= delta[1]

    return u * W, v * H


def run_tests(corners: dict) -> None:
    W = corners["width"]
    H = corners["height"]
    TL = corners["top_left"]
    TR = corners["top_right"]
    BL = corners["bottom_left"]
    BR = corners["bottom_right"]

    print("=" * 60)
    print("Corner mapping (pixel → world)")
    print("=" * 60)
    cases = [
        ("top-left  pixel  (0,0)",      0,     0,     TL),
        ("top-right pixel  (W,0)",      W,     0,     TR),
        ("bot-left  pixel  (0,H)",      0,     H,     BL),
        ("bot-right pixel  (W,H)",      W,     H,     BR),
        ("center    pixel  (W/2,H/2)",  W / 2, H / 2, None),
    ]
    for label, col, row, expected in cases:
        x, y = pixel_to_world(col, row, corners)
        exp_str = f"expected ({expected[0]:.4f}, {expected[1]:.4f})" if expected else "no expected"
        print(f"  {label:38s} → world ({x:.4f}, {y:.4f})   {exp_str}")

    print()
    print("=" * 60)
    print("Round-trip: pixel → world → pixel")
    print("=" * 60)
    test_pixels = [
        (0, 0), (W - 1, 0), (0, H - 1), (W - 1, H - 1),
        (W / 2, H / 2), (W / 4, H / 4), (3 * W / 4, 3 * H / 4),
    ]
    max_err = 0.0
    for col, row in test_pixels:
        wx, wy = pixel_to_world(col, row, corners)
        col2, row2 = world_to_pixel(wx, wy, corners)
        err = np.hypot(col2 - col, row2 - row)
        max_err = max(max_err, err)
        status = "OK" if err < 0.01 else "FAIL"
        print(f"  pixel ({col:6.1f}, {row:6.1f}) → world ({wx:.4f}, {wy:.4f}) "
              f"→ pixel ({col2:.4f}, {row2:.4f})  err={err:.2e}  [{status}]")
    print(f"\n  Max round-trip error: {max_err:.2e} pixels")

    print()
    print("=" * 60)
    print("World → pixel spot checks")
    print("=" * 60)
    world_pts = [
        TL, TR, BL, BR,
        ((TL[0] + TR[0] + BL[0] + BR[0]) / 4,
         (TL[1] + TR[1] + BL[1] + BR[1]) / 4),
    ]
    labels = ["TL corner", "TR corner", "BL corner", "BR corner", "centroid"]
    for (wx, wy), lbl in zip(world_pts, labels):
        col, row = world_to_pixel(wx, wy, corners)
        print(f"  {lbl:12s} world ({wx:.3f}, {wy:.3f}) → pixel ({col:.2f}, {row:.2f})")


def interactive(corners: dict) -> None:
    print("Enter pixel coordinates to convert (ctrl-c to quit).")
    print(f"Image size: {corners['width']} x {corners['height']}\n")
    while True:
        try:
            raw = input("  col row (or 'w x y' for world→pixel): ").strip()
        except (KeyboardInterrupt, EOFError):
            print()
            break
        parts = raw.split()
        if not parts:
            continue
        try:
            if parts[0] == "w":
                wx, wy = float(parts[1]), float(parts[2])
                col, row = world_to_pixel(wx, wy, corners)
                print(f"    world ({wx}, {wy}) → pixel ({col:.2f}, {row:.2f})\n")
            else:
                col, row = float(parts[0]), float(parts[1])
                wx, wy = pixel_to_world(col, row, corners)
                print(f"    pixel ({col}, {row}) → world ({wx:.6f}, {wy:.6f})\n")
        except (ValueError, IndexError) as e:
            print(f"    Error: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Pixel <-> world coordinate mapping tester")
    parser.add_argument("config", nargs="?", default="config.toml",
                        help="Path to config.toml (default: ./config.toml)")
    parser.add_argument("--interactive", "-i", action="store_true",
                        help="Launch interactive prompt after running tests")
    parser.add_argument("--pixel", "-p", nargs=2, type=float, metavar=("ROW", "COL"),
                        help="Convert a single pixel (row col) to world x,y and save as .npy")
    parser.add_argument("--out", "-o", default="world_pose.npy",
                        help="Output .npy file path when using --pixel (default: world_pose.npy)")
    args = parser.parse_args()

    corners = load_corners_from_config(args.config)

    if args.pixel is not None:
        row, col = args.pixel
        wx, wy = pixel_to_world(col, row, corners)
        pose = np.array([wx, wy])
        np.save(args.out, pose)
        print(f"pixel ({col}, {row}) → world ({wx:.6f}, {wy:.6f})")
        print(f"Saved to {args.out}")
    else:
        print(f"Loaded corners from {args.config}")
        print(f"  top_left={corners['top_left']}  top_right={corners['top_right']}")
        print(f"  bottom_left={corners['bottom_left']}  bottom_right={corners['bottom_right']}")
        print(f"  image={corners['width']}x{corners['height']}  "
              f"elevation={corners['elevation']}m  far_clip={corners['far_clip']}m\n")

        run_tests(corners)

        if args.interactive:
            print()
            interactive(corners)
