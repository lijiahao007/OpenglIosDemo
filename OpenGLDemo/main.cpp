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
GLfloat stepSize = 0.01f;

GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f, // 左下
    
    blockSize, -blockSize, 0.0f, // 右下
    
    blockSize, blockSize, 0.0f, // 右上
    
    -blockSize, blockSize, 0.0f, // 左上
};


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
    
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    // UseStockShader 使用固定着色器
    // GLT_SHADER_IDENTITY 单元着色器
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}

void SpecialKeys(int key, int x, int y) {
    printf("SpecialKeys key[%d]  x[%d]  y[%d]\n", key, x, y);
    
    // 修改顶点数据
    
    // 先修改左下角
    GLfloat leftBottomX = vVerts[0];
    GLfloat leftBottomY = vVerts[1];
    
    if (key == GLUT_KEY_UP) {
        if (leftBottomY + 2*blockSize + stepSize <= 1) {
            leftBottomY += stepSize;
        } else {
            leftBottomY = 1 - 2 * blockSize;
        }
        
    } else if (key == GLUT_KEY_LEFT) {
        if (leftBottomX - stepSize >= -1) {
            leftBottomX -= stepSize;
        } else {
            leftBottomX = -1;
        }
    } else if (key == GLUT_KEY_RIGHT) {
        if (leftBottomX + 2*blockSize + stepSize <= 1) {
            leftBottomX += stepSize;
        } else {
            leftBottomX = 1 - 2 * blockSize;
        }
    } else if (key == GLUT_KEY_DOWN) {
        if (leftBottomY - stepSize >= -1) {
            leftBottomY -= stepSize;
        } else {
            leftBottomY = -1;
        }
    }
    
    // 根据左下角的顶点，修改其他顶点。
    vVerts[0] = leftBottomX;
    vVerts[1] = leftBottomY;
    
    // 右下
    vVerts[3] = leftBottomX + 2*blockSize;
    vVerts[4] = leftBottomY;
    
    // 右上
    vVerts[6] = leftBottomX + 2*blockSize;
    vVerts[7] = leftBottomY + 2*blockSize;
    
    // 左上
    vVerts[9] = leftBottomX;
    vVerts[10] = leftBottomY + 2*blockSize;
    
    // 将顶点数据重新引用到OpenGL中
    triangleBatch.CopyVertexData3f(vVerts);
    
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
