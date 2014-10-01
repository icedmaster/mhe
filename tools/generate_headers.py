import sys
import os
import glob

directories = ['core', 'math', 'utils', 'app', 'events', 'render', 'game', 'res', 'scene', 'threads', 'net', 'debug']

def get_header(name):
    s = '// This file has been generated by the tools/generate_headers.py script\n\n'
    include_guard = '__MHE_' + name.upper() + '__'
    s = s + '#ifndef ' + include_guard + '\n'
    s = s + '#define ' + include_guard + '\n\n'
    return s

def get_fileend(name):
    include_guard = '__MHE_' + name.upper() + '__'    
    return '#endif // ' + include_guard

def get_includes(name, prev_path = ''):
    symbols_to_exclude = ['#', '~', '_p.']
    s = ''
    os.chdir(name)
    for f in glob.glob('*.hpp'):
        exclude = False
        for sym in symbols_to_exclude:
            if f.find(sym) >= 0:
                exclude = True
                break
        if exclude == True: continue
        filename = prev_path + name + '/' + f
        print('Include:' + filename)
        s += '#include "' + filename + '"\n'
    s += '\n'
    for d in os.listdir('.'):
        if not os.path.isdir(d): continue
        s += get_includes(d, prev_path + name + '/')
    os.chdir('..')
    return s

def fill_file(f, name):
    f.write(get_header(name))
    f.write(get_includes(name))
    f.write(get_fileend(name))

os.chdir('../include/')

main_filename = 'mhe.hpp'
main_file = open(main_filename, 'w')
main_file.write(get_header('mhe'))

for d in directories:
    filename = 'mhe_' + d + '.hpp'
    #try:
    f = open(filename, 'w')
    fill_file(f, d)
    f.close()
    include = '#include "' + filename + '"\n'
    main_file.write(include)
    #except:
    #    print('Error while opening file ' + filename)
# final file
main_file.write(get_fileend('mhe'))
main_file.close()
