print("hello")

a = v.vector(9)
b = v.vector({1,2,3,4,5,6,7,8,9})
for i=1, #a do
    a[i] = 1.0
end
c = a+b
for i=1, #a do
    print(string.format("%4d %9.1f+%9.1f=%9.1f", i, a[i], b[i], c[i]))
end
print(a)

y = v.matrix(9,9)
x = v.dim(y)
print(string.format("shape=%d", x))

--if type(jit)~="table" then
--    print("Not running in LuaJit, cannot proceed!")
--    os.exit(1)
--end
--print("Runing in LuaJit")

--local ffi = require("ffi")
-- key is to redefine the library function v.raw()
-- v.raw_info() returns both point and dim
--function vgetraw(a)
--    local ta = v.raw_alias(a)        --temp a, lightuserdata
--    local ra = ffi.cast("double*", ta) --raw a, for ffi use
--    return ra
--end
--v.raw = vgetraw
--ra = vgetraw(a)

ra = v.raw(a)

for i=1, 9 do
    ra[i] = 9.0
end

for i=1, 9 do
    print(string.format("a[%4d]=%9.1f | %9.1f", i, a[i], ra[i]))
end

print("End of test")
