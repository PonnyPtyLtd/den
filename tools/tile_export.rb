#!/usr/bin/env ruby
# Exports all tile data from the SMS roguelike into a single BMP image
# Each tile is rendered at 4x scale. BG tiles as 8x8, sprites as 16x16 entities.
# Usage: ruby tools/tile_export.rb [output.bmp]

TILES_FILE = File.join(__dir__, '..', 'src', 'data', 'tiles.inc')
VDP_FILE = File.join(__dir__, '..', 'src', 'vdp.inc')
SCALE = 4
MARGIN = 2

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

# Main
output_file = ARGV[0] || 'tiles_export.bmp'
palettes = parse_palette(VDP_FILE)
pal0 = palettes[0].map { |c| sms_to_rgb(c) }
pal1 = palettes[1].map { |c| sms_to_rgb(c) }

blocks = parse_tiles(TILES_FILE)
bg_blocks = blocks.select { |t| t[:label] =~ /Wall|Floor|Stair|Root|Icon/i }
spr_blocks = blocks.reject { |t| t[:label] =~ /Wall|Floor|Stair|Root|Icon/i }

# Calculate layout
entries = []
bg_blocks.each { |b| entries << { block: b, pal: pal0, type: :bg } }
spr_blocks.each { |b| entries << { block: b, pal: pal1, type: :spr } }

max_w = 600
total_h = MARGIN
entries.each do |e|
  if e[:type] == :bg
    total_h += 8 * SCALE + MARGIN
  else
    total_h += 16 * SCALE + MARGIN
  end
end

bmp = BMPWriter.new(max_w, total_h)
y = MARGIN

entries.each do |e|
  block = e[:block]
  pal = e[:pal]

  if e[:type] == :bg
    n = block[:bytes].length / 32
    n.times do |i|
      px = decode_tile(block[:bytes], i * 32)
      draw_tile(bmp, px, pal, MARGIN + i * (8 * SCALE + MARGIN), y)
    end
    y += 8 * SCALE + MARGIN
  else
    n = block[:bytes].length / 128
    n.times do |i|
      base = i * 128
      tl = decode_tile(block[:bytes], base)
      bl = decode_tile(block[:bytes], base + 32)
      tr = decode_tile(block[:bytes], base + 64)
      br = decode_tile(block[:bytes], base + 96)
      ox = MARGIN + i * (16 * SCALE + MARGIN * 2)
      # Transparent = dark grey background
      pal_with_bg = pal.dup
      pal_with_bg[0] = [32, 32, 32]
      draw_tile(bmp, tl, pal_with_bg, ox, y)
      draw_tile(bmp, bl, pal_with_bg, ox, y + 8 * SCALE)
      draw_tile(bmp, tr, pal_with_bg, ox + 8 * SCALE, y)
      draw_tile(bmp, br, pal_with_bg, ox + 8 * SCALE, y + 8 * SCALE)
    end
    y += 16 * SCALE + MARGIN
  end
end

bmp.save(output_file)
puts "Exported #{entries.length} tile blocks to #{output_file} (#{max_w}x#{total_h})"
