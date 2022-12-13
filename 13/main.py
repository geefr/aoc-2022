
import sys
import json
from functools import cmp_to_key

def parse_line(line):
    result = json.loads(line)
    return result

def compare(left, right):
    if isinstance(left, int) and isinstance(right, int):
        return left < right
    elif isinstance(left, list) and isinstance(right, list):
        for l,r in zip(left,right):
            if l == r:
                continue
            return compare(l,r)
        if len(left) < len(right):
            return True
        if len(right) < len(left):
            return False
        return None
    elif isinstance(left, list) and isinstance(right, int):
        return compare(left, [right])
    elif isinstance(left, int) and isinstance(right, list):
        return compare([left], right)

def compare_part2(left, right):
    # -ve if left < right
    # 0 if left == right
    # +ve if left > right
    if isinstance(left, int) and isinstance(right, int):
        if left < right:
            return -1
        if left > right:
            return 1
        else:
            return 0
    elif isinstance(left, list) and isinstance(right, list):
        for l,r in zip(left,right):
            x = compare_part2(l,r)
            if x != 0:
                return x
        if len(left) < len(right):
            return -1
        if len(right) < len(left):
            return 1
        return 0
    elif isinstance(left, list) and isinstance(right, int):
        return compare_part2(left, [right])
    elif isinstance(left, int) and isinstance(right, list):
        return compare_part2([left], right)

def main(input_filename):
    left_lines = []
    right_lines = []

    line_type = 0

    with open(input_filename) as input_file:
        for line in input_file:
            if line_type == 0:
                left_lines.append(parse_line(line))
            elif line_type == 1:
                right_lines.append(parse_line(line))
            else:
                pass
            line_type += 1
            if line_type == 3:
                line_type = 0

    correct = []

    for l, r in zip(left_lines, right_lines):
        # print(f"Left: {l}\nRight: {r}")
        correct_order = compare(l, r)
        if correct_order is None:
            raise RuntimeError("Whoops!")
        correct.append(correct_order)
    
    # Problem 1
    sum_correct_indices = 0
    for i in range(len(correct)):
        if correct[i]:
            sum_correct_indices += i + 1
    print(f"Problem 1: {sum_correct_indices}\n\n")

    # Problem 2
    packets = [*left_lines, *right_lines]
    packets.append([[2]])
    packets.append([[6]])
    packets = sorted(packets, key=cmp_to_key(compare_part2))
    # for p in packets:
    #     print(p)
    
    a = packets.index([[2]]) + 1
    b = packets.index([[6]]) + 1
    print(f"Divider packet 1 at: {a}")
    print(f"Divider packet 2 at: {b}")
    print(f"Problem 2: {a*b}")


if __name__ == '__main__':
    main("13/input.txt")
