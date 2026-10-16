# Turns an art file like art/mopmania_sprites_v3.txt into game/CharacterArtData.cpp.
# Usage: python3 tools/gen_art.py art/mopmania_sprites_v3.txt game/CharacterArtData.cpp
# Fails if any art line is over 80 columns.
import re, sys
src, out_cpp = sys.argv[1], sys.argv[2]
lines = open(src, encoding='utf-8').read().split('\n')
sprites = []  # (char, size, view, note, art_lines)
i = 0
while i < len(lines):
    if lines[i].startswith('-----') and i + 2 < len(lines) and lines[i+2].startswith('-----'):
        m = re.match(r'\s*(BROOMBA|SWIFTER JETWET|BYSONV8) - (small|large) - (front|back|attack)(?: \((.*)\))?\s*$', lines[i+1])
        assert m, lines[i+1]
        j = i + 3
        body = []
        while j < len(lines) and not lines[j].startswith('-----') and not lines[j].startswith('====='):
            body.append(lines[j].rstrip())
            j += 1
        while body and body[-1] == '': body.pop()
        while body and body[0] == '': body.pop(0)
        sprites.append((m.group(1), m.group(2), m.group(3), m.group(4), body))
        i = j
    else:
        i += 1
assert len(sprites) == 18, len(sprites)
names = {'BROOMBA': 'BROOMBA', 'SWIFTER JETWET': 'JETWET', 'BYSONV8': 'BYSON'}
out = ['// Ultra Deluxe: character art from mopmania_sprites_v3.txt.',
       '// Small = combat sprites, large = character select. Back views are kept',
       '// for later (animation). Generated from the art file; every line fits in',
       '// 80 columns, and raw string literals keep the backslashes as drawn.',
       '#include "CharacterArt.h"',
       '']
widest = 0
for ch, size, view, note, body in sprites:
    for l in body:
        assert ')ART"' not in l
        widest = max(widest, len(l))
        assert len(l) <= 80, (ch, size, view, len(l), l)
    var = f'ART_{names[ch]}_{size.upper()}_{view.upper()}'
    if note: out.append(f'// {note}')
    out.append(f'const char* const {var} = R"ART(')
    out.extend(body)
    out.append(')ART";')
    out.append('')
code = '\n'.join(out)
for n, l in enumerate(code.split('\n'), 1):
    assert len(l) <= 80, (n, len(l))
open(out_cpp, 'w', encoding='utf-8').write(code)
print(f'{len(sprites)} pieces, widest art line {widest} columns')
