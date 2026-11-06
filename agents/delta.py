#!/usr/bin/env python3
import argparse
import csv
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
PIN = "3140648a668aaebf87bae2dea17512fdca125ba0"
UPSTREAM = Path("/Volumes/Hangar/ps5/upstream/Eglantine")
TEST_WEIGHT = 0.25
AGGREGATE = 40

SUBSYSTEMS = {
    "shaders": ("src/shader_recompiler/",),
    "gpu-vulkan": ("src/video_core/renderer_vulkan/", "src/video_core/vulkan_common/", "externals/FidelityFX-SDK/"),
    "gpu-opengl": ("src/video_core/renderer_opengl/",),
    "gpu-core": ("src/video_core/",),
    "cpu-jit": ("src/dynarmic/", "src/core/arm/"),
    "kernel-memory": ("src/core/hle/kernel/", "src/core/memory", "src/core/device_memory", "src/core/core_timing",
                      "src/core/cpu_manager", "src/core/core.cpp", "src/core/core.h"),
    "services": ("src/core/hle/service/",),
    "audio": ("src/audio_core/",),
    "loader-fs": ("src/core/loader/", "src/core/file_sys/", "src/core/crypto/"),
    "input": ("src/hid_core/", "src/input_common/"),
    "common": ("src/common/",),
    "core": ("src/core/",),
}
OTHER = "frontend-build"
ABOUT = {
    "shaders": "the shader recompiler: Maxwell/Ampere instructions to IR to SPIR-V/GLSL",
    "gpu-vulkan": "the Vulkan renderer (RADV on the PS5), its scheduler, pipelines and upscalers",
    "gpu-opengl": "the OpenGL renderer (the PS5 port's fallback)",
    "gpu-core": "the GPU core: texture and buffer caches, engines, host1x, memory manager, host shaders",
    "cpu-jit": "dynarmic and the ARM interface: on the PS5, ARM64 recompiled to x86-64",
    "kernel-memory": "the HLE kernel, guest memory, timing and the CPU manager",
    "services": "HLE services",
    "audio": "the audio core",
    "loader-fs": "loaders, file systems and crypto",
    "input": "HID and input",
    "common": "src/common",
    "core": "the rest of src/core",
    OTHER: "frontends (Qt, Android, CLI), build, CI, docs and other externals: mostly not used by the PS5 port",
}
HEADER = "| Unit | Date | Title | Size | Also touches | Diff | Verdict | Why |"


def git(*args: str) -> str:
    return subprocess.run(["git", "-C", str(UPSTREAM), *args], check=True, capture_output=True, text=True).stdout


def is_ancestor(commit: str, ref: str) -> bool:
    return subprocess.run(["git", "-C", str(UPSTREAM), "merge-base", "--is-ancestor", commit, ref]).returncode == 0


def subsystem(path: str) -> tuple[str, float]:
    weight = 1.0
    if path.startswith("src/tests/"):
        path, weight = "src/" + path[len("src/tests/"):], TEST_WEIGHT
    for name, prefixes in SUBSYSTEMS.items():
        if path.startswith(prefixes):
            return name, weight
    return OTHER, weight


def unit_of(sha: str, source: str) -> dict:
    prefix = "dj" if source == "eglantine" else "li"
    parents = git("rev-list", "--parents", "-n1", sha).split()[1:]
    date, author, subject, body = git("show", "-s", "--format=%cs%x00%an%x00%s%x00%b", sha).split("\x00")
    base = parents[0]
    pr = re.match(r"Merge pull request #(\d+) from (\S+)", subject)
    squash = re.search(r"\(#(\d+)\)\s*$", subject)
    if pr:
        unit, title = f"{prefix}#{pr.group(1)}", (body.strip().splitlines() or [pr.group(2)])[0]
    elif squash:
        unit, title = f"{prefix}#{squash.group(1)}", subject[: squash.start()].strip()
    else:
        unit, title = f"{prefix}:{sha[:10]}", subject
    commits = int(git("rev-list", "--no-merges", "--count", f"{base}..{sha}"))
    lines: dict[str, float] = defaultdict(float)
    added = removed = files = 0
    for row in git("diff", "--numstat", base, sha).splitlines():
        a, r, path = row.split("\t", 2)
        files += 1
        if a == "-":
            continue
        added, removed = added + int(a), removed + int(r)
        name, weight = subsystem(path.split(" => ")[-1].replace("}", "").replace("{", ""))
        lines[name] += (int(a) + int(r)) * weight
    ranked = sorted(lines, key=lines.get, reverse=True) or [OTHER]
    return {
        "unit": unit, "source": source, "date": date, "author": author, "title": title.replace("|", "/"),
        "commits": commits, "files": files, "added": added, "removed": removed,
        "primary": ranked[0], "subsystems": ",".join(ranked), "base": base, "tip": sha,
    }


def walk(tip: str, excludes: list[str], source: str, seen: set[str], found: list[dict]) -> None:
    for sha in git("rev-list", "--first-parent", "--reverse", tip, *(f"^{e}" for e in excludes)).split():
        if sha in seen:
            continue
        seen.add(sha)
        parents = git("rev-list", "--parents", "-n1", sha).split()[1:]
        subject = git("show", "-s", "--format=%s", sha).strip()
        if len(parents) > 1:
            inner = [*excludes, parents[0]]
            fresh = int(git("rev-list", "--no-merges", "--count", parents[1], *(f"^{e}" for e in inner)))
            total = int(git("rev-list", "--no-merges", "--count", parents[1], f"^{parents[0]}", f"^{PIN}"))
            pure_pr = subject.startswith("Merge pull request #") and fresh == total and fresh <= AGGREGATE
            if not pure_pr:
                for side in parents[1:]:
                    walk(side, inner, source, seen, found)
                continue
        found.append(unit_of(sha, source))


def units() -> list[dict]:
    found: list[dict] = []
    seen: set[str] = set()
    walk("djihads80/nx2", [PIN], "eglantine", seen, found)
    walk("lichii/nx2", [PIN, "djihads80/nx2"], "lichii", seen, found)
    return found


def write_tsv(rows: list[dict], path: Path) -> None:
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t", lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def row(u: dict) -> str:
    also = ", ".join(u["subsystems"].split(",")[1:]) or "-"
    size = f"+{u['added']} -{u['removed']}, {u['files']} files, {u['commits']} commits"
    return f"| {u['unit']} | {u['date']} | {u['title']} | {size} | {also} | `{u['base'][:10]}..{u['tip'][:10]}` | | |"


def write_subsystem(name: str, rows: list[dict], folder: Path) -> int:
    path = folder / f"{name}.md"
    if path.exists():
        text = path.read_text()
        known = set(re.findall(r"^\| ((?:dj|li)[#:][0-9a-f]+) \|", text, re.M))
        new = [u for u in rows if u["unit"] not in known]
        if new:
            path.write_text(text.rstrip("\n") + "\n" + "\n".join(row(u) for u in new) + "\n")
        return len(new)
    prefixes = ", ".join(f"`{p}`" for p in SUBSYSTEMS.get(name, ())) or "everything not listed in the other files"
    lines = [
        f"# {name}",
        "",
        f"Units of the upstream delta whose largest share of changed lines is in {ABOUT[name]} ({prefixes}). "
        "Writer: the agent assigned to this file; nobody else edits it. Verdicts and how to read a unit: [README](../README.md).",
        "",
        HEADER,
        "|---|---|---|---|---|---|---|---|",
        *(row(u) for u in rows),
        "",
    ]
    path.write_text("\n".join(lines))
    return len(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description="List the upstream Eglantine and Lichii changes the port's core does not have yet.")
    parser.add_argument("--fetch", action="store_true", help="fetch both remotes first")
    args = parser.parse_args()
    if args.fetch:
        git("fetch", "--quiet", "--all")
    rows = units()
    folder = HERE / "delta"
    folder.mkdir(exist_ok=True)
    write_tsv(rows, folder / "units.tsv")
    by: dict[str, list[dict]] = defaultdict(list)
    for u in rows:
        by[u["primary"]].append(u)
    for name in [*SUBSYSTEMS, OTHER]:
        if by.get(name):
            added = write_subsystem(name, by[name], folder)
            print(f"{name}: {len(by[name])} units, {added} new")
    return 0


if __name__ == "__main__":
    sys.exit(main())
