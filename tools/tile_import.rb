#!/usr/bin/env ruby
# Imports tile data from BMP images back into tiles.inc and font.inc
# BMPs must match the layout produced by tile_export.rb
# Usage: ruby tools/tile_import.rb [bg_tiles.bmp] [sprites.bmp] [font.bmp]
# Any argument can be omitted or set to "-" to skip that import.

TILES_FILE = File.join(__dir__, '..', 'src', 'data', 'tiles.inc')
FONT_FILE = File.join(__dir__, '..', 'src', 'data', 'font.inc')
VDP_FILE = File.join(__dir__, '..', 'src', 'vdp.inc')
SCALE = 1
MARGIN = 1

# Simple BMP reader (24-bit uncompressed only)
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

# Read an 8x8 tile from BMP at scaled position, map to palette indices
def read_tile(bmp, palette_rgb, x, y)
  Array.new(8) { |py| Array.new(8) { |px|
    r, g, b = bmp.get(x + px * SCALE + SCALE / 2, y + py * SCALE + SCALE / 2)
    nearest_color(r, g, b, palette_rgb)
  }}
end

# Parse tile data blocks from an .inc file, tracking .db line positions
def parse_tile_blocks(file)
  blocks = []
  current_label = nil
  current_bytes = []
  db_lines = []
  lines = File.readlines(file)
  lines.each_with_index do |line, idx|
    stripped = line.strip
    if stripped =~ /^(\w+Data\w*):/ || stripped =~ /^(\w+Spr\w*Data\w*):/
      if current_label && current_bytes.length >= 32
        blocks << { label: current_label, bytes: current_bytes, db_lines: db_lines.dup }
      end
      current_label = $1
      current_bytes = []
      db_lines = []
    end
    if stripped =~ /^\.db\s+(.+)/
      data_part = $1.sub(/;.*/, '').strip
      tokens = data_part.split(',').map(&:strip)
      # Only include lines where all values are numeric hex/dec (skip symbolic constants)
      if tokens.all? { |b| b =~ /^\$[0-9a-fA-F]+$/ || b =~ /^\d+$/ }
        db_lines << idx
        bs = tokens.map { |b|
          b.start_with?('$') ? b[1..].to_i(16) : b.to_i
        }
        current_bytes.concat(bs)
      end
    end
  end
  if current_label && current_bytes.length >= 32
    blocks << { label: current_label, bytes: current_bytes, db_lines: db_lines.dup }
  end
  [lines, blocks]
end

# Replace .db lines in the lines array with new encoded bytes
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

# --- Import BG tiles ---
def import_bg(bmp_path, palette_rgb, tiles_file)
  return 0 unless bmp_path && bmp_path != "-" && File.exist?(bmp_path)
  bmp = BMPReader.new(bmp_path)
  lines, blocks = parse_tile_blocks(tiles_file)

  bg_blocks = blocks.reject { |t| t[:label] =~ /Spr/i }
  cols = 8
  tile_px = 8 * SCALE + MARGIN

  changes = 0
  idx = 0
  bg_blocks.each do |block|
    n = block[:bytes].length / 32
    new_bytes = []
    n.times do |i|
      col = idx % cols
      row = idx / cols
      ox = MARGIN + col * tile_px
      oy = MARGIN + row * tile_px
      pixels = read_tile(bmp, palette_rgb, ox, oy)
      new_bytes.concat(encode_tile(pixels))
      idx += 1
    end
    if new_bytes.length == block[:bytes].length
      replace_db_lines(lines, block, new_bytes)
      changes += 1
    end
  end

  File.write(tiles_file, lines.join)
  puts "Imported #{changes} BG tile blocks from #{bmp_path}"
  changes
end

# --- Import sprites ---
def import_sprites(bmp_path, palette_rgb, tiles_file)
  return 0 unless bmp_path && bmp_path != "-" && File.exist?(bmp_path)
  bmp = BMPReader.new(bmp_path)
  lines, blocks = parse_tile_blocks(tiles_file)

  spr_blocks = blocks.select { |t| t[:label] =~ /Spr/i }
  cols = 6
  spr_px = 16 * SCALE + MARGIN * 2

  # For sprites, dark pixels (< 40 per channel) map to color 0 (transparent)
  # This matches the dark grey background used in export
  spr_palette = palette_rgb.dup

  changes = 0
  idx = 0
  spr_blocks.each do |block|
    n = block[:bytes].length / 128
    new_bytes = []
    n.times do |i|
      col = idx % cols
      row = idx / cols
      ox = MARGIN + col * spr_px
      oy = MARGIN + row * spr_px
      # Read 16x16 sprite as 4 quadrants
      full = Array.new(16) { |py| Array.new(16) { |px|
        r, g, b = bmp.get(ox + px * SCALE + SCALE / 2, oy + py * SCALE + SCALE / 2)
        ci = nearest_color(r, g, b, spr_palette)
        ci = 0 if r < 48 && g < 48 && b < 48  # dark grey bg = transparent
        ci
      }}
      tl = full[0..7].map { |r| r[0..7] }
      bl = full[8..15].map { |r| r[0..7] }
      tr = full[0..7].map { |r| r[8..15] }
      br = full[8..15].map { |r| r[8..15] }
      [tl, bl, tr, br].each { |t| new_bytes.concat(encode_tile(t)) }
      idx += 1
    end
    if new_bytes.length == block[:bytes].length
      replace_db_lines(lines, block, new_bytes)
      changes += 1
    end
  end

  File.write(tiles_file, lines.join)
  puts "Imported #{changes} sprite blocks from #{bmp_path}"
  changes
end

# --- Import font ---
def import_font(bmp_path, palette_rgb, font_file)
  return 0 unless bmp_path && bmp_path != "-" && File.exist?(bmp_path)
  bmp = BMPReader.new(bmp_path)

  # Font is a single contiguous block of .db lines under FontData:
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

  # Replace .db lines
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
bg_input = ARGV[0] || 'bg_tiles.bmp'
spr_input = ARGV[1] || 'sprites.bmp'
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
