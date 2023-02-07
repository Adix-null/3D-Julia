# 3D-Julia
Fraktalų atvaizdavimas trimatėje erdvėje

![test](https://user-images.githubusercontent.com/74368343/207424131-724f8ee7-4da7-4546-b7be-28024e916d0d.png)

Promo videos:

https://www.youtube.com/watch?v=PxuoigfIPjc

https://www.youtube.com/watch?v=f6RKcRPP5oQ

Sukurta su C++, OpenGL, GLFW, ImGui ir FreeImage bibliotekomis Adomo Bieliūno 2022 metų pabaigoje
OpenGL - kompiuterio GPU naudojimas
GLFW - lango ir įvesties tvarkymas
ImGui - parametrų langas
FreeImage - vaizdo išsaugojimui

Norint naudotis programa reikia tiesiog palesiti run.bat failą

Pagrindinis kodas yra 3D-Julia/src/vendor/app.cpp, x64/Release/frac.shader ir x64/Release/functions.h failuose

Kad pakeisti kodą galima atidaryti .sln failą, jei yra instaliuota Visual Studio 2022

Jei yra metama klaida su FreeImage.dll failu galima užkomentoti app.cpp esančios captureImage funkcijos kodą
