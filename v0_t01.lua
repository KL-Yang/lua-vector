-- ./v0 v0_t01.lua
-- lua v0_t01.lua gives same result!
if _vector_v0_def ~= nil then
    print("@@@@@@> my v0C interpreter")
else
    dofile("vector_v0.lua")
    print("@@@@@@> my lua interpreter")
end

print("#Hello vecsmath!")

len=4
print(len)
a = v.vector(len)
b = v.vector(len)
c = a+b

print(c)
print("#finished")
