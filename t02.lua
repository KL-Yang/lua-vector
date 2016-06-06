print("02. Test of memory leakage issue!")

--might leak 32k in each call for v2 implementation
function test (len)
    local a, b, c, d 
    a = v.vector(len, 1)
    b = v.vector(len, 2)
    c = v.vector(len, 3)
    d = v.vector(len, 4)
    return (a^2+b^2+c^2+d^2)
end

a = v.vector(4096)
for i=1, 1000 do
    a = a + test(4096)
    if i%10==0 then
        os.execute("sleep 1")
        k=collectgarbage("count")
        print(i.."memory use="..k.."KB")
    end
end

print(" #a="..#a)
print(" a[1]="..a[1])

k=collectgarbage("count")
print("memory use="..k.."KB")
collectgarbage()
