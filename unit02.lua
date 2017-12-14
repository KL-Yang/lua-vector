print("Start of Unit02 Test")

v.debug(2) -- enable this to track memory usage!

print("  2.a - create array of vectors!")
a = v.matrix(3,9)
print(string.format("    matrix of %dx%d", #a, #a[1]))
for i=1, #a[1] do
    a[1][i] = i
    a[2][i] = i*0.1
    a[3][i] = 10-i 
end

for i=1, #a[2] do
    print(string.format("%8.2f  %8.2f  %8.2f", a[1][i], a[1][i], a[1][i]))
end

print("  2.b - vector maths +-*/^")
e = a[1]+a[3]
f = a[1]*a[3]+a[1]
g = a[3]^2+1+a[1]-0.1
for i=1, #a[3] do
    print(string.format("%8.2f  %8.2f  %8.2f", e[i], f[i], g[i]))
end

print("End of Unit02")
