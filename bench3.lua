--vector manipulation speed by element
--the v4 implementation is slower than v1 on this test!
--userdata is less efficient than internal table!

--1. Test must have LuaJit
if type(jit)~="table" then
    print("Not running in LuaJit:")
else
    print("LuaJit: "..jit.version)
end

--2. Initiate test vector
a = v.vector(4096)
b = v.vector(4096)
for i=1, #a do
    a[i] = 1.0
end

--3. Start to operate on RAW!
ra = v.raw(a)
rb = v.raw(b)

for i=1, 10000 do
    for j=1, #a do
        rb[j] = rb[j]+ra[j]^3+ra[j]^2+ra[j]+1
    end
end

--4. RAW rb is just alias of b
print('b[1]='..b[1]..'  b[4096]='..b[4096])
print('finished!')
