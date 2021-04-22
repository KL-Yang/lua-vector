print("Hello vecsmath!")

print("1. Test of creating vectors")

a = {}
a[1] = v.vector(1000)
for i=2, 10000 do
    a[i] = v.vector(1000)
    a[i-1] = nil
end

b = {}
b[1] = v.vector(1000)
for i=2, 10000 do
    b[i] = v.vector(1000)
    b[i] = b[i-1]+a[i]
end

print(" #a="..#a)
print(" #a="..#b)

k=collectgarbage("count")
print("memory use="..k.."KB")
collectgarbage()
print("finished")
os.exit(0);
