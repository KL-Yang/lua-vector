a = v.matrix(3,9)

for i=1, 9 do
    a[1][i] = i
    a[2][i] = 10-i
    a[3][i] = i*0.1
end

b = a[1]*2+100

for i=1, 3 do
    for j=1, 9 do
        print(a[i][j])
    end
end

print("  2.b - vector test")
for i=1, 9 do
    print(b[i])
end
