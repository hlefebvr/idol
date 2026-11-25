#!/usr/bin/env python3
"""Check or synchronize the portable Claude copies of canonical idol skills."""
import argparse
from pathlib import Path
import re
import shutil
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sync', action='store_true', help='Copy canonical skills to Claude; never delete extra skills.')
    args = parser.parse_args()
    canonical = ROOT / '.agents' / 'skills'
    claude = ROOT / '.claude' / 'skills'
    sources = {p.parent.name: p for p in canonical.glob('*/SKILL.md')}
    copies = {p.parent.name: p for p in claude.glob('*/SKILL.md')}
    errors = []
    if not sources:
        errors.append('No canonical skills found.')
    for name, source in sorted(sources.items()):
        content = source.read_text(encoding='utf-8')
        match = re.match(r'\A---\nname: ([a-z0-9-]+)\ndescription: (\S[^\n]*)\n---\n', content)
        if not match or match.group(1) != name:
            errors.append(f'Invalid skill frontmatter: {source.relative_to(ROOT)}')
            continue
        destination = claude / name / 'SKILL.md'
        if not destination.exists() or destination.read_bytes() != source.read_bytes():
            if args.sync:
                destination.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(source, destination)
                print(f'Synced {name}')
            else:
                errors.append(f'Missing or divergent Claude copy: {name}')
    for name in sorted(copies.keys() - sources.keys()):
        errors.append(f'Extra Claude skill (not deleted): {name}')
    for error in errors:
        print(error, file=sys.stderr)
    if errors:
        return 1
    print(f'OK: {len(sources)} canonical skills and matching Claude copies.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
