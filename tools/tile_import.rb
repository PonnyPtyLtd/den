#!/usr/bin/env ruby
# Imports tile data from a BMP image back into tiles.inc
# The BMP must match the layout produced by tile_export.rb
# Usage: ruby tools/tile_import.rb input.bmp

TILES_FILE = File.join(__dir__, '..', 'src', 'data', 'tiles.inc')
VDP_FILE = File.join(__dir__, '..', 'src', 'vdp.inc')
SCALE = 4
MARGIN = 2

# Simple BMP reader (24-bit uncompressed only)
class BMPReader
  attr_reader :width, :height, :pixels

  def initialize(path)
    data = File.binread(path)
    raise "Not a BMP file" unless data[0..1] == 'BM'
    offset = data[10..13].unpack1('V')
    @width = data[18..21].unpack1('l<')
    @height = data[22..25].unpack1('l<').abs
    bpp = data[28..29].unpack1('v')
    raise "Only 24-bit BMP supported (got #{bpp})" unless bpp == 24
    bottom_up = data[22..25].unpack1('l<') > 0
    row_bytes = @width * 3
    row_pad = (4 - row_bytes % 4) % 4
    @pixels = Array.new(@height) { Array.new(@width, [0, 0, 0]) }
    @height.times do |y|
      src_y = bottom_up ? (@height - 1 - y) : y
      row_offset = offset + src_y * (row_bytes + row_pad)
      @width.times do |x|
        i = row_offset + x * 3
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
      db_lines << idx
      data_part = $1.sub(/;.*/, '').strip
      bs = data_part.split(',').map { |b|
        b = b.strip
        b.start_with?('$') ? b[1..].to_i(16) : (b =~ /^\d+$/ ? b.to_i : nil)
      }.compact
      current_bytes.concat(bs)
    end
  end
  if current_label && current_bytes.length >= 32
    blocks << { label: current_label, bytes: current_bytes, db_lines: db_lines.dup }
  end
  [lines, blocks]
end

# Main
input_file = ARGV[0] || abort("Usage: ruby tools/tile_import.rb <input.bmp>")
bmp = BMPReader.new(input_file)
palettes = parse_palette(VDP_FILE)
pal0 = palettes[0].map { |c| sms_to_rgb(c) }
pal1 = palettes[1].map { |c| sms_to_rgb(c) }
lines, blocks = parse_tile_blocks(TILES_FILE)

bg_pat = /Wall|Floor|Stair|Root|Icon/i
bg_blocks = blocks.select { |t| t[:label] =~ bg_pat }
spr_blocks = blocks.reject { |t| t[:label] =~ bg_pat }
ordered = bg_blocks + spr_blocks

y = MARGIN
changes = 0

ordered.each do |block|
  is_bg = block[:label] =~ bg_pat
  pal = is_bg ? pal0 : pal1

  if is_bg
    n = block[:bytes].length / 32
    new_bytes = []
    n.times do |i|
      ox = MARGIN + i * (8 * SCALE + MARGIN)
      pixels = Array.new(8) { |py| Array.new(8) { |px|
        r, g, b = bmp.get(ox + px * SCALE + SCALE / 2, y + py * SCALE + SCALE / 2)
        nearest_color(r, g, b, pal)
      }}
      new_bytes.concat(encode_tile(pixels))
    end
    y += 8 * SCALE + MARGIN
  else
    n = block[:bytes].length / 128
    new_bytes = []
    n.times do |i|
      ox = MARGIN + i * (16 * SCALE + MARGIN * 2)
      full = Array.new(16) { |py| Array.new(16) { |px|
        r, g, b = bmp.get(ox + px * SCALE + SCALE / 2, y + py * SCALE + SCALE / 2)
        ci = nearest_color(r, g, b, pal)
        ci = 0 if r < 40 && g < 40 && b < 40  # dark = transparent
        ci
      }}
      tl = full[0..7].map { |r| r[0..7] }
      bl = full[8..15].map { |r| r[0..7] }
      tr = full[0..7].map { |r| r[8..15] }
      br = full[8..15].map { |r| r[8..15] }
      [tl, bl, tr, br].each { |t| new_bytes.concat(encode_tile(t)) }
    end
    y += 16 * SCALE + MARGIN
  end

  next unless new_bytes.length == block[:bytes].length

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
  changes += 1
end

File.write(TILES_FILE, lines.join)
puts "Imported #{changes} tile blocks from #{input_file}"
