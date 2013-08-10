#!/usr/bin/env ruby
file0 = ""
file1 = ""

# 16x16 フォント 東雲 ビットマップフォントファミリー
file0 = "../fonts/shinonome-0.9.11/bdf/shnm8x16ab.bdf"
#file0 = "../fonts/shinonome-0.9.11/bdf/shnm8x16a.bdf"
$maxWidth1byte = 8
$maxWidth2byte = 16
$maxHeight = 16

while args = ARGV.shift do
  if args =~ /USE_KANJI/
    file1 = "../fonts/shinonome-0.9.11/bdf/shnmk16b.bdf"
  end
end
puts file1

def getFontFile(file)
  total = 0
  flg = false
  width  = 0
  height = 0
  fontData = []
  size = 0
  open(file, "r") do |f|
    while line = f.gets
      if line =~ /STARTCHAR\s+(\S.+)$/
        fontCode = $1.hex
      end
      if line =~ /BBX\s+(\d+)\s+(\d+)\s+\S.+$/
        width  = $1.to_i
        if ($maxWidth2byte > 0) and (width > $maxWidth2byte)
          width = $maxWidth2byte
        end
        height = $2.to_i
        total += (width * height)
      end
      if line =~ /BITMAP/
        flg = true
        fontData = []
      elsif line =~ /ENDCHAR/
        flg = false
        $allFontDatas.push [fontCode,width,height,size * 4,fontData]
        fontData = []
        size = 0
      elsif flg
        line.chop!
        if line.size > size
          size = line.size
        end
        d0 = "#{line}".hex
        fontData.push d0
        d = d0
      end
    end
  end
  if total > 8 * 1024
    puts "#{file}: #{format("%d kbyte",total / (8 * 1024))}"
  else
    puts "#{file}: #{format("%d byte",total / 8)}"
  end
end

def gaku8(d)
  r = 0
  for i in 1..8 do
    r = (r << 1) | (d & 0x01)
    d = d >> 1
  end
  return r
end

$allFontDatas = []
if file0 != ""
  getFontFile(file0)
end
if file1 != ""
  getFontFile(file1)
end
i = 0
j = 0

block1byte = []
block2byte = []
alldatas = []
bb = -10
bs = -10
sp = 0
dd = ""
ddx = 0
maxw = 0
minw = 0
dd << "const int width1bytefont = #{$maxWidth1byte};\n"
dd << "const int width2bytefont = #{$maxWidth2byte};\n"
dd << "const int heightfont = #{$maxHeight};\n"
dd << "#define GLCD_W #{400/$maxWidth1byte}\n"
dd << "#define GLCD_H #{240/$maxWidth2byte}\n"

dd << "const uint8_t allFontDatas[] ={\n"
for k in 0..($allFontDatas.size - 1)
  ff = $allFontDatas[k]
  fontCode = ff[0]
  width    = ff[1]
  height   = ff[2]
  size     = ff[3]
  if fontCode != bb + 1
    if bs != -10
      if (maxw == $maxWidth1byte) and (minw == $maxWidth1byte)
        block1byte.push [bs,bb,sp]
      elsif (maxw == $maxWidth2byte) and (minw == $maxWidth2byte)
        block2byte.push [bs,bb,sp]
      end
    end
    maxw = width
    minw = width
    bs = fontCode
    sp = ddx
  else
    if width > maxw
      maxw = width
    end
    if width < minw
      minw = width
    end
  end
  bb = fontCode
  if width > 8
    for j in 0..(height - 1) do
      data = gaku8((ff[4][j]>>8) & 0xff)
      dd << format("0x%x,",data)
      alldatas.push(data)
      ddx += 1
    end
  end
  for j in 0..(height - 1) do
    data = gaku8(ff[4][j] & 0xff)
    dd << format("0x%x,",data)
    alldatas.push(data)
    ddx += 1
  end
  dd << "\n"
end
dd << "};\n"
dd << "const uint32_t sizeOfAllFontDatas = #{ddx};\n"
puts ddx / 1024
if bs != -10
  if (maxw == $maxWidth1byte) and (minw == $maxWidth1byte)
    block1byte.push [bs,bb,sp]
  elsif (maxw == $maxWidth2byte) and (minw == $maxWidth2byte)
    block2byte.push [bs,bb,sp]
  end
end
block1byte.each do |e|
  puts "#{format("%x",e[0])}-#{format("%x",e[1])} : #{e[1]-e[0]} #{e[2]}"
end
block2byte.each do |e|
  puts "#{format("%x",e[0])}-#{format("%x",e[1])} : #{e[1]-e[0]} #{e[2]}"
end

dd << "const uint32_t block1bytesp[] ={\n"
for k in 0..(block1byte.size - 1)
  if k < (block1byte.size - 1)
    dd << "#{block1byte[k][2]},"
  else
    dd << "#{block1byte[k][2]}"
  end
  if (k % 10) == 9
    dd << "\n"
  end
end
dd << "\n};\n"
dd << "const uint8_t block1bytecode[] ={\n"
for k in 0..(block1byte.size - 1)
  if k < (block1byte.size - 1)
    dd << "0x#{format("%02x",block1byte[k][0])},0x#{format("%02x",block1byte[k][1])},"
  else
    dd << "0x#{format("%02x",block1byte[k][0])},0x#{format("%02x",block1byte[k][1])}"
  end
  if (k % 10) == 9
    dd << "\n"
  end
end
dd << "\n};\n"
dd << "const int sizeOFblock1bytecode = #{2 * block1byte.size};\n"

dd << "const uint32_t block2bytesp[] ={\n"
for k in 0..(block2byte.size - 1)
  if k < (block2byte.size - 1)
    dd << "#{block2byte[k][2]},"
  else
    dd << "#{block2byte[k][2]}"
  end
  if (k % 10) == 9
    dd << "\n"
  end
end
dd << "\n};\n"
dd << "const uint16_t block2bytecode[] ={\n"
for k in 0..(block2byte.size - 1)
  if k < (block2byte.size - 1)
    dd << "0x#{format("%02x",block2byte[k][0])},0x#{format("%02x",block2byte[k][1])},"
  else
    dd << "0x#{format("%02x",block2byte[k][0])},0x#{format("%02x",block2byte[k][1])}"
  end
  if (k % 5) == 4
    dd << "\n"
  end
end
dd << "\n};\n"
dd << "const int sizeOFblock2bytecode = #{2 * block2byte.size};\n"

open("fontdata.h","w") do |f|
  f.binmode
  f.print dd
end

