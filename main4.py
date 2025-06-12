# vim: set ft=python fdm=indent :
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

DATATYPE=np.float32
DATETIME_FORMAT="%Y-%m-%d_%H-%M-%S"
np.set_printoptions(
    precision=3,
    floatmode='fixed',
    suppress=True,
    sign=' ',
    linewidth=100,
)

# Базисная функция
def N(U, m, n, i, t):
    if i == m-2 and t == U[m-1]:
        return DATATYPE(1)
    elif n == 0:
        return DATATYPE( U[i] <= t < U[i+1] )
    else:
        a = N_f(U, m, n, i, t)
        b = 1-N_f(U, m, n, i+1, t)
        x = N(U, m, n-1, i, t) if a != 0 else DATATYPE(0)
        y = N(U, m, n-1, i+1, t) if b != 0 else DATATYPE(0)
        return DATATYPE(a*x + b*y)

# Коэффициенты базисной функции
def N_f(U, m, n, i, t):
    a = DATATYPE(t - U[i])
    b = DATATYPE(U[i+n] - U[i])
    return DATATYPE(0) if b == 0 else a/b

def __todo_de_boor(U, P, k, n, t):
    default = numpy.array([0, 0], dtype='float64')
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

# B-сплайн
def Q(U, P, k, n, t):
    bb = np.array([N(U, k+n+1, n, i, t) for i in range(k)])
    return np.sum(P * bb[:,np.newaxis], axis=0)

def C(U, P, W, k, n, t):
    bb = np.array([N(U, k+n+1, n, i, t) for i in range(k)])
    ww = bb * W
    return np.sum(P * ww[:,np.newaxis], axis=0) / np.sum(ww)

def u(total_points, degree, i):
    if i <= degree:
        return 0
    elif i <= total_points:
        return (i - degree) / (total_points - degree)
    return 1

def main():
    # Управляющий вектор
    # Control points
    # P = np.array([[0, 0], [1, 1], [2, 0]], dtype=DATATYPE)
    # P = np.array([ [0.65, 0.21] , [1.2, 2.75] , [2.88, 0.35] , [3.2, 3.06] ], dtype=DATATYPE)
    P = np.array([
          [0.8, 0.4]
        , [1.4, 2.2]
        , [2.06, 0.38]
    ], dtype=DATATYPE)
    k = len(P)
    # Вектор весов
    # Weight vector
    W = np.array([
          1
        , 0.33
        , 1
    ], dtype=DATATYPE)
    # Степень кривой (n = степень, n+1 = порядок)
    n = 2
    # Расширенный узловой вектор
    # [...] knot vector
    # U = np.array([0, 0, 0, 3, 4, 5, 6, 7], dtype=DATATYPE)
    m = k + n + 1
    U = np.array([u(k, n, i) for i in range(m)], dtype=DATATYPE)
    # m = len(U)
    # print("knot vector", U)
    # print(Q(U, P, k, n, 0.98))
    # print(N(U, m, n, 2, 1))
    fig, ax = plt.subplots(figsize=(25, 25))
    ax.grid(True)
    samples = 100
    scale = 1.0 / samples # (U[m-1] - U[0]) / samples
    print(scale)
    all_samples = np.linspace(U[0], U[m-1], samples+1)
    # print(all_samples)
    all_points = np.array([C(U, P, W, k, n, t) for t in all_samples], dtype=DATATYPE)
    print(all_points)
    # basis_funcs = np.array([[[N(U, m, deg, i, t) for t in all_samples] for i in range(k)] for deg in range(3)], dtype=DATATYPE)
    # for q in basis_funcs_2:
    #     ax.plot(all_samples, q)
    # for q in basis_funcs[2]:
    #     ax.plot(all_samples, q)
    # ax.plot(all_samples, basis_funcs[1])
    ax.plot(all_points[:,0], all_points[:,1])
    fig.savefig("fig_{}.png".format(datetime.now().strftime(DATETIME_FORMAT)))
    plt.show(block=True)
    pass

if __name__== '__main__':
    main()

