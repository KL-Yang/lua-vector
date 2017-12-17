--vector manipulation speed by element
--the v4 implementation is slower than v1 on this test!
--userdata is less efficient than internal table!

--1. Test must have LuaJit
if type(jit)~="table" then
    print("Not running in LuaJit:")
else
    print("LuaJit: "..jit.version)
end

n = 128
--2. Initiate test vector
a = v.matrix(n, n)
b = v.matrix(n, n)
for i=1, #a do
    for j=1, #a[i] do
        a[i][j] = 1.0
        b[i][j] = 0.0
    end
end

--3. Start to operate on RAW!
ra = v.raw(a)
rb = v.raw(b)
--print("ra:", ra)
--print("rb:", rb)
--for i=1, #a do
--    print("ra"..i..":", ra[i])
--    print("rb"..i..":", rb[i])
--end
--os.exit(0)

--function printmat(x, m, n)
--    for i=1, m do
--        io.write("  ")
--        for j=1, n do
--            io.write(string.format("%9.2f ", x[i][j]))
--        end
--        io.write("\n")
--    end
--end

for i=1, 10000 do
    --print("Loop "..i)
    --printmat(a, #a, #a[1])
    for j=1, #a do
        for k=1, #a[j] do
            rb[j][k] = rb[j][k]+ra[j][k]^3+ra[j][k]^2+ra[j][k]+1
        end
    end
    --printmat(ra, #a, #a[1])
end

--4. RAW rb is just alias of b
print('a[1][1]='..ra[1][1]..'  a[n][n]='..ra[n][n])
print('b[1][1]='..rb[1][1]..'  b[n][n]='..rb[n][n])
print('finished!')
