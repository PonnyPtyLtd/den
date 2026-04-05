#!/usr/bin/env ruby
# Imports tile data from BMPs back into tiles.inc and font.inc
# BMPs must match the layout produced by tile_export.rb
# Usage: ruby tools/tile_import.rb [bg_tiles.bmp] [sprites.bmp] [font.bmp]
# Any argument can be "-" to skip that import.

TILES_FILE = File.join(__dir__, '..', 'src', 'data', 'tiles.inc')
FONT_FILE  = File.join(__dir__, '..', 'src', 'data', 'font.inc')
VDP_FILE   = File.join(__dir__, '..', 'src', 'vdp.inc')
SCALE  = 1
MARGIN = 1

# Layouts must match tile_export.rb exactly
BG_LAYOUT = [
  [
    [:bg8, 'WallTLData'],
    [:bg8, 'WallTRData'],
    [:bg8, 'FloorTileData'],
    [:bg8, 'SolidBlackData'],
  ],
  [
    [:bg16, 'StairsTLData', 'StairsTRData', 'StairsBLData', 'StairsBRData'],
    [:bg16, 'IconInvTL', 'IconInvTR', 'IconInvBL', 'IconInvBR'],
    [:bg16, 'IconPickTL', 'IconPickTR', 'IconPickBL', 'IconPickBR'],
    [:bg16, 'IconWaitTL', 'IconWaitTR', 'IconWaitBL', 'IconWaitBR'],
    [:bg16, 'IconXTL', 'IconXTR', 'IconXBL', 'IconXBR'],
  ],
  [
    [:bg8, 'RootTLData'],
    [:bg8, 'RootTData'],
    [:bg8, 'RootTRData'],
    [:bg8, 'RootT2Data'],
    [:gap],
    [:bg8, 'RootLData'],
    [:bg8, 'RootRData'],
    [:bg8, 'RootL2Data'],
    [:bg8, 'RootR2Data'],
    [:gap],
    [:bg8, 'RootBLData'],
    [:bg8, 'RootBData'],
    [:bg8, 'RootBRData'],
    [:bg8, 'RootB2Data'],
  ],
  [
    [:bg8, 'DirtSolidData'],
    [:bg8, 'DirtSolidV2Data'],
    [:bg8, 'DirtEdgeTData'],
    [:bg8, 'DirtEdgeT2Data'],
    [:bg8, 'DirtEdgeLData'],
    [:bg8, 'DirtEdgeL2Data'],
    [:bg8, 'DirtCornerTLData'],
  ],
  [
    [:bg8, 'GreebleCrackData'],
    [:bg8, 'GreebleBoneData'],
    [:bg8, 'GreebleSkullData'],
    [:bg8, 'GreebleMoleData'],
    [:bg8, 'GreebleRoot1Data'],
    [:bg8, 'GreebleRoot2Data'],
    [:bg8, 'GreebleRoot3Data'],
    [:gap],
    [:bg8, 'HeartFullData'],
    [:bg8, 'Heart34Data'],
    [:bg8, 'HeartHalfData'],
    [:bg8, 'Heart14Data'],
    [:bg8, 'HeartEmptyData'],
  ],
]

SPR_LAYOUT = [
  [
    [:spr, 'PlayerSpr1Data'],
    [:spr, 'PlayerSpr2Data'],
    [:spr, 'RatSprData'],
    [:spr, 'SnakeSprData'],
    [:spr, 'OrcSprData'],
    [:spr, 'WeaponSprData'],
    [:spr, 'ArmorSprData'],
    [:spr, 'BadgerSprData'],
  ],
  [
    [:spr, 'BodyArmorSprData'],
    [:spr, 'PotionSprData'],
    [:spr, 'WandSprData'],
    [:spr, 'Weapon1SprData'],
    [:spr, 'Weapon2SprData'],
    [:spr, 'Weapon3SprData'],
    [:spr, 'Shield1SprData'],
    [:spr, 'Shield2SprData'],
  ],
  [
    [:spr, 'Shield3SprData'],
    [:spr, 'Armor1SprData'],
    [:spr, 'Armor2SprData'],
    [:spr, 'Armor3SprData'],
    [:spr, 'DingoSpr1Data'],
    [:spr, 'DingoSpr2Data'],
    [:spr, 'ManulSpr1Data'],
    [:spr, 'ManulSpr2Data'],
  ],
  [
    [:spr, 'ChickenSpr1Data'],
    [:spr, 'ChickenSpr2Data'],
    [:spr, 'FireballSprData'],
  ],
]

def labels_from_layout(layout)
  layout.flat_map { |row|
    row.flat_map { |item|
      case item[0]
      when :bg8  then [item[1]]
      when :bg16 then item[1..4]
      when :spr  then [item[1]]
      else []
      end
    }
  }
end

ALL_LABELS = (labels_from_layout(BG_LAYOUT) + labels_from_layout(SPR_LAYOUT)).freeze

# Simple BMP reader (24/32-bit uncompressed)
class BMPReader
  attr_reader :width, :height

  def initialize(path)
    data = File.binread(path)
    raise "Not a BMP file" unless data[0..1] == 'BM'
    offset = data[10..13].unpack1('V')
    @width = data[18..21].unpack1('l<')
    @height = data[22..25].unpack1('l<').abs
    bpp = data[28..29].unpack1('v')
    raise "Only 24/32-bit BMP supported (got #{bpp})" unless [24, 32].include?(bpp)
    bytes_per_pixel = bpp / 8
    bottom_up = data[22..25].unpack1('l<') > 0
    row_bytes = @width * bytes_per_pixel
    row_pad = (4 - row_bytes % 4) % 4
    @pixels = Array.new(@height) { Array.new(@width, [0, 0, 0]) }
    @height.times do |y|
      src_y = bottom_up ? (@height - 1 - y) : y
      row_offset = offset + src_y * (row_bytes + row_pad)
      @width.times do |x|
        i = row_offset + x * bytes_per_pixel
        b, g, r = data[i].ord, data[i + 1].ord, data[i + 2].ord
        @pixels[y][x] = [r, g, b]
      end
    end
  end

  def get(x, y)
    return [0, 0, 0] if x < 0 || x >= @width || y < 0 || y >= @height
    @pixels[y][x]
  end
end

def sms_to_rgb(byte)
  [(byte & 0x03) * 85, ((byte >> 2) & 0x03) * 85, ((byte >> 4) & 0x03) * 85]
end

def parse_palette(file)
  palettes = []
  File.readlines(file).each do |line|
    next unless line =~ /^\.db\s+(.+)/
    bytes = $1.split(',').map { |b| b.strip.sub(/;.*/, '').strip }
                         .reject(&:empty?)
                         .map { |b| b.start_with?('$') ? b[1..].to_i(16) : b.to_i }
    palettes << bytes if bytes.length == 16
  end
  palettes
end

def nearest_color(r, g, b, palette_rgb)
  best = 0
  best_d = Float::INFINITY
  palette_rgb.each_with_index do |(pr, pg, pb), i|
    d = (r - pr) ** 2 + (g - pg) ** 2 + (b - pb) ** 2
    best, best_d = i, d if d < best_d
  end
  best
end

def encode_tile(pixels)
  bytes = []
  8.times do |row|
    bp = [0, 0, 0, 0]
    8.times do |col|
      bit = 7 - col
      ci = pixels[row][col]
      4.times { |p| bp[p] |= ((ci >> p) & 1) << bit }
    end
    bytes.concat(bp)
  end
  bytes
end

# Read an 8x8 tile from BMP at pixel position, map to palette indices
def read_tile(bmp, palette_rgb, x, y, transparent_bg: false)
  Array.new(8) { |py| Array.new(8) { |px|
    r, g, b = bmp.get(x + px * SCALE + SCALE / 2, y + py * SCALE + SCALE / 2)
    ci = nearest_color(r, g, b, palette_rgb)
    ci = 0 if transparent_bg && r < 48 && g < 48 && b < 48
    ci
  }}
end

# Compute placements from a layout → [{label:, x:, y:, type:}, ...] + dimensions
def compute_layout(layout)
  placements = []
  cur_y = MARGIN
  img_w = 0

  layout.each do |row|
    row_h = row.any? { |item| item[0] == :bg16 || item[0] == :spr } ? 16 * SCALE : 8 * SCALE
    cur_x = MARGIN

    row.each do |item|
      case item[0]
      when :bg8
        placements << { label: item[1], x: cur_x, y: cur_y, type: :bg8 }
        cur_x += 8 * SCALE + MARGIN
      when :bg16
        placements << { label: item[1], x: cur_x,              y: cur_y,              type: :bg8 }
        placements << { label: item[2], x: cur_x + 8 * SCALE,  y: cur_y,              type: :bg8 }
        placements << { label: item[3], x: cur_x,              y: cur_y + 8 * SCALE,  type: :bg8 }
        placements << { label: item[4], x: cur_x + 8 * SCALE,  y: cur_y + 8 * SCALE,  type: :bg8 }
        cur_x += 16 * SCALE + MARGIN
      when :spr
        placements << { label: item[1], x: cur_x, y: cur_y, type: :spr }
        cur_x += 16 * SCALE + MARGIN
      when :gap
        cur_x += 4
      end
    end

    img_w = [img_w, cur_x].max
    cur_y += row_h + MARGIN
  end

  [placements, img_w, cur_y]
end

# Parse tile blocks from tiles.inc, tracking .db line positions per label
def parse_tile_blocks(file, labels)
  label_set = labels.to_a
  blocks = {}
  current_label = nil
  current_bytes = []
  db_lines = []
  lines = File.readlines(file)

  lines.each_with_index do |line, idx|
    stripped = line.strip
    if stripped =~ /^(\w+):/
      name = $1
      if label_set.include?(name)
        if current_label && current_bytes.length >= 32
          blocks[current_label] = { bytes: current_bytes.dup, db_lines: db_lines.dup }
        end
        current_label = name
        current_bytes = []
        db_lines = []
      end
    end
    if current_label && stripped =~ /^\.db\s+(.+)/
      data_part = $1.sub(/;.*/, '').strip
      tokens = data_part.split(',').map(&:strip)
      if tokens.all? { |b| b =~ /^\$[0-9a-fA-F]+$/ || b =~ /^\d+$/ }
        db_lines << idx
        bs = tokens.map { |b| b.start_with?('$') ? b[1..].to_i(16) : b.to_i }
        current_bytes.concat(bs)
      end
    end
  end
  if current_label && current_bytes.length >= 32
    blocks[current_label] = { bytes: current_bytes.dup, db_lines: db_lines.dup }
  end

  [lines, blocks]
end

# Replace .db lines for a block with new encoded bytes
def replace_db_lines(lines, block, new_bytes)
  byte_idx = 0
  block[:db_lines].each do |li|
    orig = lines[li]
    n_in_line = orig.sub(/^.*\.db\s+/, '').sub(/;.*/, '').strip
                    .split(',').count { |b| b.strip =~ /^\$?[0-9a-fA-F]+$/ }
    comment = orig =~ /(;.*)/ ? "  #{$1.rstrip}" : ""
    indent = orig[/^\s*/]
    hex = new_bytes[byte_idx, n_in_line].map { |b| "$%02x" % b }
    lines[li] = "#{indent}.db #{hex.join(',')}#{comment}\n"
    byte_idx += n_in_line
  end
end

# Import BG tiles from BMP
def import_bg(bmp_path, pal0_rgb, tiles_file)
  return 0 unless bmp_path && bmp_path != "-" && File.exist?(bmp_path)

  bmp = BMPReader.new(bmp_path)
  placements, _, _ = compute_layout(BG_LAYOUT)
  lines, blocks = parse_tile_blocks(tiles_file, ALL_LABELS)

  # Color 0 rendered as dark grey in export to distinguish from color 5
  bg_pal = pal0_rgb.dup
  bg_pal[0] = [16, 16, 16] if bg_pal[0] == [0, 0, 0]

  changes = 0
  placements.each do |p|
    block = blocks[p[:label]]
    next unless block
    pixels = read_tile(bmp, bg_pal, p[:x], p[:y])
    new_bytes = encode_tile(pixels)
    if new_bytes.length == block[:bytes].length
      replace_db_lines(lines, block, new_bytes)
      changes += 1
    end
  end

  File.write(tiles_file, lines.join)
  puts "Imported #{changes} BG tile blocks from #{bmp_path}"
  changes
end

# Import sprites from BMP
def import_sprites(bmp_path, pal1_rgb, tiles_file)
  return 0 unless bmp_path && bmp_path != "-" && File.exist?(bmp_path)

  bmp = BMPReader.new(bmp_path)
  placements, _, _ = compute_layout(SPR_LAYOUT)
  lines, blocks = parse_tile_blocks(tiles_file, ALL_LABELS)

  changes = 0
  placements.each do |p|
    block = blocks[p[:label]]
    next unless block

    # Read 16x16 sprite, split into TL/BL/TR/BR quadrants
    full = Array.new(16) { |py| Array.new(16) { |px|
      r, g, b = bmp.get(p[:x] + px * SCALE + SCALE / 2, p[:y] + py * SCALE + SCALE / 2)
      ci = nearest_color(r, g, b, pal1_rgb)
      ci = 0 if r < 48 && g < 48 && b < 48
      ci
    }}
    tl = full[0..7].map { |r| r[0..7] }
    bl = full[8..15].map { |r| r[0..7] }
    tr = full[0..7].map { |r| r[8..15] }
    br = full[8..15].map { |r| r[8..15] }
    new_bytes = []
    [tl, bl, tr, br].each { |t| new_bytes.concat(encode_tile(t)) }
    if new_bytes.length == block[:bytes].length
      replace_db_lines(lines, block, new_bytes)
      changes += 1
    end
  end

  File.write(tiles_file, lines.join)
  puts "Imported #{changes} sprite blocks from #{bmp_path}"
  changes
end

# Import font
def import_font(bmp_path, palette_rgb, font_file)
  return 0 unless bmp_path && bmp_path != "-" && File.exist?(bmp_path)
  bmp = BMPReader.new(bmp_path)

  lines = File.readlines(font_file)
  db_lines = []
  all_bytes = []
  in_font = false

  lines.each_with_index do |line, idx|
    stripped = line.strip
    if stripped =~ /^FontData:/
      in_font = true
      next
    end
    if stripped =~ /^FontDataEnd:/ || (in_font && stripped =~ /^\w+:/ && stripped !~ /^\.db/)
      break
    end
    if in_font && stripped =~ /^\.db\s+(.+)/
      db_lines << idx
      data_part = $1.sub(/;.*/, '').strip
      bs = data_part.split(',').map { |b|
        b = b.strip
        b.start_with?('$') ? b[1..].to_i(16) : (b =~ /^\d+$/ ? b.to_i : nil)
      }.compact
      all_bytes.concat(bs)
    end
  end

  num_chars = all_bytes.length / 32
  cols = 16
  tile_px = 8 * SCALE + MARGIN
  label_h = 12
  cell_h = 8 * SCALE + label_h + MARGIN

  new_bytes = []
  num_chars.times do |i|
    col = i % cols
    row = i / cols
    ox = MARGIN + col * tile_px
    oy = MARGIN + row * cell_h
    pixels = read_tile(bmp, palette_rgb, ox, oy)
    new_bytes.concat(encode_tile(pixels))
  end

  return 0 unless new_bytes.length == all_bytes.length

  byte_idx = 0
  db_lines.each do |li|
    orig = lines[li]
    n_in_line = orig.sub(/^.*\.db\s+/, '').sub(/;.*/, '').strip
                    .split(',').count { |b| b.strip =~ /^\$?[0-9a-fA-F]+$/ }
    indent = orig[/^\s*/]
    hex = new_bytes[byte_idx, n_in_line].map { |b| "$%02x" % b }
    lines[li] = "#{indent}.db #{hex.join(',')}\n"
    byte_idx += n_in_line
  end

  File.write(font_file, lines.join)
  puts "Imported #{num_chars} font chars from #{bmp_path}"
  1
end

# Main
bg_input   = ARGV[0] || 'bg_tiles.bmp'
spr_input  = ARGV[1] || 'sprites.bmp'
font_input = ARGV[2] || 'font.bmp'

palettes = parse_palette(VDP_FILE)
pal0 = palettes[0].map { |c| sms_to_rgb(c) }
pal1 = palettes[1].map { |c| sms_to_rgb(c) }

total = 0
total += import_bg(bg_input, pal0, TILES_FILE)
total += import_sprites(spr_input, pal1, TILES_FILE)
total += import_font(font_input, pal0, FONT_FILE)

puts "Done. #{total} blocks updated." if total > 0
puts "No changes." if total == 0
