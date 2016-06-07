
a = v.vector(4096, 1.0)
b = v.vector(4096)

for i=1, 10000 do
    b = b+a^3+a^2+a+1
end

print('b[1]='..b[1]..'  b[4096]='..b[4096])
v.report()
print('finished!')
