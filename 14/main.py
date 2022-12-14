
import sys
from dataclasses import dataclass
from typing import List, Dict
from enum import Enum
from copy import deepcopy
import time

# WHEE! :D
# (Set sand to None for input.txt)
ANIMATION_SAND = None
ANIMATION_MAP = 0.0001
ANIMATION_EVERY_N = 100

class MapVal(Enum):
    Empty = 1
    Wall = 2
    Sand = 3

@dataclass
class Point:
    x: int
    y: int

@dataclass
class SandMap:
    topleft: Point
    bottomright: Point
    data: Dict
    has_infinite_floor: bool

    def __init__(self, has_infinite_floor: bool = False) -> None:
        self.topleft = Point(x=1e10,y=1e10)
        self.bottomright = Point(x=-1e10,y=-1e10)
        self.data = {}
        self.has_infinite_floor = has_infinite_floor

    def set(self, p: Point, v: MapVal):
        self.data[str(p)] = v
        self.topleft.x = min(self.topleft.x,p.x)
        self.topleft.y = min(self.topleft.y,p.y)
        self.bottomright.x = max(self.bottomright.x,p.x)
        self.bottomright.y = max(self.bottomright.y,p.y)

    def point_is(self, p: Point, v: MapVal):

        if self.has_infinite_floor:
            if p.y >= self.bottomright.y + 2:
                if v == MapVal.Wall:
                    return True
                else:
                    return False

        x = self.data.get(str(p))
        if x is None:
            if v == MapVal.Empty:
                return True
            else:
                return False
        return x == v
    
    def sand_point_is_valid(self, p: Point):
        # Sand may fall in from the top
        valid = True
        if p.x < self.topleft.x or \
           p.x > self.bottomright.x:
            valid = False
        if not self.has_infinite_floor and \
           p.y > self.bottomright.y:
            valid = False

        return valid

    def __str__(self) -> str:
        s = f"TL: {self.topleft} BR: {self.bottomright}\n"
        for y in range(self.topleft.y, self.bottomright.y + 1):
            for x in range(self.topleft.x, self.bottomright.x + 1):
                if self.point_is(Point(x,y), MapVal.Wall):
                    s += "#"
                elif self.point_is(Point(x,y), MapVal.Sand):
                    s += "o"
                else:
                    s += "."
            s += "\n"
        if self.has_infinite_floor:
            for x in range(self.topleft.x, self.bottomright.x + 1):
                y = self.bottomright.y + 1
                if self.point_is(Point(x,y), MapVal.Wall):
                    s += "#"
                elif self.point_is(Point(x,y), MapVal.Sand):
                    s += "o"
                else:
                    s += "."
            s += "\n"
            for x in range(self.topleft.x, self.bottomright.x + 1):
                s += "#"
        return s


def parse_lines(filename: str) -> SandMap:
    # 466,137 -> 466,138 -> 484,138
    map = SandMap()
    with open(filename) as input_file:
        for line in input_file:
            coord_tokens = line.split('->')
            coords = []
            for coord_token in coord_tokens:
                point_tokens = coord_token.split(',')
                point = Point(int(point_tokens[0]), int(point_tokens[1]))
                coords.append(point)
            
            if len(coords) < 2:
                continue
            
            i = 0
            while i != len(coords) - 1:
                c = coords[i]
                cnext = coords[i+1]

                # print(f"{c} -> {cnext}")

                if c.x == cnext.x:
                    if c.y > cnext.y:
                        c, cnext = cnext, c
                    points = [Point(c.x, y) for y in range(c.y, cnext.y + 1)]
                elif c.y == cnext.y:
                    if c.x > cnext.x:
                        c, cnext = cnext, c
                    points = [Point(x, c.y) for x in range(c.x, cnext.x + 1)]
                else:
                    raise RuntimeError
                for p in points:
                    # print(f"WALL: {p}")
                    map.set(p, MapVal.Wall)
                i += 1

    return map

def _drop_sand(m: SandMap, sand_pos: Point) -> Point:
    """
    Drop sand by one step
    - Returns the new point
    - If sand cannot move returns sand_pos
    - If sand leaves map returns new point (caller checks bounds)
    """
    p = Point(sand_pos.x, sand_pos.y+1)
    if m.point_is(p, MapVal.Empty):
        return p
    p = Point(sand_pos.x-1, sand_pos.y+1)
    if m.point_is(p, MapVal.Empty):
        return p
    p = Point(sand_pos.x+1, sand_pos.y+1)
    if m.point_is(p, MapVal.Empty):
        return p
    return sand_pos


def drop_sand_problem_1(m: SandMap, spawn_point: Point) -> SandMap:
    """Drop sand until it leaves the map"""
    map = deepcopy(m)

    while True:
        sand_pos = spawn_point
        new_sand_pos = None
        point_is_valid = True
        while True:
            new_sand_pos = _drop_sand(map, sand_pos)
            point_is_valid = map.sand_point_is_valid(new_sand_pos)
            if not point_is_valid:
                break
            if sand_pos == new_sand_pos:
                break
            sand_pos = new_sand_pos

            if ANIMATION_SAND:
                map2 = deepcopy(map)
                map2.set(new_sand_pos, MapVal.Sand)
                print(map2)
                time.sleep(ANIMATION_SAND)


        if not point_is_valid:
            # Sand now falling out of bounds
            break
        map.set(new_sand_pos, MapVal.Sand)
        if ANIMATION_MAP:
            print(map)
            time.sleep(ANIMATION_MAP)

    return map


def drop_sand_problem_2(m: SandMap, spawn_point: Point) -> SandMap:
    """
    Drop sand until the source is blocked
    Also the map has an infinitely wide floor at y=bottomleft.y + 2
    """
    map = deepcopy(m)
    map.has_infinite_floor = True

    anim_n = 0
    while True:
        sand_pos = spawn_point
        new_sand_pos = None
        point_is_valid = True
        while True:
            new_sand_pos = _drop_sand(map, sand_pos)
            if sand_pos == new_sand_pos:
                break
            sand_pos = new_sand_pos

            if ANIMATION_SAND:
                map2 = deepcopy(map)
                map2.set(new_sand_pos, MapVal.Sand)
                print(map2)
                time.sleep(ANIMATION_SAND)

        if new_sand_pos.y == map.bottomright.y + 1:
            # Sand has reached the infinite floor
            y = map.bottomright.y
            p = Point(new_sand_pos.x, new_sand_pos.y)
            map.set(p, MapVal.Sand)
            map.bottomright.y = y
        else:
            map.set(new_sand_pos, MapVal.Sand)

        if new_sand_pos.x == spawn_point.x and new_sand_pos.y == spawn_point.y:
            break
        
        anim_n += 1
        if anim_n == ANIMATION_EVERY_N:
            if ANIMATION_MAP:
                print(map)
                time.sleep(ANIMATION_MAP)
            anim_n = 0

    return map


def main(input_filename):
    map = parse_lines(input_filename)

    print(map)

    spawn_point = Point(500,0)
    # map = drop_sand_problem_1(map, spawn_point)

    # print(f"Problem 1: {list(map.data.values()).count(MapVal.Sand)}")

    map = drop_sand_problem_2(map, spawn_point)
    print(f"Problem 2: {list(map.data.values()).count(MapVal.Sand)}")
    


if __name__ == '__main__':
    main(sys.argv[1])
