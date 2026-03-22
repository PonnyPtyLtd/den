# Sprite Style Guide - Ponny's SMS Roguelike

## Reference: Real SMS Pixel Art

Study Golden Axe Warrior, Phantasy Star, Wonder Boy III. These games achieve depth and character at 8x8 and 16x16 through:
- **Consistent light source** (top-left) with highlights AND cast shadows
- **Anti-aliasing**: using mid-tones at edges where dark meets light
- **Dithering**: checkerboard or stripe patterns to blend between tones
- **Sub-pixel detail**: single pixels of accent color that imply detail at smaller-than-pixel scale
- **Volume**: curved surfaces read as 3D through graduated tone bands

## Technical Constraints

- 16x16 pixels, inner 14x14 active (1px transparent border)
- SMS Palette 1 (16 colors, index 0 = transparent)
- 4bpp planar format, stored as TL, BL, TR, BR for 8x16 sprite mode
- Light source: **top-left**

## Palette 1 Colors

| Idx | Hex  | Visual | Role |
|-----|------|--------|------|
| 0   | $00  | Transparent | Border, cutouts |
| 1   | $08  | Dark green | Deep shadow on green things |
| 2   | $0C  | Mid green | Green body fill |
| 3   | $1C  | Bright lime | Green highlight, lit edges |
| 4   | $3F  | White | Eye shine, metal gleam, brightest highlight |
| 5   | $07  | Bright red | Hat, fire, blood, warm accent |
| 6   | $02  | Dark red | Red shadow, deepest warm tone |
| 7   | $19  | Warm brown | Wood, fur, leather base |
| 8   | $1D  | Light tan | Fur/wood highlight, lit brown |
| 9   | $2B  | Vivid pink | Skin, ears, nose, tongue |
| 10  | $18  | Dark olive | Snake shadow, dark foliage |
| 11  | $1C  | Yellow-green | Snake body, mid foliage |
| 12  | $2C  | Bright lime | Snake highlight, lit foliage |
| 13  | $1A  | Warm grey | Stone, metal, orc shadow |
| 14  | $1E  | Bright teal | Orc/cool highlight, gems |
| 15  | $1F  | Bright yellow | Glowing eyes, gold, fire tip |

## Shading Technique (NOT flat fill)

### For any surface, use at minimum 3 tones arranged by light direction:

```
Light from top-left:

  3 3 3 3 2 2 2      ← highlight band (top/left edges)
  3 2 2 2 2 2 1      ← transition row
  2 2 2 2 2 1 1      ← mid-tone with shadow creeping in
  2 2 2 2 1 1 1      ← shadow grows toward bottom-right
  2 2 2 1 1 1 1
  2 2 1 1 1 1 1      ← deepest shadow on bottom-right
```

This is NOT a flat rectangle. Every filled area should have this tonal gradient.

### Dithering for smooth transitions:

Where two tones meet, use a **checkerboard** or **alternating pixel** pattern:

```
  2 2 2 2 2          ← solid mid
  2 1 2 1 2          ← dithered transition
  1 1 1 1 1          ← solid dark
```

This is how SMS games achieved more apparent colors than the palette allowed.

### Anti-aliasing at edges:

Where a character's outline meets transparent background, use the mid-tone (not the darkest tone) at corners to soften the silhouette:

```
  0 0 2 1 1 1 2 0 0    ← mid-tone (2) softens the corner
  0 2 2 2 2 2 2 2 0    ← full body
  0 1 2 2 2 2 2 1 0    ← shadow tone (1) on bottom edge
  0 0 1 1 1 1 1 0 0    ← dark outline at very bottom
```

## Character Design Rules

### Head-heavy (chibi) proportions:
- Head: rows 1-8 (8 rows = ~57%)
- Body+legs: rows 9-13 (5 rows = ~36%)
- Characters should be WIDE — use 12-14 columns

### Eyes are everything:
- Minimum 2x2 pixels per eye
- Structure: dark surround → white fill → dark pupil dot → white catchlight
- Example 3x2 eye: `1 4 4 / 1 0 4` (dark outline, white with pupil, catchlight)

### Outlines:
- Bottom and right: darkest tone (shadow side)
- Top and left: NO outline OR use highlight tone (implies light hitting the edge)
- This creates directionality and avoids the "sticker" look of uniform outlines

## Per-Character Pixel Art Specifications

### FROG (Player)
- **Shape**: Very round head (8 rows), tiny body (4 rows), nub feet
- **Key features**: Red pointed hat (rows 1-3), HUGE eyes (rows 4-5, 3px wide each), wide smile
- **Shading**: Green body uses 1→2→3 gradient from bottom-right to top-left. Hat uses 6→5 gradient.
- **Width**: 12px wide at head, 10px at body

### RAT
- **Shape**: Oval body, prominent round ears poking up, curving tail to the right
- **Key features**: Big pink ears (color 9), dot eyes, visible whisker dots, pink tail
- **Shading**: Brown fur 7→8 gradient, ears have 9 with 7 shadow inside
- **Width**: 12px wide, ears extend to row 1

### SNAKE
- **Shape**: S-curve filling the space, big head at top-left, body curves right then left, thin tail
- **Key features**: Large triangular head, red tongue/fangs (5), prominent eyes
- **Shading**: Olive body 10→11→12 with dithered bands along the body curves
- **Width**: Uses full 14px width with the S-curve

### ORC
- **Shape**: Broad shouldered square, big head, stubby legs spread wide
- **Key features**: Underbite with white tusks (4), angry yellow eyes (15), thick brow
- **Shading**: Teal skin 13→14→4 gradient, dark 13 on arms/sides
- **Width**: 14px at shoulders (fills the space)

### HONEY BADGER (Boss)
- **Shape**: Low and wide, flat skull, stocky body close to ground
- **Key features**: White stripe from forehead across back (color 4), mean yellow eyes (15)
- **Shading**: Dark body 13 with brown 7 on belly/sides, stripe is white with grey shadow
- **Width**: 14px wide, only 10px tall (wider than tall = low/mean)

### DINGO
- **Shape**: Alert upright pose, pointed ears, narrow snout
- **Key features**: Large pointed ears, visible snout/muzzle in profile, bushy tail hint
- **Shading**: Brown 7→8 with pink 9 inside ears and on tongue
- **Width**: 10px body, ears extend silhouette to 12px

### MANUL (Pallas's Cat)
- **Shape**: SPHERE. The roundest character. Almost circular head/body, barely visible legs
- **Key features**: Tiny flat ears, enormous round flat face, striped fur pattern (dithered 13+8)
- **Shading**: Grey/brown mix via dithering 13 and 8, white 4 face patches around eyes
- **Width**: 14px wide, nearly 14px tall (almost fills the entire space)

### POLISH CHICKEN
- **Shape**: Upright tall posture, explosive wild crest/poof on head, tail feathers
- **Key features**: Wild red crest cascading over head (5), yellow beak (15), wing tuck detail
- **Shading**: White body 4 with grey 13 wing shadows, dithered feather texture
- **Width**: 10px body, crest extends to 14px

## Item Sprite Specifications

### Weapons (progress from simple to ornate):
- **Tier 1 (Sharp Stick)**: Diagonal brown stick (7), slight taper, minimal
- **Tier 2 (Lacquer Blade)**: Proper sword shape - white blade (4), red guard (5), brown grip (7)
- **Tier 3 (Bog Oak Mace)**: Heavy flanged head (13+4), thick shaft (7), menacing

### Shields (progress from small to large):
- **Tier 1 (Wooden)**: Small round shield, brown (7+8), simple
- **Tier 2 (Spidershell)**: Medium kite shape, grey (13) with teal (14) boss
- **Tier 3 (Bog Oak)**: Large ornate, grey (13) with gold (15) trim and white (4) emblem

### Armor (progress from vest to full plate):
- **Tier 1 (Reed)**: Simple vest outline, light brown (8), minimal coverage
- **Tier 2 (Spidershell)**: Chest plate with shoulder guards, grey (13+14)
- **Tier 3 (Bog Oak)**: Full plate with trim, grey (13) with gold (15) and white (4) details

### Consumables:
- **Potion**: Round flask, red liquid (5) visible through white glass (4), cork top (7)
- **Wand**: Diagonal wand (7) with fire at tip (5+15 gradient), sparkle pixel (4)
- **Helmet**: Simple cap shape, grey (13) with white (4) highlight band

## Encoding Checklist

Before encoding any sprite:
1. ✅ Does it fill at least 12x12 of 14x14?
2. ✅ Does it have 3-tone shading (dark/mid/light)?
3. ✅ Is there dithering at tone transitions?
4. ✅ Are eyes at least 2x2 with catchlight?
5. ✅ Is the silhouette unique and readable?
6. ✅ Is row 0 and row 14-15 all transparent (border)?
7. ✅ Is column 0 and column 15 all transparent (border)?
8. ✅ Light from top-left, shadow on bottom-right?
