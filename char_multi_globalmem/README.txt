insmod globalmem.ko
mknod /dev/globalmem c 230 0
mknod /dev/globalmem c 230 1
echo 'hello world'>/dev/globalmem
cat /dev/globalmem
ls tree /sys/module/globalmem

coresize    
initsize    
notes       
refcnt       模块的引用计数   
taint
holders    
initstate   
parameters  
sections     包含的几个文件则给出了globalmem所包含的BSS、数据段和代码段等的地址及其他信息。
uevent

