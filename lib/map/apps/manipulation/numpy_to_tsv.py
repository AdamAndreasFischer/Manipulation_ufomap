#!/usr/bin/env python3

import sys
import numpy as np

"""
Args: 
1: Path to pcds and poses
2: Number of clouds per pose!
3: Convert to meters: true or false
4: Change quaternion direction from quat(x y z w) to (w x y z) as the config tells that the program expects scalar first

"""
def main() -> int: 
    if 2 > len(sys.argv):
        return 1

    path = sys.argv[1]

    repeating = 1
    if 3 <= len(sys.argv):
        repeating = int(sys.argv[2])

    to_meter = False
    if 4 <= len(sys.argv):
        to_meter = 'true' == sys.argv[3].lower()

    swap_qw = False
    if 5 <= len(sys.argv):
        swap_qw = 'true' == sys.argv[4].lower()

    with open(f'{path}/poses.tsv', 'w') as f:
        try:
            i = 1
            while True:
                data = np.load(f'{path}/pose_{i}_init_transformed.npy')
                if 7 != data.shape[0]:
                    sys.stderr.write(
                        f'Error: Expected 7 values per pose, got {data.shape[0]}.\n')
                    return 1
                if to_meter:
                    data[0:3] /= 1000.0
                if swap_qw:
                    data[3], data[4], data[5], data[6] = data[6], data[3], data[4], data[5]
                for j in range(0, repeating):
                    if 1 < (i + j):
                        f.write('\n')
                    np.savetxt(f, np.transpose(data), delimiter='',
                               newline='\t', fmt="%.8f")
                i += 1
        except OSError as error:
            pass

    return 0


if __name__ == '__main__':
    sys.exit(main())
