# lua-vector extention experiments

This experiment runs on Debian Stretch, with default lua/luajit in Debian reponsitory.

vector_v1.c :
implementation use lua table.

vector_v4.c :
implementation use lua userdata.

other files are kept to be clean up. 

| Tests    |  bench1-runtime | bench2-runtime | comment |
| --- | --- | --- | --- |
| v1-lua | 0m13.668s | 0m12.930s | |
| v1-jit | 0m11.204s | 0m0.142s  | |
| v4-lua | 0m2.785s  | 0m26.283s | |
| v4-jit | 0m2.584s  | 0m14.036s | |

LuaJit is very good at array access.
