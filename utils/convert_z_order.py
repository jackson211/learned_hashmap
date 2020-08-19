import numpy as np
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


path = "../data/"
filename = "australia-oceania"
x = np.loadtxt(path + filename + ".txt", delimiter=" ")

x = np.apply_along_axis(convert_fn, 1, x).astype(np.uint64)
x = np.sort(x, axis=None)
print(x)

out = path + filename + "_uint64"
with open(out, "wb") as f:
    f.write(struct.pack("Q", len(x)))
    x.tofile(f)
