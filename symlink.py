#!/usr/bin/env python3

import argparse
import os
import sys
from collections import Counter
from pathlib import Path

script_path = Path(__file__).resolve()
src_dir = script_path.parent
default_target = Path("/usr/local/include/")

parser = argparse.ArgumentParser(description="将模板库 .hpp 扁平链接到 /usr/local/include/")
parser.add_argument("-d", "--dry-run", action="store_true", help="仅预览，不执行实际操作")
parser.add_argument("-v", "--verbose", action="store_true", help="显示详细信息")
args = parser.parse_args()

if os.geteuid() != 0 and not args.dry_run:
    print("错误: 此脚本需要管理员 (root) 权限才能写入到 /usr/local/include。")
    print(f"请使用 sudo 运行，例如: sudo {sys.argv[0]}")
    sys.exit(1)

hpp_files = sorted(src_dir.rglob("*.hpp"))
if not hpp_files:
    print("未找到任何 .hpp 文件。")
    sys.exit(0)

names = [f.name for f in hpp_files]
dupes = {k for k, v in Counter(names).items() if v > 1}
if dupes:
    print("⚠ 发现同名 .hpp 文件，将只链接第一个：")
    for name in dupes:
        print(f"  {name}:")
        for f in hpp_files:
            if f.name == name:
                print(f"    - {f}")
    print()

seen = set()
linked = 0
skipped = 0
failed = 0

for src in hpp_files:
    name = src.name
    dest = default_target / name

    if name in seen:
        if args.verbose:
            print(f"[跳过] {src} (同名文件已链接)")
        skipped += 1
        continue

    if dest.exists() or dest.is_symlink():
        if args.dry_run:
            print(f"[dry-run] 将删除: {dest}")
        else:
            try:
                dest.unlink()
                if args.verbose:
                    print(f"[删除] {dest}")
            except OSError as e:
                print(f"[失败] 无法删除 {dest}: {e}")
                skipped += 1
                continue

    if args.dry_run:
        print(f"[dry-run] 将链接: {src} -> {dest}")
    else:
        try:
            os.symlink(src, dest)
            if args.verbose:
                print(f"[链接] {src} -> {dest}")
        except OSError as e:
            print(f"[失败] 无法链接 {src} -> {dest}: {e}")
            failed += 1
            continue

    seen.add(name)
    linked += 1

print()
print(f"已链接: {linked}   跳过: {skipped}   失败: {failed}")
