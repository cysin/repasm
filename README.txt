
*------------------------------------------------
*              Building RepASM
*------------------------------------------------*

under Unixes
------------

WARNING : only 32-bit platforms are supported at this time. It is likely the build will fail on 64-bit so if you want to do it anyway, it will be at your own risk. Do NOT contact us for any support concerning that kind of platform as we will be unable to help you.



Linux:
Check the "config" file in the Rexplorer directory and modify the PHPDIR variable according to your installation. Then just type "make". repasm.so will be generated in Rexplorer/libs.


Bsd:
Check the "config" file in the Rexplorer directory and modify the PHPDIR variable according to your installation. 
WARNING : There is good chance just that typing make won't make it, due to differences between GNU make and BSD make. Using gmake instead of make should get things working fine.


We were able to build repasm under Ubuntu (gcc 4.1.2). If you encounter any problem during the build please mail us at repasm@repasm.net with a complete description of the compiler's error messages so that we can help you fixing this out.







With Visual Studio
-------------------

1/ensure that you are in "release" mode (build / configuration manager)
2/go to the repasm property, C/C++ -> General -> Additional Include Directories. Any path containing "php" should be modified to reflect your installation of the php soure code
3/add the php5ts.lib (or php4ts.lib) to the repasm project (right click on repasm, add existing item)
4/build the repasm project. repasm.dll will be generated in the repasm\release directory.

Repasm was developped using Visual Studio 2005. As for the compatibility with other Visual Studio versions, we didn't try and also don't expect it to work.


*--------------------------------------------------*
		Jpeg support
*--------------------------------------------------*

you can disable the libjpeg support in repasm. There is little reason to do this, as libjpeg is widely available, but... Anyway, if you want to disable it add RX_NO_JPEGLIB to the additionnal preprocessor definition of repasm and rxmaptools projects (VS), or add it in the config file (Unixes). 


*--------------------------------------------------*
		Contact
*--------------------------------------------------*

http://repasm.net
repasm@repasm.net



