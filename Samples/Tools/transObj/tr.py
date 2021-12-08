from io import TextIOWrapper
import sys

vertSet = []
coordSet = []
normSet = []
oface = []
idx = 0

def processV(frag : list):
    global vertSet
    print('vertex ==> ')
    vertSet.append((float(frag[1]), float(frag[2]), float(frag[3])))

def processVt(frag : list):
    global coordSet
    print('coordinate ==> ')
    coordSet.append((float(frag[1]), float(frag[2])))

def processVn(frag : list):
    print('normal ==> ')
    global normSet
    normSet.append((float(frag[1]), float(frag[2]), float(frag[3])))

def processF(frag : list, fw : TextIOWrapper):
    global idx
    global normSet
    global vertSet
    global coordSet
    print('face ==> ')
    vs = []
    for i in range(1, len(frag)):
        vs.append(idx)
        tri = frag[i].split('/')
        v = vertSet[int(tri[0]) - 1]
        c = coordSet[int(tri[1]) - 1]
        n = normSet[int(tri[2]) - 1]
        fw.write(str(v[0]) + ' ' + str(v[1]) + ' ' + str(v[2]) + \
            ' ' + str(c[0]) + ' ' + str(c[1]) + \
            ' ' + str(n[0]) + ' ' + str(n[1]) + ' ' + str(n[2]) + '\n')
        idx += 1
    oface.append(vs)
    

def getLine(s : str, fw : TextIOWrapper):
    index = s.find('#')
    if index >= 0:
        s = s[: index]
    
    s = s.strip('\n')
    frag = s.split(' ')
    
    for i in frag:
        print('[' + i + ']', end = '')
    print()
    
    if frag[0] == 'v':
        processV(frag)
    if frag[0] == 'vt':
        processVt(frag)
    if frag[0] == 'vn':
        processVn(frag)
    if frag[0] == 'f':
        processF(frag, fw)
    

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
                print(s)
                getLine(s, fw)
                if not s: 
                    break
            fw.write('VEND\n')
            for i in oface:
                s = str(len(i)) + ' '
                for v in i:
                    s += str(v) + ' '
                fw.write(s + '\n')
            fw.write('FEND\n')
                
    