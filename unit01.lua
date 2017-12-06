print("Start of Unit01")

print("  1.a - create vectors and print")
a = v.vector({1,2,3,4,5,6,7,8,9})
b = v.vector(9)         -- 9 zeros
c = v.vector(9, 0.1)    -- 9 0.1
d = v.vector(9, 10, 90) -- 9 in range [10,90]

for i=1, 9 do
    print("    "..a[i].."  "..b[i].."  "..c[i].."  "..d[i])
end

print("  1.b - vector maths +-*/^")
e = a+c
f = a*c+d
g = c^2+1+d-0.1
for i=1, 9 do
    print("    "..e[i].."  "..f[i].."  "..g[i])
end

print("End of Unit01")
--d = vector.new(20)
--f = c+d
--print(c)
