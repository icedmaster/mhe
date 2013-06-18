def print_enum_element(name, number):
    print('\t' + 'key_' + name + ' = ' + str(number) + ',')

print('enum\n{')
# numbers
print('\t// numbers')
for n in range(ord('0'), ord('9') + 1):
    print_enum_element(chr(n), n)
# letters
print('\t// small letters')
for l in range(ord('a'), ord('z') + 1):
    print_enum_element(chr(l), l)
print('\t// capital letters')
for l in range(ord('A'), ord('Z') + 1):
    print_enum_element(chr(l), l)
# special symbols
names = ['return', 'space', 'up' ,'down', 'right', 'left']
symbols = [13, 32, 273, 274, 275, 276]
print('\t// different keys')
i = 0
for n in names:
    print_enum_element(n, symbols[i])
    i = i + 1
# f1 - f15
print('\t// f1 - f15')
f_first = 282
for n in range(1, 16):
    print_enum_element('f' + str(n), f_first)
    f_first = f_first + 1
print('};')


    
