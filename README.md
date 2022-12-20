# [MPI - Determinant of NxN Matrix](https://github.com/Rhtymn/MPI-Matrix-Determinant)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

## Introduction
Repo ini adalah hasil pekerjaan kami untuk tugas project akhir mata kuliah Komputasi Tersebar dan Paralel mengenai Message Passing Interface (MPI). Pada project ini kami mengambil tema mengenai determinan matriks NxN dengan metode perhitungan menggunakan minor dan cofactor. Dari tema tersebut kami mencoba membuat program yang dapat dijalankan secara serial dan paralel. Setelah itu akan dilakukan pengamatan perbedaan waktu eksekusi secara serial maupun paralel dengan beberapa jumlah process yang berbeda.

## Team
Anggota kelompok pada tugas project ini antara lain:
1. Agnes Triana Cyntianesa (24060120120021)
2. Arifa Alif Malicha Khairunnisa (24060120120033)
3. Dimas Wahyu Ardiyanto (24060120140159)
4. Muhammad Hafizh Roihan (24060120130123)
5. Zara Zetira Puti (24060120120030)

## Reference
Pemilihan ide dan pengembangan program ini terinsipirasi dari penjelasan Geeksforgeeks mengenai determinan matriks. [Read Me!](https://www.geeksforgeeks.org/determinant-of-a-matrix/).

## Tech
Project ini menggunakan beberapa teknologi sebagai berikut.
- C++
- [Microsoft Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)
- [Microsoft Message Passing Interface v1.1.2](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi)

## Note
Jika ingin melihat code program dapat melihat langsung pada file 'MPI.cpp'. Jika ingin mencoba program ini dapat mengikuti panduan instalasi dan cara pakai di bawah ini.

## Installation
Clone this repository
```
https://github.com/Rhtymn/MPI-Matrix-Determinant.git
```
Make sure you have installed MPI and Microsoft Visual Studio
```
https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi
```
```
https://visualstudio.microsoft.com/downloads/
```
Open this folder project
```
Open file MPI.sln via Microsoft Visual Studio
```
Setting the project properties and follow the tutorial from this link
```
https://youtu.be/BA_Mqi3a9HI
```

## Usage
Build Project in Microsoft Visual Studio
```
Build -> Build Solution
```
Open cmd in directory ../MPI-Matrix-Determinant/x64/Debug/
Try this command
```
mpiexec -n nProcessMPI MPI.exe MatrixSize
```
example:
- MPI process: 3
- Matrix size: 5x5
```
mpiexec -n 3 MPI.exe 5
```
