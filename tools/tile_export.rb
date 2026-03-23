#!/usr/bin/env ruby
# Exports tile data from the SMS roguelike into two BMP images:
#   - bg_tiles.bmp: background tiles rendered with palette 0
#   - sprites.bmp:  sprite tiles rendered with palette 1
# Usage: ruby tools/tile_export.rb [bg_output.bmp] [spr_output.bmp]

TILES_FILE = File.join(__dir__, '..', 'src', 'data', 'tiles.inc')
VDP_FILE = File.join(__dir__, '..', 'src', 'vdp.inc')
SCALE = 1
MARGIN = 1

# Simple BMP writer (24-bit, no compression, no dependencies)
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
      # BMP header
      f.write(['BM', file_size, 0, 0, 54].pack('A2Vv2V'))
      # DIB header (BITMAPINFOHEADER)
      f.write([40, @w, @h, 1, 24, 0, pixel_data_size, 2835, 2835, 0, 0].pack('Vl<2v2V6'))
      # Pixel data (bottom-up)
      (@h - 1).downto(0) do |y|
        @w.times do |x|
          r, g, b = @pixels[y][x]
          f.write([b, g, r].pack('CCC'))  # BGR
        end
        f.write("\0" * row_pad)
      end
    end
  end
end

# Convert SMS color byte to RGB array
def sms_to_rgb(byte)
  r = (byte & 0x03) * 85
  g = ((byte >> 2) & 0x03) * 85
  b = ((byte >> 4) & 0x03) * 85
  [r, g, b]
end

# Parse SMS palettes from vdp.inc
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

# Parse tile data blocks from tiles.inc
def parse_tiles(file)
  tiles = []
  current_label = nil
  current_bytes = []
  comment = nil

  File.readlines(file).each do |line|
    stripped = line.strip
    if stripped =~ /^(\w+Data\w*):/ || stripped =~ /^(\w+Spr\w*Data\w*):/
      if current_label && current_bytes.length >= 32
        tiles << { label: current_label, bytes: current_bytes, comment: comment }
      end
      current_label = $1
      current_bytes = []
      comment = nil
    end
    if stripped =~ /^;\s*={3,}\s*(.+?)\s*={3,}/ || stripped =~ /^;\s*-{3,}\s*(.+?)\s*-{3,}/
      comment = $1.strip if current_bytes.empty?
    elsif stripped =~ /^;\s*(.+?)\s*\(tiles?\s/
      comment = $1.strip if current_bytes.empty?
    end
    if stripped =~ /^\.db\s+(.+)/
      data_part = $1.sub(/;.*/, '').strip
      bytes = data_part.split(',').map { |b|
        b = b.strip
        b.start_with?('$') ? b[1..].to_i(16) : (b =~ /^\d+$/ ? b.to_i : nil)
      }.compact
      current_bytes.concat(bytes)
    end
  end
  if current_label && current_bytes.length >= 32
    tiles << { label: current_label, bytes: current_bytes, comment: comment }
  end
  tiles
end

# Decode 8x8 tile from 32 bytes
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

# Draw an 8x8 tile onto the BMP at scaled position
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

# Export a set of 8x8 BG tile blocks to a BMP
def export_bg(blocks, palette, output_file)
  cols = 8  # tiles per row
  tile_px = 8 * SCALE + MARGIN

  total_tiles = blocks.sum { |b| b[:bytes].length / 32 }
  rows = (total_tiles + cols - 1) / cols
  img_w = MARGIN + cols * tile_px
  img_h = MARGIN + rows * tile_px

  bmp = BMPWriter.new(img_w, img_h)
  idx = 0
  blocks.each do |block|
    n = block[:bytes].length / 32
    n.times do |i|
      px = decode_tile(block[:bytes], i * 32)
      col = idx % cols
      row = idx / cols
      draw_tile(bmp, px, palette, MARGIN + col * tile_px, MARGIN + row * tile_px)
      idx += 1
    end
  end

  bmp.save(output_file)
  puts "Exported #{total_tiles} BG tiles to #{output_file} (#{img_w}x#{img_h})"
end

# Export sprite blocks to a BMP (16x16 entities, TL/BL/TR/BR quadrant order)
def export_sprites(blocks, palette, output_file)
  cols = 6  # sprites per row
  spr_px = 16 * SCALE + MARGIN * 2

  total_sprites = blocks.sum { |b| b[:bytes].length / 128 }
  rows = (total_sprites + cols - 1) / cols
  img_w = MARGIN + cols * spr_px
  img_h = MARGIN + rows * spr_px

  # Transparent = dark grey background
  pal_with_bg = palette.dup
  pal_with_bg[0] = [32, 32, 32]

  bmp = BMPWriter.new(img_w, img_h)
  idx = 0
  blocks.each do |block|
    n = block[:bytes].length / 128
    n.times do |i|
      base = i * 128
      tl = decode_tile(block[:bytes], base)
      bl = decode_tile(block[:bytes], base + 32)
      tr = decode_tile(block[:bytes], base + 64)
      br = decode_tile(block[:bytes], base + 96)
      col = idx % cols
      row = idx / cols
      ox = MARGIN + col * spr_px
      oy = MARGIN + row * spr_px
      draw_tile(bmp, tl, pal_with_bg, ox, oy)
      draw_tile(bmp, bl, pal_with_bg, ox, oy + 8 * SCALE)
      draw_tile(bmp, tr, pal_with_bg, ox + 8 * SCALE, oy)
      draw_tile(bmp, br, pal_with_bg, ox + 8 * SCALE, oy + 8 * SCALE)
      idx += 1
    end
  end

  bmp.save(output_file)
  puts "Exported #{total_sprites} sprites to #{output_file} (#{img_w}x#{img_h})"
end

# Parse font data from font.inc (same 4bpp format, 95 chars starting at space)
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

# Export font as a grid of 8x8 tiles with ASCII labels
def export_font(font_bytes, palette, output_file)
  cols = 16
  num_chars = font_bytes.length / 32
  rows = (num_chars + cols - 1) / cols
  tile_px = 8 * SCALE + MARGIN
  label_h = 12  # space for ASCII label below each tile
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
bg_output = ARGV[0] || 'bg_tiles.bmp'
spr_output = ARGV[1] || 'sprites.bmp'
font_output = ARGV[2] || 'font.bmp'

FONT_FILE = File.join(__dir__, '..', 'src', 'data', 'font.inc')

palettes = parse_palette(VDP_FILE)
pal0 = palettes[0].map { |c| sms_to_rgb(c) }
pal1 = palettes[1].map { |c| sms_to_rgb(c) }

blocks = parse_tiles(TILES_FILE)
# Sprite blocks have "Spr" in the label
bg_blocks = blocks.reject { |t| t[:label] =~ /Spr/i }
spr_blocks = blocks.select { |t| t[:label] =~ /Spr/i }

export_bg(bg_blocks, pal0, bg_output)
export_sprites(spr_blocks, pal1, spr_output)

font_bytes = parse_font(FONT_FILE)
export_font(font_bytes, pal0, font_output)
