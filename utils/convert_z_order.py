import numpy as np
import os
import sys
import struct
import pymorton as pm


def numberToBase(n, b):
    if n == 0:
        return [0]
    digits = []
    while n:
        digits.append(int(n % b))
        n //= b
    return ''.join(map(str, digits[::-1]))


def convert_fn(row):
    return int(pm.interleave_latlng(row[0], row[1]), 4)


if __name__ == '__main__':
    filepath = sys.argv[1]
    dirname = os.path.dirname(filepath)
    base = os.path.basename(filepath)
    filename = os.path.splitext(base)[0]
    print("Reading file: " + filepath)
    x = np.loadtxt(filepath, delimiter=" ")
    x = np.apply_along_axis(convert_fn, 1, x).astype(np.uint64)
    x = np.sort(x, axis=None)
    print(x.size)

    out = os.path.join(dirname, filename + "_uint64")
    print("Writing file: " + out)
    with open(out, "wb") as f:
        f.write(struct.pack("Q", len(x)))
        x.tofile(f)
