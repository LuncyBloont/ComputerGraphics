from io import TextIOWrapper
import sys

vertSet = []
coordSet = []
normSet = []
oface = []
idx = 0
content = []

def processV(frag : list):
    global vertSet
    vertSet.append((float(frag[1]), float(frag[2]), float(frag[3])))

def processVt(frag : list):
    global coordSet
    coordSet.append((float(frag[1]), float(frag[2])))

def processVn(frag : list):
    global normSet
    normSet.append((float(frag[1]), float(frag[2]), float(frag[3])))

def processF(frag : list):
    global idx
    global normSet
    global vertSet
    global coordSet
    global content
    vs = []
    for i in range(1, len(frag)):
        vs.append(idx)
        tri = frag[i].split('/')
        v = vertSet[int(tri[0]) - 1]
        if len(tri[1]) > 0:
            c = coordSet[int(tri[1]) - 1]
        else:
            c = (0., 0.)
        if len(tri[2]) > 0:
            n = normSet[int(tri[2]) - 1]
        else:
            n = (0., 0., 1.)
        content.append(str(v[0]) + ' ' + str(v[1]) + ' ' + str(v[2]) + \
            ' ' + str(c[0]) + ' ' + str(c[1]) + \
            ' ' + str(n[0]) + ' ' + str(n[1]) + ' ' + str(n[2]) + '\n')
        idx += 1
    oface.append(vs)
    

def getLine(s : str):
    index = s.find('#')
    if index >= 0:
        s = s[: index]
    
    s = s.strip('\n')
    frag = s.split(' ')
    
    if frag[0] == 'v':
        processV(frag)
    if frag[0] == 'vt':
        processVt(frag)
    if frag[0] == 'vn':
        processVn(frag)
    if frag[0] == 'f':
        processF(frag)
    

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("too less arguments.")
        exit(-1)
    fname = sys.argv[1]
    oname = sys.argv[2]
    idx = 0
    with open(fname, 'r') as fr:
        with open(oname, 'w') as fw:
            s = ''
            while True:
                s = fr.readline()
                getLine(s)
                if not s: 
                    break
            print('READED.')
            for i in oface:
                s = str(len(i)) + ' '
                for v in i:
                    s = s + str(v) + ' '
                content.append(s + '\n')
            print("WRITING...")
            fw.write(str(idx) + ' ' + str(len(oface)) + '\n')
            fw.write(''.join(content))
            print('DONE')
                
    