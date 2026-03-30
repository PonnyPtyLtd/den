#!/usr/bin/env ruby
# Exports tile data from tiles.inc into a single combined BMP:
#   tiles_export.bmp: BG tiles (palette 0) + sprites (palette 1)
# Font exported separately to font.bmp.
# Usage: ruby tools/tile_export.rb [tiles_output.bmp] [font_output.bmp]

TILES_FILE = File.join(__dir__, '..', 'src', 'data', 'tiles.inc')
FONT_FILE  = File.join(__dir__, '..', 'src', 'data', 'font.inc')
VDP_FILE   = File.join(__dir__, '..', 'src', 'vdp.inc')
SCALE  = 1
MARGIN = 1

# Layout definition: rows of tile entries
# [:bg8, label]                         - single 8x8 BG tile (palette 0)
# [:bg16, tl, tr, bl, br]              - 16x16 BG group from 4 labels (palette 0)
# [:spr, label]                         - 16x16 sprite (palette 1, 128 bytes)
# [:gap]                                - small spacer
LAYOUT = [
  # Row 0: Basic terrain (8x8)
  [
    [:bg8, 'WallTLData'],
    [:bg8, 'WallTRData'],
    [:bg8, 'FloorTileData'],
    [:bg8, 'SolidBlackData'],
  ],
  # Row 1: 16x16 BG groups
  [
    [:bg16, 'StairsTLData', 'StairsTRData', 'StairsBLData', 'StairsBRData'],
    [:bg16, 'IconInvTL', 'IconInvTR', 'IconInvBL', 'IconInvBR'],
    [:bg16, 'IconPickTL', 'IconPickTR', 'IconPickBL', 'IconPickBR'],
    [:bg16, 'IconWaitTL', 'IconWaitTR', 'IconWaitBL', 'IconWaitBR'],
    [:bg16, 'IconXTL', 'IconXTR', 'IconXBL', 'IconXBR'],
  ],
  # Row 2: Root border (8x8)
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
  # Row 3: Dirt wall tiles (8x8)
  [
    [:bg8, 'DirtSolidData'],
    [:bg8, 'DirtSolidV2Data'],
    [:bg8, 'DirtEdgeTData'],
    [:bg8, 'DirtEdgeT2Data'],
    [:bg8, 'DirtEdgeLData'],
    [:bg8, 'DirtEdgeL2Data'],
    [:bg8, 'DirtCornerTLData'],
  ],
  # Row 4: Greebles + Hearts (8x8)
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
  # Row 5: Sprites row 1 (palette 1)
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
  # Row 6: Sprites row 2
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
  # Row 7: Sprites row 3
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
  # Row 8: Sprites row 4
  [
    [:spr, 'ChickenSpr1Data'],
    [:spr, 'ChickenSpr2Data'],
    [:spr, 'FireballSprData'],
  ],
]

# Collect all label names from layout for the parser
LAYOUT_LABELS = LAYOUT.flat_map { |row|
  row.flat_map { |item|
    case item[0]
    when :bg8  then [item[1]]
    when :bg16 then item[1..4]
    when :spr  then [item[1]]
    else []
    end
  }
}.to_a.freeze

# Simple BMP writer (24-bit, no compression)
class BMPWriter
  def initialize(width, height)
    @w = width
    @h = height
    @pixels = Array.new(height) { Array.new(width, [0, 0, 0]) }
  end

  def set(x, y, r, g, b)
    return if x < 0 || x >= @w || y < 0 || y >= @h
    @pixels[y][x] = [r, g, b]
  end

  def fill_rect(x, y, w, h, r, g, b)
    h.times { |dy| w.times { |dx| set(x + dx, y + dy, r, g, b) } }
  end

  def save(path)
    row_bytes = @w * 3
    row_pad = (4 - row_bytes % 4) % 4
    pixel_data_size = (row_bytes + row_pad) * @h
    file_size = 54 + pixel_data_size

    File.open(path, 'wb') do |f|
      f.write(['BM', file_size, 0, 0, 54].pack('A2Vv2V'))
      f.write([40, @w, @h, 1, 24, 0, pixel_data_size, 2835, 2835, 0, 0].pack('Vl<2v2V6'))
      (@h - 1).downto(0) do |y|
        @w.times do |x|
          r, g, b = @pixels[y][x]
          f.write([b, g, r].pack('CCC'))
        end
        f.write("\0" * row_pad)
      end
    end
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

# Parse tile data blocks from tiles.inc, only for labels we care about
def parse_tiles(file, labels)
  label_set = labels.to_a
  blocks = {}
  current_label = nil
  current_bytes = []

  File.readlines(file).each do |line|
    stripped = line.strip
    if stripped =~ /^(\w+):/
      name = $1
      if label_set.include?(name)
        if current_label && current_bytes.length >= 32
          blocks[current_label] = current_bytes.dup
        end
        current_label = name
        current_bytes = []
      elsif current_label && name !~ /^\./ && current_bytes.length >= 32
        # Non-layout label encountered: save current block
        blocks[current_label] = current_bytes.dup
        current_label = nil
        current_bytes = []
      end
    end
    if current_label && stripped =~ /^\.db\s+(.+)/
      data_part = $1.sub(/;.*/, '').strip
      bytes = data_part.split(',').map { |b|
        b = b.strip
        b.start_with?('$') ? b[1..].to_i(16) : (b =~ /^\d+$/ ? b.to_i : nil)
      }.compact
      current_bytes.concat(bytes)
    end
  end
  if current_label && current_bytes.length >= 32
    blocks[current_label] = current_bytes.dup
  end
  blocks
end

# Decode one 8x8 tile from 32 bytes of 4bpp planar data
def decode_tile(bytes, offset = 0)
  pixels = Array.new(8) { Array.new(8, 0) }
  8.times do |row|
    bp0 = bytes[offset + row * 4] || 0
    bp1 = bytes[offset + row * 4 + 1] || 0
    bp2 = bytes[offset + row * 4 + 2] || 0
    bp3 = bytes[offset + row * 4 + 3] || 0
    8.times do |col|
      bit = 7 - col
      pixels[row][col] = ((bp0 >> bit) & 1) |
                         (((bp1 >> bit) & 1) << 1) |
                         (((bp2 >> bit) & 1) << 2) |
                         (((bp3 >> bit) & 1) << 3)
    end
  end
  pixels
end

# Draw an 8x8 tile onto the BMP
def draw_tile(bmp, pixels, palette, x, y)
  8.times do |py|
    8.times do |px|
      r, g, b = palette[pixels[py][px]]
      SCALE.times do |sy|
        SCALE.times do |sx|
          bmp.set(x + px * SCALE + sx, y + py * SCALE + sy, r, g, b)
        end
      end
    end
  end
end

# Compute layout positions → returns array of {label:, x:, y:, type:, palette:}
# and total image dimensions
def compute_layout
  placements = []
  cur_y = MARGIN
  img_w = 0

  LAYOUT.each do |row|
    # Determine row height from item types
    row_h = row.any? { |item| item[0] == :bg16 || item[0] == :spr } ? 16 * SCALE : 8 * SCALE
    cur_x = MARGIN

    row.each do |item|
      case item[0]
      when :bg8
        placements << { label: item[1], x: cur_x, y: cur_y, type: :bg8 }
        cur_x += 8 * SCALE + MARGIN
      when :bg16
        # Place 4 tiles in 2x2 arrangement (no internal margins)
        placements << { label: item[1], x: cur_x,              y: cur_y,              type: :bg8 }  # TL
        placements << { label: item[2], x: cur_x + 8 * SCALE,  y: cur_y,              type: :bg8 }  # TR
        placements << { label: item[3], x: cur_x,              y: cur_y + 8 * SCALE,  type: :bg8 }  # BL
        placements << { label: item[4], x: cur_x + 8 * SCALE,  y: cur_y + 8 * SCALE,  type: :bg8 }  # BR
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

# Export combined tiles BMP
def export_tiles(blocks, pal0, pal1, output_file)
  placements, img_w, img_h = compute_layout

  # Sprite transparent color → dark grey background
  spr_pal = pal1.dup
  spr_pal[0] = [32, 32, 32]

  # BG palette: render color 0 as dark grey (16,16,16) to distinguish from
  # color 5 which is also $00/black. This mirrors how sprites render color 0
  # as dark grey for transparency.
  bg_pal = pal0.dup
  bg_pal[0] = [16, 16, 16] if bg_pal[0] == [0, 0, 0]

  bmp = BMPWriter.new(img_w, img_h)

  placements.each do |p|
    data = blocks[p[:label]]
    unless data
      $stderr.puts "Warning: no data for #{p[:label]}"
      next
    end

    case p[:type]
    when :bg8
      pixels = decode_tile(data, 0)
      draw_tile(bmp, pixels, bg_pal, p[:x], p[:y])
    when :spr
      # 128 bytes: TL(0), BL(32), TR(64), BR(96)
      tl = decode_tile(data, 0)
      bl = decode_tile(data, 32)
      tr = decode_tile(data, 64)
      br = decode_tile(data, 96)
      draw_tile(bmp, tl, spr_pal, p[:x],              p[:y])
      draw_tile(bmp, bl, spr_pal, p[:x],              p[:y] + 8 * SCALE)
      draw_tile(bmp, tr, spr_pal, p[:x] + 8 * SCALE,  p[:y])
      draw_tile(bmp, br, spr_pal, p[:x] + 8 * SCALE,  p[:y] + 8 * SCALE)
    end
  end

  bmp.save(output_file)
  tile_count = placements.count { |p| p[:type] == :bg8 }
  spr_count = placements.count { |p| p[:type] == :spr }
  puts "Exported #{tile_count} BG tiles + #{spr_count} sprites to #{output_file} (#{img_w}x#{img_h})"
end

# Parse and export font (unchanged)
def parse_font(file)
  bytes = []
  File.readlines(file).each do |line|
    stripped = line.strip
    next unless stripped =~ /^\.db\s+(.+)/
    data_part = $1.sub(/;.*/, '').strip
    data_part.split(',').each do |b|
      b = b.strip
      val = b.start_with?('$') ? b[1..].to_i(16) : (b =~ /^\d+$/ ? b.to_i : nil)
      bytes << val if val
    end
  end
  bytes
end

def export_font(font_bytes, palette, output_file)
  cols = 16
  num_chars = font_bytes.length / 32
  rows = (num_chars + cols - 1) / cols
  tile_px = 8 * SCALE + MARGIN
  label_h = 12
  cell_h = 8 * SCALE + label_h + MARGIN
  img_w = MARGIN + cols * tile_px
  img_h = MARGIN + rows * cell_h

  bmp = BMPWriter.new(img_w, img_h)
  num_chars.times do |i|
    px = decode_tile(font_bytes, i * 32)
    col = i % cols
    row = i / cols
    x = MARGIN + col * tile_px
    y = MARGIN + row * cell_h
    draw_tile(bmp, px, palette, x, y)
  end

  bmp.save(output_file)
  puts "Exported #{num_chars} font chars to #{output_file} (#{img_w}x#{img_h})"
end

# Main
tiles_output = ARGV[0] || 'tiles_export.bmp'
font_output  = ARGV[1] || 'font.bmp'

palettes = parse_palette(VDP_FILE)
pal0 = palettes[0].map { |c| sms_to_rgb(c) }
pal1 = palettes[1].map { |c| sms_to_rgb(c) }

blocks = parse_tiles(TILES_FILE, LAYOUT_LABELS)
export_tiles(blocks, pal0, pal1, tiles_output)

font_bytes = parse_font(FONT_FILE)
export_font(font_bytes, pal0, font_output)
