print("Start of Unit03 Test")

v.debug(2) -- enable this to track memory usage!

print("  3.a - create array of vectors!")
a = v.matrix(9,9)
b = v.matrix(9,9)
print(string.format("    matrix of %dx%d", #a, #a[1]))
for i=1, #a do
    for j=1, #a[1] do
        a[i][j] = i+j*0.1 
    end
end

for i=2, #a-1 do
    for j=2, #a[1]-1 do
        b[i][j] = a[i-1][j-1] + a[i-1][j] + a[i-1][j+1]
                + a[i+0][j-1] + a[i+0][j] + a[i+0][j+1]
                + a[i+1][j-1] + a[i+1][j] + a[i+1][j+1]
    end
end

print("  Matrix-a:")
for i=1, #a do
    for j=1, #a[1] do
        io.write(string.format(" %8.2f", a[i][j]))
    end
    print(" ")
end

print("  Matrix-b:")
for i=1, #b do
    for j=1, #b[1] do
        io.write(string.format(" %8.2f", b[i][j]))
    end
    print(" ")
end

print("End of Unit03")
