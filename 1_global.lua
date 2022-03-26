Rlist = {}      --To record the uninitiated access
Wlist = {}      --To record the uninitiated access

local mt = {
    __index = function (t, k)
        table.insert(_G.Rlist, k)
        return 0
    end,
    __newindex = function (t, k, v)
        rawset(t, k, v)
        table.insert(_G.Wlist, k)
    end
}
setmetatable(_G, mt)

print(recx)
print(recx)
print(shotx)
print(shoty)

midx = 1.23456789

print("Those <"..#Rlist.."> attributes have been accessed:")
for i=1, #Rlist do
    print(Rlist[i])
end
print("I have returned 0, but remember to import next time!")

print("Those <"..#Wlist.."> attributes have been write:")
for i=1, #Wlist do
    print(Wlist[i])
    print(_G[Wlist[i]])
end

-- This test the variable argument functions!
function tprint(...)
    for i,v in ipairs {...} do
        print(i, v)
    end
end
tprint("hello", "world", "lua")
