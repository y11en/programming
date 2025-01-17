#! /bin/sh

if [ "$#" -gt "0" ]
then
    for dir in $@
    do
        cd $dir"_build"
        rm CMakeCache.txt -rf
        cmake "../"$dir -DCMAKE_INSTALL_PREFIX=/opt/kde/
        make -j8
        sudo make install
        sudo /sbin/ldconfig
        echo "make "$dir "successfully !"
        cd ..
    done
else
for dir in kdesupport kdelibs kdebase kdeartwork kdegraphics kdeplasma-addons 
#kdepimlibs kdepim kdeadmin
    do
        cd $dir"_build"
        rm CMakeCache.txt -rf
        cmake "../"$dir -DCMAKE_INSTALL_PREFIX=/opt/kde/
        make -j8
        sudo make install
        sudo /sbin/ldconfig
        echo "make "$dir "successfully !"
        cd ..
    done
fi

