
import sys
from dataclasses import dataclass
from typing import List, Dict, Tuple, Any
from enum import Enum
from copy import deepcopy, copy
import time
from multiprocessing import Pool

class MapVal(Enum):
    Empty = 1
    Sensor = 2
    Beacon = 3
    Marked = 4

@dataclass
class Point:
    x: int
    y: int

    def __mul__(self, v: object):
        if isinstance(v,int):
            return Point(self.x * v, self.y * v)
        else:
            raise TypeError
    
    def __add__(self, v: object):
        if isinstance(v,int):
            return Point(self.x + v, self.y + v)
        elif isinstance(v,Point):
            return Point(self.x + v.x, self.y + v.y)
        else:
            raise TypeError
    
    def __eq__(self, o: object) -> bool:
        if isinstance(o,Point):
            return self.x == o.x and self.y == o.y
        return False

@dataclass
class Sensor:
    position: Point
    closest_beacon: Point
    dist: int

@dataclass
class Map:
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

    def get(self, p: Point):
        return self.data.get(str(p))

    def point_is(self, p: Point, v: MapVal):

        x = self.data.get(str(p))
        if x is None:
            if v == MapVal.Empty:
                return True
            else:
                return False
        return x == v
    
    def point_is_valid(self, p: Point):
        if p.x < self.topleft.x or \
           p.x > self.bottomright.x or \
           p.y < self.bottomright.y or \
           p.y > self.bottomright.y:
            return False
        return True

    def __str__(self) -> str:
        s = f"TL: {self.topleft} BR: {self.bottomright}\n"
        for y in range(self.topleft.y, self.bottomright.y + 1):
            for x in range(self.topleft.x, self.bottomright.x + 1):
                if self.point_is(Point(x,y), MapVal.Sensor):
                    s += "S"
                elif self.point_is(Point(x,y), MapVal.Beacon):
                    s += "B"
                elif self.point_is(Point(x,y), MapVal.Marked):
                    s += "#"
                else:
                    s += "."
            s += "\n"
        return s


def parse_lines(filename: str) -> Tuple[Map, List[Sensor]]:
    # 466,137 -> 466,138 -> 484,138
    map = Map()
    sensors = []
    with open(filename) as input_file:
        for line in input_file:
            
            line_tok = line.split(":")
            sensor_line = line_tok[0]
            sensor_line = sensor_line[sensor_line.find("x=") + 2:]
            sensor_tok = sensor_line.split(',')
            sensor_tok[0] = sensor_tok[0].replace("x=","")
            sensor_tok[1] = sensor_tok[1].replace("y=","")
            sensor = Point(int(sensor_tok[0]), int(sensor_tok[1]))


            beacon_line = line_tok[1]
            beacon_line = beacon_line[beacon_line.find("x=") + 2:]
            beacon_tok = beacon_line.split(',')
            beacon_tok[0] = beacon_tok[0].replace("x=","")
            beacon_tok[1] = beacon_tok[1].replace("y=","")
            beacon = Point(int(beacon_tok[0]), int(beacon_tok[1]))

            map.set(sensor, MapVal.Sensor)
            map.set(beacon, MapVal.Beacon)

            sensors.append(Sensor(sensor, beacon, manhattan(sensor, beacon)))

    return map, sensors

def manhattan(a: Point, b: Point) -> int:
    return abs(a.x - b.x) + abs(a.y - b.y)

def problem1(map: Map, sensors: List[Sensor]) -> None:
    # row = 10
    row = 2000000
    cant_be_beacon = 0

    min_x = map.topleft.x
    max_x = map.bottomright.x
    for sensor in sensors:
        min_x = min(min_x, sensor.position.x - manhattan(sensor.position, sensor.closest_beacon))
        max_x = max(max_x, sensor.position.y + manhattan(sensor.position, sensor.closest_beacon))

    for x in range(min_x, max_x + 1):
        p = Point(x, row)
        if map.point_is(p, MapVal.Beacon):
            continue
        
        for sensor in sensors:
            sensor_to_nearest = manhattan(sensor.position, sensor.closest_beacon)
            if manhattan(sensor.position, p) <= sensor_to_nearest:
                cant_be_beacon += 1
                break

    print(f"In row {row}, {cant_be_beacon} positions can't contain a beacon")

def _problem2_loop(args: List) -> bool:
    sensors = args[0]
    y = args[1]
    x_min = args[2]
    x_max = args[3]

    # t1 = time.time()
    if y % 1000 == 0:
        print(f"y={y} process", flush=True)

    x = x_min
    while True:
        covered_by_a_sensor = False
        for sensor in sensors:
            dy = abs(y - sensor.position.y)
            dx = sensor.dist - dy
            if dx <= 0:
                continue

            sensor_min_x = sensor.position.x - dx
            sensor_max_x = sensor.position.x + dx

            if x >= sensor_min_x and x <= sensor_max_x:
                # Covered by a sensor, beacon can't be here
                x = sensor_max_x
                covered_by_a_sensor = True
                break
        
        if x >= x_max:
            break

        if covered_by_a_sensor:
            x += 1
            continue

        tuning_freq = (x * 4000000) + y
        print(f"Found beacon at {x,y} freq {tuning_freq}")
        return True

    return False


def problem2(map: Map, sensors: List[Sensor]) -> None:
    min_y = 0
    max_y = 4000000
    # max_y = 20

    min_x = 0
    # max_x = 20
    max_x = 4000000

    # for sensor in sensors:
    #     min_x = min(min_x, sensor.position.x - manhattan(sensor.position, sensor.closest_beacon))
    #     max_x = max(max_x, sensor.position.y + manhattan(sensor.position, sensor.closest_beacon))

    y_vals = range(min_y, max_y)

    # rows_processed = 0
    # with Pool() as pool:
    #     args = [(sensors, y, min_x, max_x) for y in y_vals]
    #     for result in pool.map(_problem2_loop, args):
    #         if result:
    #             break
    
    for y in y_vals:
        args = (sensors, y, min_x, max_x)
        if _problem2_loop(args):
            break

def main(input_filename):
    map, sensors = parse_lines(input_filename)

    # print(map)
    # problem1(map, sensors)
    problem2(map, sensors)
    


if __name__ == '__main__':
    main(sys.argv[1])
