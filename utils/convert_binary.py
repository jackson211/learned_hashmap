import numpy as np
import os
import struct
import sys


if __name__ == "__main__":
    filepath = sys.argv[1]
    dirname = os.path.dirname(filepath)
    base = os.path.basename(filepath)
    filename = os.path.splitext(base)[0]

    print("Reading file: " + filepath)
    x = np.loadtxt(filepath, dtype=np.float64, delimiter=" ")
    # x = np.apply_along_axis(convert_fn, 1, x).astype(np.uint64)
    # x = np.sort(x, axis=None)
    print(x)

    out = os.path.join(dirname, filename + "_uint64")
    print("Writing file: " + out)
    with open(out, "wb") as f:
        f.write(struct.pack("Q", len(x)))
        x.tofile(f)
