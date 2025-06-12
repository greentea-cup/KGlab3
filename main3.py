# vim: set ft=python fdm=indent :
import numpy as np
import matplotlib.pyplot as plt

"""
i - ith control point
n - nth degree basis function
t - parameter
"""

def f(U, i, n, t):
    return (t - U[i]) / (u[i+n] - u[i])

def g(U, i, n, t):
    return 1 - f(U, i, n, t)

def N(U, i, n, t):
    if n == 0:
        return 1 if U[i] <= t <= U[i+1] else 0
    a = f(U, i, n, t)
    b = g(U, i+1, n, t)
    N1 = N(U, i, n-1, t)
    N2 = N(U, i+1, n-1, t)
    return a*N1 + b*N2

