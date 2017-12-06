print("Start of Unit01")

print("  1.a - create vectors and print")
a = v.vector({1,2,3,4,5,6,7,8,9})
b = v.vector(9)         -- 9 zeros
c = v.matrix(9)         -- 9 zeros

for i=1, 9 do
    c[i] = 10-i
end

for i=1, 9 do
    print("    "..a[i].."  "..b[i].."  "..c[i])
end

print("  1.b - vector maths +-*/^")
e = a+c
f = a*c+a
g = c^2+1+a-0.1
for i=1, 9 do
    print("    "..e[i].."  "..f[i].."  "..g[i])
end

print("End of Unit01")
