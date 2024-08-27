# TIPS

- OpenGL就是一个大型状态机，绑定了谁就是对谁操作，use了哪个程序，后面就用哪个程序绘图
- OpenGL的顶点坐标系为中心(0, 0)，整个范围是(-1, -1)到(1, 1) 
- OpenGL的纹理坐标为无限，(0, 0)到(1, 1)为原始图像，在这以外的将按规则进行重复、扩展
- OpenGL的纹理坐标系原点在左下角，而一般图片原点在左上角，因此直接使用将会上下颠倒
- 缩放矩阵
$$
\begin{bmatrix}
S_1 & 0 & 0 & 0 \\
0 & S_2 & 0 & 0\\
0 & 0 & S_3 & 0\\
0 & 0 & 0 & 1\\
\end{bmatrix}
\cdot
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}
=
\begin{pmatrix}
S_1 \cdot x \\
S_2 \cdot y \\
S_3 \cdot z \\
1 \\
\end{pmatrix}
$$
- 位移矩阵
$$
\begin{bmatrix}
1 & 0 & 0 & T_x \\
0 & 1 & 0 & T_y\\
0 & 0 & 1 & T_z\\
0 & 0 & 0 & 1\\
\end{bmatrix}
\cdot
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}
=
\begin{pmatrix}
x + T_x \\
y + T_y \\
z + T_z \\
1 \\
\end{pmatrix}
$$
- 旋转矩阵，太长了懒得写
- glDraw一次，就会多一个东西，在这期间可以通过uniform传递参数，让实际的绘图发生变化