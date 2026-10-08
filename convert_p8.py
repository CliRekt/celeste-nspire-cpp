import re

def parse_p8(filename):
    with open(filename, 'r') as f:
        content = f.read()

    # Extract __gfx__ section (sprites)
    gfx_match = re.search(r'__gfx__\n(.*?)__', content, re.DOTALL)
    gfx_lines = gfx_match.group(1).strip().split('\n') if gfx_match else []

    # Extract __map__ section (tilemap)
    map_match = re.search(r'__map__\n(.*?)__', content, re.DOTALL)
    map_lines = map_match.group(1).strip().split('\n') if map_match else []

    gfx_data = []
    for line in gfx_lines:
        row = [int(c, 16) for c in line.strip()]
        gfx_data.extend(row)

    map_data = []
    for line in map_lines:
        tiles = [int(line[i:i+2], 16) for i in range(0, len(line.strip()), 2)]
        map_data.extend(tiles)

    # Pad map to standard 128x32 tile dimensions (4096 bytes)
    while len(map_data) < 4096:
        map_data.append(0)

    # Pad GFX to standard 128x128 pixel dimensions (16384 bytes)
    while len(gfx_data) < 16384:
        gfx_data.append(0)

    # Output generated game_data.h
    with open('game_data.h', 'w') as f:
        f.write('#ifndef GAME_DATA_H\n#define GAME_DATA_H\n\n')
        f.write('#include <stdint.h>\n\n')
        
        f.write('const uint8_t CELESTE_MAP[4096] = {\n    ')
        f.write(', '.join(map(str, map_data)))
        f.write('\n};\n\n')

        f.write('const uint8_t CELESTE_GFX[16384] = {\n    ')
        f.write(', '.join(map(str, gfx_data)))
        f.write('\n};\n\n')

        f.write('#endif // GAME_DATA_H\n')

if __name__ == '__main__':
    parse_p8('celeste.p8')
    print("Generated game_data.h successfully.")
