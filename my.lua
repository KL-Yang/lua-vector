print("Hello vecsmath!")

print("1. Test of creating vectors")
a = v.vector(5)
b = v.vector(5,1,10)
c = v.vector(5,1)
d = v.vector(5,9.9)
e = v.vector({5,4,3,2,1})
f = v.vector({5,4,3,2,1})

print("  A: a = v.vector(5)             #a="..#a)
print("  B: b = v.vector(5,1,10)        #b="..#b)
print("  C: c = v.vector(5,1)           #c="..#c)
print("  D: d = v.vector(5,9.9)         #d="..#d)
print("  E: e = v.vector({5,4,3,2,1})   #e="..#e)
print("  F: f = v.vector({5,4,3,2,1})   #f="..#f)


for i=1, 5 do
    pa = string.format("  a[%d]=%8.2f", i, a[i])
    pb = string.format("  b[%d]=%8.2f", i, b[i])
    pc = string.format("  c[%d]=%8.2f", i, c[i])
    pd = string.format("  d[%d]=%8.2f", i, d[i])
    pe = string.format("  e[%d]=%8.2f", i, e[i])
    pf = string.format("  f[%d]=%8.2f", i, f[i])
    print(pa..pb..pc..pd..pe..pf)
end

print("2. Test of vector algrithm")
print("  A: a=e+f   vector-vector add")
print("  B: b=f+8   vector-scaler add")
print("  C: c=2*f   scaler-vector mul")
print("  D: d=f^2   vector-scaler pow")
a = e+f
b = f+8
c = 2*f
d = f^2
for i=1, 5 do
    pa = string.format("  a[%d]=%8.2f", i, a[i])
    pb = string.format("  b[%d]=%8.2f", i, b[i])
    pc = string.format("  c[%d]=%8.2f", i, c[i])
    pd = string.format("  d[%d]=%8.2f", i, d[i])
    print(pa..pb..pc..pd)
end

print("3. Test of array index algrithm")
for i=1, 5 do
    c[i] = 100 + c[i]
end
c=100+c
print("normal things!")
for i=1, 5 do
    print("c["..i.."]="..c[i])
end

x={5,4,3,2,1}
x=v.vector(x)
y=x^2
for i=1, 5 do
    print("x["..i.."]="..x[i].."  y["..i.."]="..y[i])
end
print("length of y="..#y)

t = v.isvector(x)
print("  is vector:")
print(t)

print("The follow is not a vector: expecting false")
z={5,4,3,2,1}
t = v.isvector(z)
print(t)
