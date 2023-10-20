[English]

# 3D-Julia
Fractal visualization in 3D using raymarching

![test](https://user-images.githubusercontent.com/74368343/207424131-724f8ee7-4da7-4546-b7be-28024e916d0d.png)

Promo videos:

https://www.youtube.com/watch?v=PxuoigfIPjc

https://www.youtube.com/watch?v=f6RKcRPP5oQ

Created with C++, OpenGL, GLFW, ImGui and FreeImage libraries by Adomas Bieliūnas in the end of 2022
OpenGL - graphics
GLFW - display and IO
ImGui - parameter window
FreeImage - image capturing 

Execute run.bat to start the program 

Main code is in 3D-Julia/src/vendor/app.cpp, x64/Release/frac.shader and x64/Release/functions.h files

To modify the code you need to open the .sln file if you have Visual Studio 2022

If you get an error with FreeImage.dll try commenting out the code inside the captureImage function



[Lietuvių]

# 3D-Julia
Fraktalų atvaizdavimas trimatėje erdvėje

![test](https://user-images.githubusercontent.com/74368343/207424131-724f8ee7-4da7-4546-b7be-28024e916d0d.png)

Promo video:

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
