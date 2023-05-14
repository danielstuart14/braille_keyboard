from layout_ptbr import KeyboardLayout

BRAILLE_DEFAULT = """
  0b000000 DEFAULT
' 0b000001 DEFAULT
. 0b000010 DEFAULT
- 0b000011 DEFAULT
  0b000100 DEFAULT
$ 0b000101 DEFAULT
* 0b000110 DEFAULT
° 0b000111 DEFAULT
, 0b001000 DEFAULT
? 0b001001 DEFAULT
; 0b001010 DEFAULT
" 0b001011 DEFAULT
: 0b001100 DEFAULT
/ 0b001101 DEFAULT
! 0b001110 DEFAULT
= 0b001111 DEFAULT
  0b010000 DEFAULT
  0b010001 UPPER
í 0b010010 DEFAULT
ó 0b010011 DEFAULT
  0b010100 DEFAULT
| 0b010101 DEFAULT
ã 0b010110 DEFAULT
  0b010111 NUMBER
i 0b011000 DEFAULT
õ 0b011001 DEFAULT
s 0b011010 DEFAULT
è 0b011011 DEFAULT
j 0b011100 DEFAULT
w 0b011101 DEFAULT
t 0b011110 DEFAULT
ú 0b011111 DEFAULT
a 0b100000 DEFAULT
â 0b100001 DEFAULT
k 0b100010 DEFAULT
u 0b100011 DEFAULT
e 0b100100 DEFAULT
@ 0b100101 DEFAULT
o 0b100110 DEFAULT
z 0b100111 DEFAULT
b 0b101000 DEFAULT
ê 0b101001 DEFAULT
l 0b101010 DEFAULT
v 0b101011 DEFAULT
h 0b101100 DEFAULT
ü 0b101101 DEFAULT
r 0b101110 DEFAULT
á 0b101111 DEFAULT
c 0b110000 DEFAULT
ì 0b110001 DEFAULT
m 0b110010 DEFAULT
x 0b110011 DEFAULT
d 0b110100 DEFAULT
ô 0b110101 DEFAULT
n 0b110110 DEFAULT
y 0b110111 DEFAULT
f 0b111000 DEFAULT
à 0b111001 DEFAULT
p 0b111010 DEFAULT
ç 0b111011 DEFAULT
g 0b111100 DEFAULT
ï 0b111101 DEFAULT
q 0b111110 DEFAULT
é 0b111111 DEFAULT
"""

BRAILLE_NUMBER = """
  0b000000 DEFAULT
  0b000001 DEFAULT
. 0b000010 NUMBER
  0b000011 DEFAULT
  0b000100 DEFAULT
  0b000101 DEFAULT
  0b000110 DEFAULT
  0b000111 DEFAULT
, 0b001000 NUMBER
  0b001001 DEFAULT
  0b001010 DEFAULT
x 0b001011 DEFAULT
  0b001100 DEFAULT
  0b001101 DEFAULT
+ 0b001110 DEFAULT
  0b001111 DEFAULT
  0b010000 DEFAULT
  0b010001 DEFAULT
  0b010010 DEFAULT
  0b010011 DEFAULT
  0b010100 DEFAULT
  0b010101 DEFAULT
  0b010110 DEFAULT
  0b010111 NUMBER
9 0b011000 NUMBER
( 0b011001 DEFAULT
  0b011010 DEFAULT
  0b011011 DEFAULT
0 0b011100 NUMBER
  0b011101 DEFAULT
  0b011110 DEFAULT
  0b011111 DEFAULT
1 0b100000 NUMBER
  0b100001 DEFAULT
# 0b100010 NUMBER
  0b100011 DEFAULT
5 0b100100 NUMBER
  0b100101 DEFAULT
) 0b100110 DEFAULT
  0b100111 DEFAULT
2 0b101000 NUMBER
  0b101001 DEFAULT
  0b101010 DEFAULT
  0b101011 DEFAULT
8 0b101100 NUMBER
' 0b101101 NUMBER
  0b101110 DEFAULT
  0b101111 DEFAULT
3 0b110000 NUMBER
  0b110001 DEFAULT
  0b110010 DEFAULT
  0b110011 DEFAULT
4 0b110100 NUMBER
  0b110101 DEFAULT
  0b110110 DEFAULT
  0b110111 DEFAULT
6 0b111000 NUMBER
  0b111001 DEFAULT
  0b111010 DEFAULT
  0b111011 DEFAULT
7 0b111100 NUMBER
  0b111101 DEFAULT
  0b111110 DEFAULT
  0b111111 DEFAULT
"""

def parse_braille(data):
    lines = data.split("\n")[1:]

    lookup_table = [""]*64
    for line in lines:
        if line.strip() == "":
            continue
        
        if (line.startswith(" ")):
            key = ""
            value, state = line[2:].split(" ")
        else:
            key, value, state = line.split(" ")
        value = int(value, 2)

        lookup_table[value] = (key.lower(), state)
    
    return lookup_table

BRAILLE_CHARS = parse_braille(BRAILLE_DEFAULT)
BRAILLE_NUMS = parse_braille(BRAILLE_NUMBER)

def print_map(data):
    output = "{\n"

    for d in data:
        c = d[0]
        state = d[1]
        altgr = False
        if len(c) == 1 and c in KeyboardLayout.NEED_ALTGR:
            altgr = True
        
        if len(c) == 1:
            c_ord = ord(c)
            if c_ord < len(KeyboardLayout.ASCII_TO_KEYCODE):
                first = KeyboardLayout.ASCII_TO_KEYCODE[c_ord]
                second = 0
                if first == 0:
                    raise Exception("Not supported")
                
            elif c_ord in KeyboardLayout.COMBINED_KEYS.keys():
                first = (KeyboardLayout.COMBINED_KEYS[c_ord] >> 8) & 0xff
                sec = KeyboardLayout.COMBINED_KEYS[c_ord] & 0xff
                second = KeyboardLayout.ASCII_TO_KEYCODE[sec & 0x7F] | (sec & 0x80)
            elif c_ord in KeyboardLayout.HIGHER_ASCII.keys():
                first = KeyboardLayout.HIGHER_ASCII[c_ord]
                second = 0
            else:
                raise Exception("Not supported key: " + c)
        else:
            if len(c) > 1:
                raise Exception("Not supported")
            first = 0
            second = 0
        
        mod_str_1 = ""
        mod_str_2 = ""
        if altgr:
            if (second == 0):
                mod_str_1 += "ALTGR|"
            else:
                mod_str_2 += "ALTGR|"
        if (first & 0x80) != 0:
            mod_str_1 += "SHIFT|"
        if (second & 0x80) != 0:
            mod_str_2 += "SHIFT|"
        
        if len(mod_str_1) == 0:
            mod_str_1 = "NOMOD|"
        if len(mod_str_2) == 0:
            mod_str_2 = "NOMOD|"

        output += "  {{" + mod_str_1[:-1] + "," + f"0x{(first & 0x7F):02x}" + "},{" + mod_str_2[:-1] + "," + f"0x{(second &0x7F):02x}" + "}," + state + "}, // " + c + "\n"

    output += "};"
    print(output)


print("const struct KeySequence default_map[]", end=' = ')
print_map(BRAILLE_CHARS)

print("const struct KeySequence upper_map[]", end=' = ')
upper_map = [(c[0].upper() if c[0].upper() != c[0] else '', c[1]) for c in BRAILLE_CHARS]
upper_map[0b010001] = ('', 'HOLDUPPER')
print_map(upper_map)

print("const struct KeySequence holdupper_map[]", end=' = ')
holdupper_map = [(c[0].upper() if c[0].upper() != c[0] else '', 'HOLDUPPER' if c[0].upper() != c[0] else c[1]) for c in BRAILLE_CHARS]
print_map(holdupper_map)

print("const struct KeySequence number_map[]", end=' = ')
print_map(BRAILLE_NUMS)