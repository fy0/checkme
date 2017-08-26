#include <imgui.h>
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <zmq.hpp>
#include <msgpack11.hpp>
#include "imgui_impl_glfw_gl3.h"
#include "../deps/utfconvert/utfconvert.h"
#include <locale>
#include <shlwapi.h>

using msgpack11::MsgPack;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

zmq::context_t context(1);
zmq::socket_t request(context, ZMQ_REQ);
zmq::socket_t broadcast(context, ZMQ_SUB);

void setup_zmq() {
	broadcast.connect("tcp://127.0.0.1:12321");
	broadcast.setsockopt(ZMQ_SUBSCRIBE, "10001 ", 0);
	request.connect("tcp://127.0.0.1:12322");
}

char* get_font_path() {
	wchar_t szExePath[MAX_PATH] = { 0 };
	char *buf = new char[1024];

	uint32_t u32buf[MAX_PATH];
	int u32buf_used;

	GetModuleFileNameW(NULL, szExePath, sizeof(szExePath));
	PathRemoveFileSpecW(szExePath);
	wcscat(szExePath, L"\\SourceHanSansCN-Regular.otf");

	uc_ucs4str_from_wchar((wchar_t*)szExePath, MAX_PATH, (uint32_t*)&u32buf, &u32buf_used);
	int ret = uc_ucs4str_to_utf8z((uint32_t*)&u32buf, u32buf_used, (uint8_t*)buf, 1024);

	return buf;
}

int main(int, char**) {
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Check me", NULL, NULL);
    glfwMakeContextCurrent(window);
    gl3wInit();

	// setup zmq
	setup_zmq();

    // Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);

    // Load Fonts
    ImGuiIO& io = ImGui::GetIO();

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

	zmq::pollitem_t items[] = {
		{ broadcast, 0, ZMQ_POLLIN, 0 },
		{ request, 0, ZMQ_POLLIN, 0 }
	};

	zmq::message_t message;
	float poll_time = 0;
	float poll_duration = 1;

	glfwSwapInterval(1);

	MsgPack my_msgpack = MsgPack::object{
		{ "key1", "value1" },
		{ "key2", false },
		{ "key3", MsgPack::array { 1, 2, 3 } },
	};

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
		ImGui::GetIO().FontGlobalScale = 1.2;

		poll_time += ImGui::GetIO().DeltaTime;
		if (poll_time > poll_duration) {
			poll_time = 0;
			zmq::poll(&items[0], 2, 0);

			if (items[0].revents & ZMQ_POLLIN) {
				broadcast.recv(&message);
				printf("%d\n", message.size());
				//  Process task
			}
			if (items[1].revents & ZMQ_POLLIN) {
				request.recv(&message);
				printf("%d\n", message.size());
				//  Process weather update
			}
		}
        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);

		char sss[400];
		uc_wchar_to_utf8(L"测试一下 123", 100, (uint8_t*)&sss);
		ImGui::Text(u8"Dear ImGui says hel  测试  123");
		ImGui::Text("\x68\x65\x6C\x6C\x6F\x20" "\xE8\x8F\x9C\xE5\x8D\x95\x20" "\xE5\x9C\xA8\xE5\x93\xAA\x20" "a" "\xE9\x87\x8C\xE6\x96\x87" "\xE4\xBB\x3F\xE6\x88\x91\xE6\x98\xAF\xE4\xB8\xAD\xE6\x96\x87\x61");
		ImGui::Text((const char *)&sss);

        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Help")) {
            ImGui::Text("test 123");
        }

        ImGui::End();

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}
