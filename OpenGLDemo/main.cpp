//
//  main.cpp
//  OpenGLDemo
//
//  Created by 李嘉浩 on 2024/3/7.
//

#include "GLShaderManager.h"

#include "GLTools.h"

#include <glut/glut.h>

GLBatch triangleBatch;

GLShaderManager shaderManager;

GLfloat blockSize = 0.1f;
GLfloat stepSize = 0.05f;

GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f, // 左下
    
    blockSize, -blockSize, 0.0f, // 右下
    
    blockSize, blockSize, 0.0f, // 右上
    
    -blockSize, blockSize, 0.0f, // 左上
};

GLfloat xOriginPos = 0; // 原点x坐标
GLfloat yOriginPos = 0; // 原点y坐标
GLfloat rotateDegreeStep = 1.0f; // 旋转的度。
GLfloat curRotateDegree= 0;


//窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素

void ChangeSize(int w,int h)

{
    printf("ChangeSize (reshape) w[%d] h[%d]\n", w, h);
    glViewport(0,0, w, h);
    
}

//为程序作一次性的设置

void SetupRC()

{
    
    //设置背影颜色 （用新的颜色覆盖旧的背景颜色；也可以说将颜色缓冲区用指定颜色填满（但还未执行渲染））
    
    glClearColor(0.0f,0.0f,1.0f,1.0f);
    
    //初始化着色管理器
    
    shaderManager.InitializeStockShaders();
    
    //设置三角形，其中数组vVert包含所有3个顶点的x,y,笛卡尔坐标对。
//
//    GLfloat vVerts[] = {
//        
//        -0.5f, 0.0f, 0.0f,
//        
//        0.5f, 0.0f, 0.0f,
//        
//        0.5f, 0.5f, 0.0f,
//        
//        -0.5f, 0.5f, 0.0f,
//        
//    };
    
    // 用不同的模式来绘图
    triangleBatch.Begin(GL_QUADS,4);
    
    triangleBatch.CopyVertexData3f(vVerts);
    
    triangleBatch.End();
    
}

//开始渲染

void RenderScene(void)

{
        
    //清除一个或一组特定的缓冲区
    // glClear(CL_COLOR_BUFFER_BIT) 用 glClearColor指定的颜色，执行清空颜色缓冲区。
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示红色
    
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    
    // 使用平面着色器
    M3DMatrix44f mTranslateMatrix, mRotateMatrix, mFinalMatrix;
    // 使用矩阵平移画面，x参数对应横向平移，y参数对应垂直平移。
    // 平移矩阵
    m3dTranslationMatrix44(mTranslateMatrix, xOriginPos, yOriginPos, 0.0f);
    // 旋转矩阵
    m3dRotationMatrix44(mRotateMatrix, curRotateDegree, 0.0f, 0.0f, 1.0f);

    /**
     矩阵相乘 （矩阵点乘）
     /注意：矩阵点乘不具备交换律，即相乘矩阵交换位置，得出来的结果会不一样
     总体的规律是，在前面的矩阵先应用，在后面的矩阵后应用。
     
     平移矩阵在前：
     如果你首先应用平移矩阵，那么物体将会先被平移到新的位置。
     接着，当你应用旋转矩阵时，旋转将会围绕平移后的新位置进行。
     这种顺序通常用于先移动物体到一个特定的位置，然后再围绕该位置进行旋转。

     平移矩阵在后：
     如果你首先应用旋转矩阵，那么物体将会先围绕其原始位置进行旋转。
     接着，当你应用平移矩阵时，平移将会基于旋转后的物体进行。
     这种顺序通常用于先围绕物体的原始位置进行旋转，然后再将旋转后的物体移动到新的位置。
     */
   
    m3dMatrixMultiply44(mFinalMatrix, mTranslateMatrix, mRotateMatrix);
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, mFinalMatrix, vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}

void SpecialKeys(int key, int x, int y) {
    printf("SpecialKeys key[%d]  x[%d]  y[%d]\n", key, x, y);
    
    // 修改顶点数据
    if (key == GLUT_KEY_UP) {
        if (yOriginPos + blockSize + stepSize <= 1) {
            yOriginPos += stepSize;
        } else {
            yOriginPos = 1 - blockSize;
        }
        
    } else if (key == GLUT_KEY_LEFT) {
        if (xOriginPos - blockSize - stepSize >= -1) {
            xOriginPos -= stepSize;
        } else {
            xOriginPos = -1 + blockSize;
        }
    } else if (key == GLUT_KEY_RIGHT) {
        if (xOriginPos + blockSize + stepSize <= 1) {
            xOriginPos += stepSize;
        } else {
            xOriginPos = 1 - blockSize;
        }
    } else if (key == GLUT_KEY_DOWN) {
        if (yOriginPos - blockSize - stepSize >= -1) {
            yOriginPos -= stepSize;
        } else {
            yOriginPos = -1 + blockSize;
        }
    } else if (key == 'r' || key == 'R') {
        curRotateDegree += rotateDegreeStep;
    }
    
    // 重新渲染。（重新调用RenderScnene 注册的渲染函数）
    glutPostRedisplay();
}

int main(int argc,char* argv[])

{
    
    //设置当前工作目录，针对MAC OS X
    
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库
    
    glutInit(&argc, argv);
    
    /*初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小，标题窗口
    
    glutInitWindowSize(800,600);
    
    glutCreateWindow("Rectangle");
    
    //注册回调函数
    
    // 注册reshape监听函数
    glutReshapeFunc(ChangeSize);
    
    // 注册显示函数
    glutDisplayFunc(RenderScene);
    
    // 注册特殊键位函数
    glutSpecialFunc(SpecialKeys);
    
    // 初始化GLEW库，确保OpenGL API对程序完全可用。
    // 在视图做任何渲染之前，要检查确定驱动程序的初始化过程中没有任何问题。
    GLenum err = glewInit();
    
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    //调用SetupRC
    SetupRC();
    
    glutMainLoop();
    
    return 0;
    
}
