SynTool Starter
===============

Interactive tool of synthesizing new generation data

====================================================

git repository

https://github.com/SpiritD/syntool

==================================

Dependences

freeglut

Install on Debian
```bash
sudo apt-get install freeglut3
```
Install on Fedora
```bash
sudo yum install freeglut-devel
```
Install on Gentoo
```bash
sudo emerge media-libs/freeglut
```

==================================

Directories
./depot/qt5 for the git
./build/qt5 for build
./syntool   for SynTool Starter

===============================

Build Qt5.1 + Qt3D on linux
```bash
mkdir depot
mkdir -p build/qt5

cd depot
git clone git://gitorious.org/qt/qt5.git qt5
git clone git://gitorious.org/qt/qt3d.git qt3d

./init-repository --no-webkit
cd ../../build/qt5
../../depot/qt5/configure -developer-build -opensource -nomake examples -nomake tests -confirm-license && make -j 9
./qtbase/bin/qmake ../../depot/qt5/qt3d/qt3d.pro
make -j 9
```
==================================================

Build SynTool Starter. And Run.
```bash
git clone https://github.com/SpiritD/syntool
cd syntool
../build/qt5/qtbase/bin/qmake syntool.pro
make -j9

bin/syntool
```
=============

![ScreenShot](http://wiki.solab.rshu.ru/images/0/06/Syntool_layers.png)
