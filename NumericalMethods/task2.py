# -*- coding: utf-8 -*-
import math

def F(x): return math.log10(x) - 0.19/x
def DF(x): return 0.19/x**2 + 1/(x*math.log(10))
def DDF(x): return -0.38/x**3 - 1/(x**2*math.log(10))

a = 1.3
b = 1.5
iterCount = 0

def BinarySearch(eps, onIteration):
	left = a
	right = b
	while math.fabs(right - left) > eps:
		onIteration()
		mid = (left + right)/2
		if F(mid) > 0:
			right = mid
		else:
			left = mid
	return (left + right)/2

def ShouldContinue(xs, eps):
	return math.fabs(xs[-1] - xs[-2]) > eps

def NextPointByChord(a, b):
	return (b*F(a) - a*F(b))/(F(a) - F(b))

def SecantMethod(p1Extractor):
	def SecantMethod_(eps, onIteration):
		xs = [a, b]
		while ShouldContinue(xs, eps):
			onIteration()
			xs.append(NextPointByChord(p1Extractor(xs), xs[-1]))
		return xs[-1]
	return lambda eps, onIteration: SecantMethod_(eps, onIteration)

def NewtonsMethod(eps, onIteration):
	def Next(x):
		return x - F(x)/DF(x)
	xs = [a, Next(a)]
	while ShouldContinue(xs, eps):
		onIteration()
		xs.append(Next(xs[-1]))
	return xs[-1]

def ParabolaMethod(eps, onIteration):
	xs = [a, b]
	M = max(math.fabs(DDF(a)), math.fabs(DDF(b)))
	while ShouldContinue(xs, eps):
		onIteration()
		last = xs[-1]
		next = -DF(last) + math.sqrt(DF(last)**2 + 2*M*F(last))
		xs.append(last + next / -M)
	return xs[-1]
	
solvers = [
	(u"Метод половинного деления", BinarySearch),
	(u"Метод неподвижных хорд", SecantMethod(lambda xs: xs[0])), 
	(u"Метод подвижных хорд", SecantMethod(lambda xs: xs[-2])),
	(u"Метод Ньютона", NewtonsMethod),
	(u"Метод парабол", ParabolaMethod),
]

def UnicodePrint(s):
	print s.encode('utf-8')

for solver in solvers:
	UnicodePrint(solver[0] + "")
	for eps in [1e-5, 1e-7]:
		iterCount = 0
		def OnIter():
			global iterCount
			iterCount += 1
		root = solver[1](eps, OnIter)
		UnicodePrint(u"\tпогрешность: %.0e, количество итераций: %d" % (eps, iterCount))
		#UnicodePrint(u"\t\tнайденный корень: %.10f" % root)