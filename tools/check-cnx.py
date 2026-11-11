#!/usr/bin/env python3
import os
from pathlib import Path
import subprocess
import tempfile
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    compiler = os.environ.get("HOST_CXX", "c++")
    c_compiler = os.environ.get("HOST_CC", "cc")
    includes = [f"-I{ROOT / 'headless'}", f"-I{ROOT / 'third_party/miniz'}"]
    if os.environ.get("JSON_INCLUDE"):
        includes.append(f"-I{os.environ['JSON_INCLUDE']}")
    else:
        for base in (ROOT / ".deps", Path("/opt/homebrew/include"),
                     ROOT.parents[2] / ".deps", *Path.home().glob(".cache/ps5-eden-headless.*")):
            if (base / "nlohmann/json.hpp").is_file():
                includes.append(f"-I{base}")
                break
            if base.is_dir():
                header = next(base.glob("**/single_include/nlohmann/json.hpp"), None)
                if header:
                    includes.append(f"-I{header.parent.parent}")
                    break
    with tempfile.TemporaryDirectory(prefix="cnx-check-") as temporary:
        work = Path(temporary)
        objects = []
        for name in ("miniz", "miniz_tdef", "miniz_tinfl", "miniz_zip"):
            target = work / f"{name}.o"
            subprocess.run([c_compiler, "-std=c11", "-O1", "-w", "-c",
                            str(ROOT / f"third_party/miniz/{name}.c"), "-o", str(target)], check=True)
            objects.append(str(target))
        binary = work / "cnx-check"
        subprocess.run([compiler, "-std=c++20", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
                        *includes, str(ROOT / "headless/cnx.cpp"), str(ROOT / "headless/cnx_check.cpp"),
                        *objects, "-o", str(binary)], check=True)
        subprocess.run([str(binary), *sys.argv[1:]], check=True)


if __name__ == "__main__":
    main()
