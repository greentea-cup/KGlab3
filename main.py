# vim: set ft=python fdm=indent :
import numpy

def guess_k(n: int, target_k: int):
    return n+1 if n + 2 <= target_k else target_k

def gen_knots(n: int, k: int):
    knots = [0] * (n + k + 2)
    # for i in range(k+1):
    #     knots[i] = 0
    scale = 1.0 / (n-k+1)
    for i in range(k+1, n+1):
        knots[i] = (i-k) * scale
    for i in range(n+1, n+k+2):
        knots[i] = 1
    return knots

def find_bounds(knots, t: float):
    for i in range(len(knots) - 1):
        if knots[i] <= t <= knots[i+1]:
            return i
    return None

def map_samples(k, knots, samples):
    scale = 1.0 / samples
    s = k
    for i in range(samples+1):
        t = scale * i
        if t > knots[s+1]: s += 1
        yield t, s

def de_boor(s: int, t: float, knots, points, k: int):
    """Вычисление сплайна S(x) по алгоритму де Бура.
        s: Индекс сегмента, содержащего t.
        t: Точка, в которой требуется вычислить сплайн.
        knots: Массив узлов (расширенный дополнительными узлами, см. статью).
        points: Массив контрольных точек.
        k: Степень сплайна.
    """
    default = numpy.array([0, 0, 0], dtype='float64')
    d = [points[j + s - k] if 0 <= j+s-k < len(points) else default for j in range(0, k + 1)]
    for r in range(1, k + 1):
        for j in range(k, r - 1, -1):
            i1 = j + s - k
            i2 = j + 1 + s - r
            if (i1 < 0
                or i1 >= len(knots)
                or i2 < 0
                or i2 >= len(knots)):
                d[j] = default
                continue
            a = (t - knots[i1])
            b = (knots[i2] - knots[i1])
            if b == 0:
                alpha = 0
            else:
                alpha = a / b
            d[j] = (1.0 - alpha) * d[j - 1] + alpha * d[j]

    return d[k]

def main():
    points = numpy.array([
        [-1, 0, 0], [0, 1, 0], [0, -1, 0], [1, 0, 0]
    ], dtype='float64')
    print("points", points)
    n = len(points)
    print("n =", n)
    k = guess_k(n, 3)
    print("k =", k)
    knots = gen_knots(n, k)
    print("knots", knots)
    samples = 50
    scale = 1.0 / samples
    print("samples =", samples, "scale =", scale)
    for i in range(samples+1):
        t = scale * i
        s = find_bounds(knots, t)
        q = de_boor(s, t, knots, points, k)
        if q is not None:
            # print("t =", t, "s =", s, "q =", q)
            print(q)
    return 0

if __name__ == '__main__':
    exit(main())

