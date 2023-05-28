#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <GL/GL.h>
#include <FreeImage.h>
#include <map> 

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "../../../x64/Release/functions.h"

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 700;
constexpr float PI = 3.14159265358979f;
constexpr float E = 2.718281828f;

float DegToRad(float deg)
{
    return deg * 0.0174533f;
}
float RadToDeg(float rad)
{
    return rad * 57.2957795f;
}

//Deklaruojamos ivesties funkcijos
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void InfoPopUp()
{
    const char* infotext = R"(
Judejimas:
    w - priekin
    a - kairen
    s - atgal
    d - desinen
    q - aukstyn
    e - zemyn
    peles ratas - keisti greiti
            
Sukimasis:
    v - ijungti/isjungti sukimasi su pele
    Naudoti pele zvalgymuisi

Kitka:
    i - irasyti matoma vaizda png formatu
)";

    size_t strli = strlen(infotext);
    wchar_t* infO = new wchar_t[strli + 1];
    mbstowcs_s(NULL, infO, strli + 1, infotext, strli);
    MessageBoxW(NULL, (LPCWSTR)(infO), (LPCWSTR)L"Kontroliavimas", MB_OK | MB_ICONINFORMATION);
}

//Nuskaitomi ir kompiliuojai seseliuokles (angl. shader) failai
static std::vector<std::string> ParseShader(const std::string& shader, const std::string& header)
{
    std::ifstream stream(shader);

    std::vector<std::string> rez;
    std::string line, linehdr, v, f, exf;
    int mode = 0, rd = 0;

    if (stream.is_open())
    {
        while (getline(stream, line))
        {
            if (line == "#shader vertex")
            {
                mode = 1;
            }
            else if (line == "#shader fragment")
            {
                mode = 2;
            }
            else if (line == "//injection")
            {
                std::ifstream hdr(header);

                if (hdr.is_open())
                {
                    while (getline(hdr, linehdr))
                    {
                        if (linehdr == "//function inj")
                        {
                            rd = 1;
                        }
                        if (rd == 1)
                        {
                            if (linehdr.find("inline") != std::string::npos)
                            {
                                exf.append(linehdr.substr(7) + "\n");
                            }
                            else if (linehdr.find("static") != std::string::npos)
                            {
                                exf.append(linehdr.substr(6) + "\n");
                            }
                            else
                            {
                                exf.append(linehdr + "\n");
                            }
                        }
                    }
                }
                hdr.close();
                f.append(exf);
            }
            else
            {
                line.append("\n");
                if (mode == 1)
                {
                    v.append(line);
                }
                else if (mode == 2)
                {
                    f.append(line);
                }
            }
        }
    }
  
    rez.push_back(v);
    rez.push_back(f);

    stream.close();

    return rez;
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Compilation error\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

//Vaizdo irasimo funkcija su FreeImage biblioteka
int pfnc = 0;
void captureImage()
{
    pfnc++;
    BYTE* pixels = (BYTE*)malloc(4 * SCREEN_WIDTH * SCREEN_HEIGHT);
    glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    // Convert to FreeImage format & save to file
    FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, SCREEN_WIDTH, SCREEN_HEIGHT, ((SCREEN_WIDTH * 24 + 31) / 32) * 4, 24, 0, 0, 0, false);
    std::string path = "Vaizdai/pic_" + std::to_string(pfnc) + ".png";
    FreeImage_Save(FIF_PNG, image, path.c_str(), 0);

    //w Free resources
    FreeImage_Unload(image);
    free(pixels);
}

//Deklaruojami ivairus kintamieji
glm::vec3 camPos = glm::vec3(0, -4.0, 0);
glm::vec3 camRot = glm::vec3(0, 0, 0);

float mouseSens = 1.0f;
int fov = 90;
glm::vec4 vr = glm::vec4(0, 0, 0, 0);
glm::vec3 vr2 = glm::vec3(-3, -2, 0);
float speed = 1.0f;
bool relSpeed = true;
int steps = 200;
float AO = 0.3f;
int msaa = 1;
float prec = 3.0f;
float softShadow = 10.0f;
bool glow = false;
bool color = true;
float glowThrMult = 30;
float eps = 6;

bool focusInit = false;
bool viewFocus = false;
bool lastshow = true;
bool vid = false;

float movSpeed;
float varLimit = 5.0f;

void toggleCursor(bool show)
{
    if(show != lastshow)
    {
        lastshow = show;
        ShowCursor(show);
    }
}

float smoothMin(float a, float b, float k)
{
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return b + (a - b) * h - k * h * (1.0 - h);
}

int main(int argc, char *argv[])
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    //Inicijuojamas OpenGl langas
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3D Julia", NULL, NULL);
    if(!window)
    {
        std::cout << "!window";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    //Iskvieciamos ivesties funkcijos
    glfwSetKeyCallback(window, keyCallBack);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    if (glewInit() != GLEW_OK)
        std::cout << "GlewInit error";

    std::cout << glGetString(GL_VERSION) << "\n";

    float pos[12] = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,  -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f};

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), pos, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

	std::vector<std::string> ParsedShader;

    //Kompiliuojami seseliuokliai
	if(argc > 2)
		ParsedShader = ParseShader(argv[1], argv[2]);
	else
        ParsedShader = ParseShader("../x64/Release/frac.shader", "../x64/Release/functions.h");

    unsigned int shd = CreateShader(ParsedShader[0], ParsedShader[1]);
    glUseProgram(shd);
       
    //Sukuriamas Imgui langas
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glm::vec3 camPosDif = vec3(0);
    glm::vec3 camRotDif = vec3(0);

    ImVec4 back_color = ImVec4(0, 0, 0, 1);
    ImVec4 obj_color = ImVec4(0, 1, 1, 1);

    POINT f, m{ 0,0 };

    int imguiSize = 500;
    glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    HWND wind_h;
    RECT rect;
    float tmpd, dsp;

    while(!glfwWindowShouldClose(window))
    {
        //Isvalomi praeito kadro duomenys
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();       

        //Nustatoma tinkama abieju langu pozicija
        wind_h = glfwGetWin32Window(window);
        GetWindowRect(wind_h, &rect);
            
        SCREEN_WIDTH = rect.right - rect.left - imguiSize;
        SCREEN_HEIGHT = rect.bottom - rect.top;

        glScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        
        if (SCREEN_WIDTH / (SCREEN_HEIGHT * 1.0f) < 0.5f)
        {
            SCREEN_WIDTH = SCREEN_HEIGHT * 0.5f;
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            glfwSetWindowSize(window, SCREEN_WIDTH + imguiSize, SCREEN_HEIGHT - 50);
        }
        else
        {
            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH, 0));
        ImGui::SetNextWindowSize(ImVec2(imguiSize, SCREEN_HEIGHT));

        //Reagavimas i peles ivesti
        GetCursorPos(&f);
        ScreenToClient(wind_h, &f);
        m.x = (LONG)(SCREEN_WIDTH * 0.5);
        m.y = (LONG)(SCREEN_HEIGHT * 0.5);

        if (GetActiveWindow() == wind_h)
        {
            //Sukimasis
            if (f.x > 0 && f.y > 0 && f.x < SCREEN_WIDTH && f.y < SCREEN_HEIGHT && viewFocus)
            {
                if (f.x - m.x != 0 || f.y - m.y != 0)
                {
                    if (!focusInit)
                    {
                        camRotDif.x = mouseSens * 0.05f * ImGui::GetIO().DeltaTime * ((float)(f.y) - (SCREEN_HEIGHT * 0.5f));
                        camRotDif.z = mouseSens * 0.05f * ImGui::GetIO().DeltaTime * ((float)(f.x) - (SCREEN_WIDTH * 0.5f));
                    }
                    else
                        focusInit = false;
                    ClientToScreen(wind_h, &m);
                    SetCursorPos(m.x, m.y);
                }
                toggleCursor(false);
            }
            else
                toggleCursor(true);

            //Judejimas
            movSpeed = speed * ImGui::GetIO().DeltaTime;
            if (GetAsyncKeyState(GLFW_KEY_D) < 0)
                camPosDif.x += movSpeed;
            if (GetAsyncKeyState(GLFW_KEY_A) < 0)
                camPosDif.x -= movSpeed;
            if (GetAsyncKeyState(GLFW_KEY_W) < 0)
                camPosDif.y += movSpeed;
            if (GetAsyncKeyState(GLFW_KEY_S) < 0)
                camPosDif.y -= movSpeed;
            if (GetAsyncKeyState(GLFW_KEY_E) < 0)
                camPosDif.z += movSpeed;
            if (GetAsyncKeyState(GLFW_KEY_Q) < 0)
                camPosDif.z -= movSpeed;

            //parametrai promo video
            if (vid && distance(camPos) > 0.00001f)
            {
                camPosDif.y += movSpeed / 30.0f;
                //camPos = 
                /*float tmd = -2.5f;
                camPos = vec3(tmd * sin(vr.y), tmd * cos(vr.y), 1);
                camRot = vec3(0.2f, 0, vr.y);
                vr.x += 0.0075f;
                vr.y += 0.0075f;
                vr2.x += 0.015f;
                vr2.y += 0.01f;*/
            }

            //Vartotojo ivesties vertimas i duoemenis seseliuoklei
            camRot += camRotDif;

            tmpd = max(abs(distance(camPos)), 0.000000000001f);
            dsp = 2.0f;
            //camPos += vec3(relSpeed ? min(pow(tmpd, dsp), pow(tmpd, 1 / dsp)) : 1) * rotate(camRot, camPosDif);
            camPos += vec3(relSpeed ? min(pow(dsp, tmpd) - 1,1.0f) : 1) * rotate(camRot, camPosDif);

            camRot.x = fmod(camRot.x, DegToRad(360));
            camRot.z = fmod(camRot.z, DegToRad(360));

            if (camRot.x > DegToRad(89.9))
                camRot.x = DegToRad(89.9);
            if (camRot.x < DegToRad(-89.9))
                camRot.x = DegToRad(-89.9);

            camRot.y = 0;
        }

        

        prec = min(0.0001f, distance(camPos)/1000.0f);
        //prec = 0.0001f;
        //Seseliuoklei perduodami duomenys
        glUniform3f(glGetUniformLocation(shd, "camPos"), camPos.x, camPos.y, camPos.z);
        glUniform3f(glGetUniformLocation(shd, "camRot"), camRot.x, camRot.y, camRot.z);
        glUniform2f(glGetUniformLocation(shd, "aspect"), SCREEN_WIDTH, SCREEN_HEIGHT);
        glUniform3f(glGetUniformLocation(shd, "style"), back_color.x, back_color.y, back_color.z);
        glUniform3f(glGetUniformLocation(shd, "objcolor"), obj_color.x, obj_color.y, obj_color.z);
        glUniform1f(glGetUniformLocation(shd, "mouseSens"), mouseSens);
        glUniform1i(glGetUniformLocation(shd, "glow"), glow);
        glUniform1i(glGetUniformLocation(shd, "coloring"), color);
        glUniform1i(glGetUniformLocation(shd, "msaa"), msaa);
        glUniform1f(glGetUniformLocation(shd, "prec"), prec);
        glUniform4f(glGetUniformLocation(shd, "var"), vr.x, vr.y, vr.z, vr.w);
        glUniform3f(glGetUniformLocation(shd, "var2"), abs(vr2.x), vr2.y, vr2.z);
        glUniform1f(glGetUniformLocation(shd, "AO"), AO);
        glUniform1f(glGetUniformLocation(shd, "eps"), eps);
        glUniform1f(glGetUniformLocation(shd, "glowMult"), glowThrMult);
        glUniform1f(glGetUniformLocation(shd, "fov"), DegToRad(fov));
        glUniform1f(glGetUniformLocation(shd, "lightIntensity"), softShadow);
        glUniform1i(glGetUniformLocation(shd, "stepLimit"), steps);

        glDrawArrays(GL_TRIANGLES, 0, 6);                

        //Sukuriamas Imgui parametru langas 
        ImGui::Begin("Parametrai");
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

        ImGui::Text("Apsvietimas");
        ImGui::Checkbox("Svytejimas", &glow);
        ImGui::SliderFloat("Svytejimo slenkstis", &glowThrMult, 1.0f, 200.0f);
        ImGui::SliderFloat("Minimali apsvieta", &AO, 0.0f, 1.0f);
        ImGui::SliderFloat("Seseliu astrumas", &softShadow, 1.0f, 256.0f);
        ImGui::Checkbox("Spalvinimas nuo centro", &color);
        ImGui::ColorEdit3("Objekto spalva", (float*)&obj_color);
        ImGui::ColorEdit3("Fono spalva", (float*)&back_color);

        ImGui::Text("Ziurejimas");
        ImGui::Checkbox("Relatyvus greitis", &relSpeed);
        ImGui::SliderInt("Matymo lauko kampas", &fov, 30, 150);
        ImGui::SliderFloat("Peles jautrumas", &mouseSens, 0.1f, 2.0f);
        //ImGui::SliderFloat("Gilumas", &prec, 1, 8);
        
        ImGui::Text("Kintamieji");
        ImGui::SliderInt("Zingsniu limitas", &steps, 10, 500);
        ImGui::SliderFloat("Epsilon (10^(-E))", &eps, 2, 10);
        ImGui::SliderInt("MSAA lygis", &msaa, 0, 3);
        ImGui::Text("4D Kintamasis x");
        ImGui::SliderFloat("Xx", &vr.x, -varLimit, varLimit);
        ImGui::SliderFloat("Xy", &vr.y, -varLimit, varLimit);
        ImGui::SliderFloat("Xz", &vr.z, -varLimit, varLimit);
        ImGui::SliderFloat("Xw", &vr.w, -varLimit, varLimit);
        ImGui::Text("3D Kintamasis y");
        ImGui::SliderFloat("Yx", &vr2.x, -varLimit, varLimit);
        ImGui::SliderFloat("Yy", &vr2.y, -varLimit, varLimit);
        ImGui::SliderFloat("Yz", &vr2.z, -varLimit, varLimit);

        ImGui::Text("Atstumas iki objekto: %.10f", distance(camPos));
        ImGui::Text("Greitis: %.1f", speed);
        ImGui::Text("Pozicija: X: %.10f, Y: %.10f, Z: %.10f", camPos.x, camPos.y, camPos.z);
        ImGui::Text("Pasisukimas: X: %.1f, Y: %.1f, Z: %.1f", RadToDeg(camRot.x), RadToDeg(camRot.y), RadToDeg(camRot.z));

        //Informacija
        if (ImGui::Button("Info", ImVec2(100, 30)))
        {
            InfoPopUp();
        }

        //Kadro pabaiga

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        camPosDif = vec3(0);
        camRotDif = vec3(0);
        var = vr;
        var2 = vr2;
        
        glfwPollEvents();
        glfwSwapBuffers(window);

        if (vid)
        {
            captureImage();
        }
    }

    //Uzdarius programa atlaisvinami resursai
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shd);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//Klaviaturos ivestis
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //std::cout << "key " << key << "\tscancode " << scancode << "\taction " << action << "\tmods " << mods << "\n";

    switch (action)
    {
        //for single tap
        case GLFW_PRESS:
        {
            switch (key)
            {
                case GLFW_KEY_V:
                {
                    viewFocus = !viewFocus;
                    focusInit = true;
                    break;
                }
                case GLFW_KEY_I:
                {
                    captureImage();
                    break;
                }
                case GLFW_KEY_J:
                {
                    vid = true;
                    break;
                }
                case GLFW_KEY_F1:
                {
                    InfoPopUp();
                    break;
                }
                case GLFW_KEY_T:
                {
                    break;
                }
            }
            break;
        }
    }
}

//Peles rato ivestis
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    speed += (float)(yoffset) * 0.1f;
    
    if (speed < 0.001f)
        speed = 0.001f;
    if (speed > 100.0f)
        speed = 100.0f;
}