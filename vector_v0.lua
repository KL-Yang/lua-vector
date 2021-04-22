---------------------------------------------
-- This section implement all the function
-- in vector_v0.c
---------------------------------------------
function meta__add(a,b)
    local c={}
    for i=1,#a do
        c[i]=a[i]+b[i]
    end
    setmetatable(c, meta)   --seems optional
    return c
end

function meta__tostring(a)
    local xstr=""
    if #a<5 then
        for i=1,#a do
            xstr=xstr..string.format("#>%3d %f\n", i, a[i])
        end
    end
    xstr=xstr..string.format("#%s(%d)", "vector_tostring",#a)
    return xstr
end

function vector(n)
    local x={}
    for i=1,n do
        x[i]=i-1
    end
    setmetatable(x, meta)
    return x
end

meta = {}
meta.__add = meta__add
meta.__tostring = meta__tostring

v = {}
v.vector = vector
-----------------------------------------------
