# BMP Image Processor

Process `.bmp` images. Run the binary with a file of instructions, one instruction per line.

## Function

| Function | Syntax |
|---|---|
| `CREATE` | `CREATE tag cols rows` |
| `LOAD` | `LOAD tag path` |
| `SAVE` | `SAVE tag path` |
| `CLEAR` | `CLEAR tag R G B` |
| `PASTE` | `PASTE dest src x y` |
| `RECTANGLE` | `RECTANGLE tag left right top bottom R G B` |
| `CROP` | `CROP tag left right top bottom` |
| `GRAYSCALE` | `GRAYSCALE tag` |
| `SQUARE` | `SQUARE tag` |
| `FILTER` | `FILTER filter tag` |

`filter` is either `BOX`, `GAUSS`, `SHARPEN`, `LAPLACE`, or `EMBOSS`.
