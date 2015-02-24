import operator
import math

def S0(n): return (n + a)/(n*(n**2 + b))
def S1(n): return (a*n - b)/(n**2*(n**2 + b))
def S2(n): return -(n*b + a*b)/(n**3*(n**2 + b))

def Sum(f, upper_bound):
	return reduce(operator.add, map(f, range(1, upper_bound + 1)))
	
a = 1.4
b = 1.6

A = math.pi**2/6
B = 1.202056903159594285399 * a

eps = 1e-5

(n0, n1, n2) = map(int, ((a + 1)/eps*2, math.sqrt(a / eps), math.pow(4*b / (3*eps), 1.0/3.0)))
print "n0 = %d, n1 = %d, n2 = %d" % (n0, n1, n2)

(s0, s1, s2) = (Sum(S0, n0), Sum(S1, n1), Sum(S2, n2))
print "S0 = %.5f" % s0
print "S0 (%.5f) = S1 (%.5f) + A (%.5f)" % (s1 + A, s1, A)
print "S0 (%.5f) = S2 (%.5f) + A (%.5f) + B (%.5f)" % (s2 + A + B, s2, A, B)