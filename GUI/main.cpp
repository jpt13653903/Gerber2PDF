#include <iostream>
#include <string>
#include <iostream>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include "gui_main.hpp"

static const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "Glfw Error "<<error<<" "<<description<<"\n";
}

static void draw_frame(GLFWwindow *window, int display_w, int display_h) {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    gui_loop();

    ImGui::Render();
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

static void window_resize_cb(GLFWwindow *window, int display_w, int display_h) {
    // std::cout << "Resize\n";
    draw_frame(window, display_w, display_h);
}

int main(int argc, char** argv) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Gerber2PdfGui", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Could not create GLFW window\n";
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    gui_setup(argc, argv);

    glfwSetWindowSizeCallback(window, window_resize_cb);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        draw_frame(window, display_w, display_h);
    }
    return 0;
}

