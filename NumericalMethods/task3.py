# -*- coding: utf-8 -*-

from decimal import *
import math
import operator

places = None

def Normalize(d):
	return d.quantize(Decimal('.%s1' % ('0'*(places - 1))), rounding=ROUND_HALF_UP)

class RoundedDecimal:
	def __init__(self, num):
		self.num = Normalize(num)

	def __add__(self, other):
		return RoundedDecimal(self.num + other.num)

	def __sub__(self, other):
		return RoundedDecimal(self.num - other.num)

	def __mul__(self, other):
		return RoundedDecimal(self.num * other.num)

	def __div__(self, other):
		return RoundedDecimal(self.num / other.num)

	def __cmp__(self, other):
		return cmp(self.num, other.num)

	def __repr__(self):
		return str(self.num)

def C(s):
	return RoundedDecimal(Decimal(s))

def CreateL():
	return [
		[C('0')]*4,
		[C('0')]*4,
		[C('0')]*4
	]

def CreateU():
	return [
		[C('1'), C('0'), C('0')],
		[C('0'), C('1'), C('0')],
		[C('0'), C('0'), C('1')]
	]

n = 3
m = 4

def PrintMatrix(D):
	print "\n".join(map(lambda row: " ".join(map(str, row)), D))

def PrintUnicode(s):
	print s.encode('utf-8')
	
def SolveA(D):
	global m, n
	for i in range(n):
		#PrintUnicode(u"Итерация %d" % (i + 1))
		if i + 1 < n:
			#PrintUnicode(u"До поиска главного элемента:")
			#PrintMatrix(D)
			toSwap = max(xrange(i + 1, n), key=lambda z: max(D[z]))
			D[toSwap], D[i] = D[i], D[toSwap]
			#PrintUnicode(u"После поиска главного элемента:")
			#PrintMatrix(D)
		pivot = D[i][i]
		for j in range(i + 1, n):
			toMul = D[j][i]/pivot
			D[j][i] = C('0')
			for k in range(i + 1, m):
				D[j][k] -= D[i][k]*toMul
		for j in range(i, 4):
			D[i][j] /= pivot
		#PrintUnicode(u"После проведения одного шага метода:")
		#PrintMatrix(D)

	x3 = D[2][3]
	x2 = D[1][3] - D[1][2]*x3
	x1 = D[0][3] - D[0][1]*x2 - D[0][2]*x3

	return (x1, x2, x3)

def SolveB(D):
	global m, n
	L = CreateL()
	U = CreateU()
	for i in xrange(n):
		L[i][0] = D[i][0]
	for i in xrange(1, n):
		U[0][i] = D[0][i]/L[0][0]
		L[i][1] = D[i][1] - L[i][0]*U[0][1]

	U[1][2] = (D[1][2] - L[1][0]*U[0][2])/L[1][1]
	L[2][2] = D[2][2] - L[2][0]*U[0][2] - L[2][1]*U[1][2]

	y1 = D[0][3]/L[0][0]
	y2 = (D[1][3] - L[1][0]*y1)/L[1][1]
	y3 = (D[2][3] - L[2][0]*y1 - L[2][1]*y2)/L[2][2]

	x3 = y3
	x2 = y2 - U[1][2]*x3
	x1 = y1 - U[0][1]*x2 - U[0][2]*x3

	return (x1, x2, x3)

def CreateD():
	return [
		[C('1.2345'), C('3.1415'), C('1'), C('9.2575')],
		[C('2.3456'), C('5.969'), C('0'), C('14.2836')],
		[C('3.4567'), C('2.1828'), C('3.4'), C('13.7383')],
	]


def Solve(solver, method_desc):
	global places
	places = 10
	trueSolution = (C('1'), C('2'), C('1.74'))
	for p in [2, 4, 6]:
		places = p
		sln = solver(CreateD())
		PrintUnicode(u"Решение, найденное по %s, с точностью %d знаков: %s" % (method_desc, p, sln))
		places = 15
		z = map(lambda (a, b): (a - b)*(a - b), zip(sln, trueSolution))
		PrintUnicode(u"Норма погрешности: %.7f" % (math.sqrt(reduce(operator.add, z).num)))
		

Solve(SolveA, u"методу Гаусса")
Solve(SolveB, u"компактной схеме метода Гаусса")