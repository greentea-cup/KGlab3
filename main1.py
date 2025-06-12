# vim: set ft=python fdm=indent :
import numpy

"""
    The NURBS Book - Les Piegl, Wayne Tiller
"""

def gen_knots(n: int, p: int):
    knots = [0] * (n + p + 2)
    # for i in range(p+1):
    #     knots[i] = 0
    scale = 1.0 / (n-p+1)
    for i in range(p+1, n+1):
        knots[i] = (i-p) * scale
    for i in range(n+1, n+p+2):
        knots[i] = 1
    return knots

"""
    n - количество точек - 1 (?)
    p - степень кривой
    u - точка для расчёта значений кривой
    U - узловой вектор
"""
def find_span(n, p, u, U):
    if u == U[n+1]:
        return n
    low = p
    high = n+1
    mid = low + (high - low) // 2
    while (u < U[mid] or u >= U[mid+1]):
        if u < U[mid]:
            high = mid
        else:
            low = mid
        mid = low + (high - low) // 2
    return mid

def basis_funcs(i, u, p, U):
    N = [0] * (p+1)
    left = [0] * (p+1)
    right = [0] * (p+1)
    N[0] = 1.0
    for j in range(1, p+1):
        left[j] = u - U[i+1-j]
        right[j] = U[i+j] - u
        saved = 0.0
        for r in range(j):
            temp = N[r] / (right[r+1] + left[j-r])
            N[r] = saved + right[r+1] * temp
            saved = left[j-r] * temp
        N[j] = saved
    return N

def curve_point(n, p, U, P, u, P0):
    span = find_span(n, p, u, U)
    N = basis_funcs(span, u, p, U)
    C = P0
    for i in range(p+1):
        print(P[span-p+1], N[i]*P[span-p+1])
        C = C + N[i]*P[span-p+1]
    return C

def main():
    P = numpy.array([
        [0, 0], [0.2, 0.6], [0.8, 0.8], [1, 0]
    ], dtype='float64')
    samples = 50
    scale = 1.0 / samples
    n = 3
    p = 3
    U = [0, 0, 0, 0, 1, 1, 1, 1]
    # U = gen_knots(n, p)
    for i in range(samples+1):
        u = scale * i
        C = curve_point(
            n, p, U, P, u, numpy.array([0, 0], dtype='float64'))
        print(C)

if __name__ == '__main__':
    main()

