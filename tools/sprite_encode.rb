#!/usr/bin/env ruby
# Encodes 16x16 sprite pixel grids into SMS 4bpp planar .db lines
# Input: text file with sprite definitions
# Output: WLA-DX .db lines for tiles.inc
#
# Format: each sprite starts with a label line, then 16 rows of 16 hex digits (0-F)
# Example:
#   @PlayerSpr1Data ; Frog frame 1
#   0000000000000000
#   0003355555530000
#   ...
#
# Usage: ruby tools/sprite_encode.rb sprites.txt > output.inc

def encode_sprite(label, comment, rows)
  # Split into 4 quadrants: TL(0-7,0-7), BL(8-15,0-7), TR(0-7,8-15), BR(8-15,8-15)
  tl = rows[0..7].map { |r| r[0..7] }
  bl = rows[8..15].map { |r| r[0..7] }
  tr = rows[0..7].map { |r| r[8..15] }
  br = rows[8..15].map { |r| r[8..15] }

  lines = []
  lines << "; #{comment}" if comment
  lines << "#{label}:"

  [["TL", tl], ["BL", bl], ["TR", tr], ["BR", br]].each do |name, quad|
    lines << "; #{name}"
    quad.each_with_index do |row, y|
      bp = [0, 0, 0, 0]
      8.times do |x|
        ci = row[x]
        bit = 7 - x
        4.times { |p| bp[p] |= ((ci >> p) & 1) << bit }
      end
      hex = bp.map { |b| "$%02x" % b }.join(",")
      lines << ".db #{hex}"
    end
  end
  lines.join("\n")
end

# Parse input file
input = ARGF.read
sprites = []
current_label = nil
current_comment = nil
current_rows = []

input.each_line do |line|
  line = line.strip
  next if line.empty? || (line.start_with?(';') && !current_label)

  if line.start_with?('@')
    # Save previous sprite
    if current_label && current_rows.length == 16
      sprites << [current_label, current_comment, current_rows]
    end
    # Parse new sprite header
    parts = line[1..].split(';', 2)
    current_label = parts[0].strip
    current_comment = parts[1]&.strip
    current_rows = []
  elsif line =~ /^[0-9a-fA-F]{16}$/
    current_rows << line.chars.map { |c| c.to_i(16) }
  end
end

# Save last sprite
if current_label && current_rows.length == 16
  sprites << [current_label, current_comment, current_rows]
end

sprites.each { |label, comment, rows| puts encode_sprite(label, comment, rows) + "\n" }
