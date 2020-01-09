with open('/tmp/test.dat', 'rb') as testdat:
    lsof_out_bytes = testdat.read()
    for out_byte in lsof_out_bytes:
        if out_byte == 0:
            print('\\0', end='')
        elif out_byte == 10:
            print('\\n')
        else:
            print(out_byte.to_bytes(1, 'little').decode('UTF-8'), end='')
