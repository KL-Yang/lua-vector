--vector manipulation speed by element
--the v4 implementation is slower than v1 on this test!
--userdata is less efficient than internal table!

a = v.vector(4096)
b = v.vector(4096)
for i=1, #a do
    a[i] = 1.0
end

for i=1, 10000 do
    for j=1, #a do
        b[j] = b[j]+a[j]^3+a[j]^2+a[j]+1
    end
end

print('b[1]='..b[1]..'  b[4096]='..b[4096])
print('finished!')
