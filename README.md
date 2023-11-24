# 3D-Julia

## English

>Fractal visualization in 3D

Created by Adomas Bieliūnas in the end of 2022

![test](https://user-images.githubusercontent.com/74368343/207424131-724f8ee7-4da7-4546-b7be-28024e916d0d.png)

### Promo videos

https://www.youtube.com/watch?v=PxuoigfIPjc

https://www.youtube.com/watch?v=f6RKcRPP5oQ

### Libraries used
* OpenGL - graphics
* GLFW - display and IO
* ImGui - parameter window
* FreeImage - image capturing 

### Using the program 
**Execute run.bat to start the program**

The program code is in 3 main files

* [app.cpp](https://github.com/Adix-null/3D-Julia/blob/master/3D-Julia/src/vendor/app.cpp) - main file controlling the window and I/O
* [frac.shader](https://github.com/Adix-null/3D-Julia/tree/master/x64/Release/frac.shader) - raymarching logic and lighting algorhitms
* [functions.h](https://github.com/Adix-null/3D-Julia/tree/master/x64/Release/functions.h) - SDF definitions

To modify the code you need to open the .sln file if you have Visual Studio 2022

To use the desired SDF return the SDF value in the distance() function located in the functions.h file

If you get an error with FreeImage.dll try commenting out the code inside the captureImage function


## Lietuvių

>Fraktalų atvaizdavimas trimatėje erdvėje

Sukurta Adomo Bieliūno 2022 metų pabaigoje

![test](https://user-images.githubusercontent.com/74368343/207424131-724f8ee7-4da7-4546-b7be-28024e916d0d.png)

### Promo vaizdo įrašai:

https://www.youtube.com/watch?v=PxuoigfIPjc

https://www.youtube.com/watch?v=f6RKcRPP5oQ

### Naudotos bibliotekos
* OpenGL - kompiuterio GPU naudojimas
* GLFW - lango ir įvesties tvarkymas
* ImGui - parametrų langas
* FreeImage - vaizdo išsaugojimui

### Programos naudojimas
**Norint naudotis programa reikia palesiti run.bat failą**

Programos kodas yra 3 pagrindiniuose failuose

* [app.cpp](https://github.com/Adix-null/3D-Julia/blob/master/3D-Julia/src/vendor/app.cpp) - pagrindinis failas kontroliuojantis langą ir įvestį bei išvestį
* [frac.shader](https://github.com/Adix-null/3D-Julia/tree/master/x64/Release/frac.shader) - spindulių žingsniavimo logika ir šviesos algoritmai
* [functions.h](https://github.com/Adix-null/3D-Julia/tree/master/x64/Release/functions.h) - SDF apibrėžimai

Kad pakeisti kodą galima atidaryti .sln failą, jei yra instaliuota Visual Studio 2022

Norint pasirinkti SDF reikia grąžinti SDF vertę distance() funkcijoje functions.h faile

Jei yra metama klaida su FreeImage.dll failu galima užkomentoti app.cpp esančios captureImage funkcijos kodą
