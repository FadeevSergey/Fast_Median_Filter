Медианный фильтр с окном фильтрации 3×3.
---
***
 Цифровых фильтр, используемый в обработке изображений для уменьшения уровня шума.
 Реализован на языке С++ с использованием фреймворка OpenCV
 
#### Описание работы алгоритма
###### Алгоритм поиска двух медиан:
Поиск медиан пикселей происходит слева направо, сверху вниз.
На каждом шаге будем искать медианы сразу двух соседних пикселей. Храним 4 массива длинны 3 (обозначены на рис. 1)
На новой строке изначально находимся в пикселе F(2, y), будем искать медианы пикселя F(2, y) и S(3, y). Заполним 4 массива значениями нужных пикселей (firstTempArray пикселями (1, y - 1) - (1, y + 1), secondTempArray пикселями (2, y - 1) - (2, y + 1) и т.д)
0) Отсортируем firstTempArray и secondTempArray
1) Отсортируем thirdTempArray и fourthTempArray
2) Сольем secondTempArray и thirdTempArray, назовем новый массив result.
3) Медиана F - это медиана слияния result с firstTempArray
4) Медиана S - это медиана слияния result с secondTempArray
*Операция слияния аналогична одноименной операции в mergeSort* 



На следующем шаге сдвинемся на 2 позиции право. Будем искать медианы пикселей (4, y) и (5, y). Заметим, что новые "зеленый" и "синий" массивы пикселей соответствуют thirdTempArray и fourthTempArray из предыдущего шага. Эти два массива остались отсортированными после предыдущего шага. Тогда сделаем swap(firstTempArray, thirdTempArray), swap(secondTempArray, fourthTempArray), заполним thirdTempArray пикселями (5, y - 1) - (5, y + 1), fourthTempArray пикселями (6, y - 1) - (6, y + 1) и вернемся к пункту 1.
![Image alt](https://sun9-2.userapi.com/c857136/v857136573/199910/Wju8ck7I-6M.jpg)
рис. 1

Если ширина изображения нечетна (как например в случае выше), то два последних ряда останутся необработанными, в этом случай запустим аналогичный алгоритм поиска двух медиан по предпоследнему столбцу сверху вниз (рис. 2). Если же нечетна и высота, то пиксель предпоследней строки предпоследнего столбца окажется необработан. Найдем его медиану отдельно.
![Image alt](https://sun9-31.userapi.com/c206828/v206828573/124b79/29i1ksCaVIM.jpg)  рис. 2
Данный алгоритм не обрабатывает пиксели по краям. Их мы обработаем отдельно, найдя медианы соседних 4, если пиксель угловой, или 6 элементов, если пиксель на первой-последней столбце-строке, но не в углу. 
Зато данный алгоритм будет работать с любыми изображениями, кроме тех, чья длина или ширина меньше или равна нулю
#### Сборка и запуск с помощью CMake:
##### В CLion
Чтобы собрать и запустить программу, например в CLion, достаточно просто открыть папку с CMakeLists.txt, main.cpp, MedianFilterCV_8U.h и  MedianFilterCV_8U.cpp, CLion сам найдет CMakeLists.txt и сделаем все за нас)
##### В терминале
Для начала откроем терминал и перейдем в директорию, где лежит CMakeLists.txt, main.cpp, MedianFilterCV_8U.h и  MedianFilterCV_8U.cpp
Создадим makefile, сделаем это с помощью СMakeLists.txt:
```sh
# CMakeLists.txt
cmake_minimum_required(VERSION 3.13)
project(MedianFilter)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

#set(CMAKE_PREFIX_PATH "/Users/Fadeev/installation/OpenCV-")
set(CMAKE_PREFIX_PATH "YOUR_PATH_TO_OPENCV") 

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(MedianFilter main.cpp MedianFilterCV_8U.h MedianFilterCV_8U.cpp)
target_link_libraries(MedianFilter ${OpenCV_LIBS})
```
*Не забудьте заменить YOUR_PATH_TO_OPENCV на правильный префикс.*

Запустим:
```sh
$ cmake CMakeLists.txt
```
После этого должен создаться Makefile, запустим его:
```sh
$ make
```
Теперь должен создаться исполняемый файл MedianFilter, который можно запустить следующей командой:
```sh
$ ./MedianFilter PATH_TO_INPUT_IMAGE PATH_TO_OUTPUT_IMAGE THRESHOLD_VALUE
```
К примеру команда ниже применяет алгоритм медианной фильтрации к изображению in.jpeg, находящемуся в этой же директории и сюда же сохраняет результат с именем out.jpeg. 
Используемое пороговое значение при этом равно 30
```sh
$ ./MedianFilter in.jpeg out.jpeg 30
```

*Пожалуйста, указывайте THRESHOLD_VALUE в диапазоне от 0 до 255 
Если THRESHOLD_VALUE не указать, то по умолчанию выставится значение, равное нулю*

## Пример работы:

###### Пороговое значение равно 15
![Image alt](https://sun9-50.userapi.com/c857432/v857432040/1e4c97/gEAHO9FYRkU.jpg)
###### Пороговое значение равно 30
![Image alt](https://sun9-39.userapi.com/c857432/v857432517/1e6400/gifVDwfZCKs.jpg)
![Image alt](https://sun9-53.userapi.com/c857432/v857432517/1e640a/zBmjpfIQqAE.jpg)
###### Пороговое значение равно 45
![Image alt](https://sun9-51.userapi.com/c857432/v857432517/1e63ed/UWWMen-KEfo.jpg)
![Image alt](https://sun9-34.userapi.com/c857432/v857432517/1e63f6/AEG85LXTLHk.jpg)
###### Пороговое значение равно 60
![Image alt](https://sun9-62.userapi.com/c857432/v857432040/1e4c88/XwuVlMchBjU.jpg)
![Image alt](https://sun9-29.userapi.com/c857432/v857432040/1e4c8f/lQfjkwSNTNg.jpg)

## Подсчет количества операций для изображения размера 1920×1080
Самый сложный момент, как мне кажется. Непонятно что считать за операцию. К примеру операция ++i - это одна операция? Операция - это одна ассемблер команда?
При подсчете было допущено достаточно много волностей, но итоговое значение - 226 564 958
В коде расписано подробнее

Спасибо за внимание :)
