# 渲染基本术语
| 术语 | 说明 |
| --- | --- |
| Transformation \& Projection | 变换和投影，空间中点和向量的变换，3D空间到2D屏幕的投影 |
| Rasterization | 光栅化，填充三角形 |
| Shading | 着色，着色器（Shader）属于在图形硬件上执行的程序 |
| Texture Mapping | 纹理贴图，纹理映射 |
| Blending | 混合，混合颜色 |

# 其他术语
| 术语 | 说明 |
| --- | --- |
| OpenGL ARB | OpenGL Architecture Review Board, OpenGL 体系审核委员会 |


# OpenGL是什么
图形硬件的一种软件接口，类似Windows API访问文件系统，程序能够使用OpenGL绘制图形。OpenGL不包括类似GLUT中的GUI接口，目前也不包括GLTools中操作矩阵和向量的3D数学库（这里不讨论GLSL）。OpenGL的功能参考opengl quick reference card。

## 辅助OpenGL的其他API
| 库 | 描述 |
| --- | --- |
| GLUT | 配合OpenGL使用的精简GUI工具包，有限的GUI功能 | 
| GLEW | 扩展加载库 |
| GLTools | 基于GLEW的工具箱，包含矩阵向量运算库等工具 |

## OpenGL特性

### 数据类型
OpenGL定义了供OpenGL使用的跨平台的数据类型（标准中不包括任何类型的指针和数组，指针和数组可能仍是语言原生提供，遵循C/C++语言标准），均由GL开头，具体参考蓝宝书或 [OpenGL Type](https://www.khronos.org/opengl/wiki/OpenGL_Type)。（GLvoid不存在于文档中，参考 [GLvoid](https://community.khronos.org/t/origin-of-glvoid/45093) ）

### 错误
使用如下函数返回发生的错误：<br>
` GLenum glGetError(void); `<br>
通常循环使用该函数直到返回为 `GL_NO_ERROR` ，会依次打印上一次 `GL_NO_ERROR` 以来的所有错误。<br>
参考 [OpenGL glGetError](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetError.xhtml)

### 确认版本
使用 `glGetString` 获取版本号。 <br>
```
const GLubyte *glGetString(GLenum name);
const GLubyte *glGetStringi(GLenum name, GLuint index);
```
[glGetString 参考](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetString.xhtml)

### glHint
`glHint` 函数可以指定在视觉质量和渲染速度间的平衡点，是OpenGL中唯一一个行为依赖生产商的函数。<br>
用法：<br>
`void glHint(GLenum target, GLenum mode);`<br>
[glHint 参考](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glHint.xhtml)

### OpenGL状态机
类似HTML5中canvas的fillStyle、lineWidth、strokeStyle等等，OpenGL在绘图也存在一系列状态变量（类型包括但不限于布尔），它们控制绘图的一些具体细节。<br>
OpenGL的状态模型让这些变量在下一次显式地修改（即使用函数修改）之前保持不变。<br>
例如：
```
glEnable(GL_DEPTH_TEST); // 打开深度测试
glDisable(GL_DEPTH_TEST); // 关闭深度测试
GLboolean b = glIsEnabled(GL_DEPTH_TEST); // 查询深度测试
```

