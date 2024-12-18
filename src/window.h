#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdio.h>
#include <iostream>

class Window
{
public:

    int width, height;
    double aspectRatio;
    GLuint texture, FBO, DRB;
    void (*resizeCallbak)(int, int);

    Window () {}

    Window(int width, int height, void (*resizeCallbak)(int, int))
        : width(width)
        , height(height)
        , aspectRatio(width / double(height))
        , resizeCallbak(resizeCallbak)
    { initFBO(); }

    ~Window()
    {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &DRB);
    }

    void updateDimensions(int newWidth, int newHeight)
    {
        width = newWidth;
        height = newHeight;
        glViewport(0, 0, width, height);
        aspectRatio = width / (float)height;

        // Update texture size
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Update depth buffer size
        glBindRenderbuffer(GL_RENDERBUFFER, DRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Re-attach texture and depth buffer to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DRB);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Invoke the window resize callback function from parent class
        resizeCallbak(newWidth, newHeight);
    }


    glm::ivec2 resolution() const
    {
        return glm::ivec2(width, height);
    }

private:

    void initFBO()
    {
        // Create FBO, texture, and depth buffer
        glGenFramebuffers(1, &FBO);
        glGenTextures(1, &texture);
        glGenRenderbuffers(1, &DRB);

        // Set texture parameters
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set depth buffer parameters
        glBindRenderbuffer(GL_RENDERBUFFER, DRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        
        // Attach texture and depth buffer to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DRB);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "Frame buffer not complete\n");
        }
        
        // Unbind texture and frame buffers
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

};

#endif