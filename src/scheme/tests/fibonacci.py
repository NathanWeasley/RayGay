def fib(n):
   if n < 2:
      return n
   else:
      return fib(n-1) + fib(n-2)

for i in range(36):
    print "%d => %d" % (i, fib(i))