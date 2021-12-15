# 新的OpenGL (programmable architecture using shaders)
## 摘要
最新的OpenGL Core已经移除了固定渲染管线的内容，glBegin(...)...glEnd();的用法已经过时，最新的OpenGL程序一般使用shader、VAO和VBO等对象让GPU使用顶点数据和着色器绘图。<br>
参考 <br>
[Tutorial2: VAOs, VBOs, Vertex and Fragment Shaders (C / SDL)](https://www.khronos.org/opengl/wiki/Tutorial2:_VAOs,_VBOs,_Vertex_and_Fragment_Shaders_(C_/_SDL)) <br>
[Vertex Specification](https://www.khronos.org/opengl/wiki/Vertex_Specification)

## 着色器创建
常用着色器有：顶点着色器、网格细分着色器、片元着色器
### 顶点着色器
传入数据主要来自客户端，即VBO缓存的顶点数据。服务端将顶点数据按一定的顺序和格式分配给顶点着色器（详见后文VAO和VBO）。一般情况下，输入数据中首要包括顶点位置，其次可能有纹理UV坐标和顶点法向信息等。 <br>
顶点着色器可输出gl_Position、gl_PointSize等信息，详见reference card。常用gl_Position，输出待插值的绘制坐标，该坐标为齐次坐标，即其具有xyzw四个分量，一般通过传入的顶点位置透视矩阵左乘得到，该矩阵左乘使得结果（gl_Position）的xyz分量是输入的顶点位置的xyz从透视矩阵来源的视锥体转换到-1，-1，-1 ~ 1，1，1的立方体内的新坐标，结果的w分量为z或-z。 <br>
平行投影变换为线性变换，即在屏幕上三角形内任意一点的深度是其重心坐标的线性插值。而透视变换为非线性变换，

## 顶点数据传递和绘制

# VAO

# VBO

# 基元