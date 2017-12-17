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
a = v.matrix(5, 5)
b = v.matrix(5, 5)
for i=1, #a do
    for j=1, #a[i] do
        a[i][j] = 1.0
        b[i][j] = 0.0
    end
end

--3. Start to operate on RAW!
ra = v.raw(a)
rb = v.raw(b)
print("ra:", ra)
print("rb:", rb)
for i=1, #a do
    print("ra"..i..":", ra[i])
    print("rb"..i..":", rb[i])
end
--os.exit(0)

function printmat(x, m, n)
    for i=1, m do
        io.write("  ")
        for j=1, n do
            io.write(string.format("%9.2f ", x[i][j]))
        end
        io.write("\n")
    end
end

for i=1, 3 do
    print("Loop "..i)
    printmat(a, #a, #a[1])
    for j=1, 5 do
     --   print("ra:"..ra[j])
     --   print("rb:"..rb[j])
        for k=1, 5 do
            rb[j][k] = rb[j][k]+a[j][k]^3+a[j][k]^2+a[j][k]+1
            --rb[j][k] = 100.0
        end
    end
    printmat(ra, #a, #a[1])
end

--4. RAW rb is just alias of b
print('b[1][1]='..rb[1][1]..'  b[5][5]='..rb[5][5])
print('finished!')
