insmod globalmem.ko
mknod /dev/globalmem c 230 0
mknod /dev/globalmem c 230 1
echo 'hello world'>/dev/globalmem
cat /dev/globalmem
ls tree /sys/module/globalmem

coresize    
initsize    
notes       
refcnt       ģ������ü���   
taint
holders    
initstate   
parameters  
sections     �����ļ����ļ��������globalmem��������BSS�����ݶκʹ���εȵĵ�ַ��������Ϣ��
uevent

