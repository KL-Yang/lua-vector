print("Hello vecsmath!")

print("1. Test of creating vectors")

a = {}
a[1] = v.vector(4)
for i=2, 10000 do
    a[i] = v.vector(4)
    a[i-1] = nil
end

print(" #a="..#a)

k=collectgarbage("count")
print("memory use="..k.."KB")
collectgarbage()
os.exit(0);
