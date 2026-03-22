#!/usr/bin/env ruby
# Encodes 8x8 background tile pixel grids into SMS 4bpp planar .db lines
# Input: text file with tile definitions (8 rows of 8 hex digits each)
# Label lines start with @
# Usage: ruby tools/tile_encode.rb tiles.txt

input = ARGF.read
current_label = nil
current_rows = []

input.each_line do |line|
  line = line.strip
  next if line.empty? || line.start_with?(';')

  if line.start_with?('@')
    if current_label && current_rows.length == 8
      puts "#{current_label}:"
      current_rows.each do |row|
        bp = [0, 0, 0, 0]
        8.times do |x|
          ci = row[x]
          bit = 7 - x
          4.times { |p| bp[p] |= ((ci >> p) & 1) << bit }
        end
        puts ".db #{bp.map { |b| '$%02x' % b }.join(',')}"
      end
      puts
    end
    current_label = line[1..].strip
    current_rows = []
  elsif line =~ /^[0-9a-fA-F]{8}$/
    current_rows << line.chars.map { |c| c.to_i(16) }
  end
end

if current_label && current_rows.length == 8
  puts "#{current_label}:"
  current_rows.each do |row|
    bp = [0, 0, 0, 0]
    8.times do |x|
      ci = row[x]
      bit = 7 - x
      4.times { |p| bp[p] |= ((ci >> p) & 1) << bit }
    end
    puts ".db #{bp.map { |b| '$%02x' % b }.join(',')}"
  end
end
