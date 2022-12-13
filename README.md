# 3D-Julia
Fraktalų atvaizdavimas trimatėje erdvėje

https://youtube.com/shorts/1Y2Nwu6XFhU

Sukurta su C++, OpenGL, GLFW, ImGui ir FreeImage bibliotekomis Adomo Bieliūno 2022 metų pabaigoje
OpenGL - kompiuterio GPU naudojimas
GLFW - lango ir įvesties tvarkymas
ImGui - parametrų langas
FreeImage - vaizdo išsaugojimui

Norint naudotis programa reikia tiesiog palesiti run.bat failą

Pagrindinis kodas yra 3D-Julia/src/vendor/app.cpp, x64/Release/frac.shader ir x64/Release/functions.h failuose

Kad pakeisti kodą galima atidaryti .sln failą, jei yra instaliuota Visual Studio 2022

Jei yra metama klaida su FreeImage.dll failu galima užkomentoti app.cpp esančios captureImage funkcijos kodą
