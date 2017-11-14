#include<linux/init.h>
#include<linux/module.h>
#include<linux/of.h> 	//这个就是要用of函数去获取设备树信息的头文件
#include<linux/of_irq.h> //这是从设备树获取中断号函数需要的头文件
#include<linux/interrupt.h>


int irqnumber;//全局的中断号

irqreturn_t tree_interrput(int irq,void *dev){

printk("enter irqreturn_t\n ");

return IRQ_HANDLED;
}


static int __init dt_drv_init(void){

	/* test_node@123456{                       //获取这个设备树里面的节点
		compatible= "fsl,test_node";
		reg=<0x12345678 0x24
		     0x87654321 0x25>;
		testprop,mytest;
		test_list_string = "red_fish","fly fish","blue fish";
		interrupt-parent = <&gpio6>;
		interrupts = <15 2>;			
	 };*/

	struct device_node *np=NULL; //先定义一个节点返回的结构体，然后后面的of函数都是以这个节点为基准来获取数据的
	struct property *prop=NULL;  //获取节点属性后要存放的结构体变量
	u32 regdata[4];//读取属性中的整数数组，需要先定义设备树reg中数组的个数
	int ret;
	/*在代码中获取节点的所有信息*/
	np=of_find_node_by_path("/test_node@123456"); //先用这个获取节点
	if(np){
	printk("find test_node@123456 ok\n");
	printk("np name : %s\n",np->name); //这里获取的name就是节点的名字，但是节点@后面的数字是不获取的，只获取@前面的字符	
	printk("full name : %s\n",np->full_name);  //这个full_name变量就是要打印整个节点的字符，包含@前后的
	}
	else{

	printk("not find node\n");
	}


	/*获取节点的属性*/
	prop=of_find_property(np,"compatible",NULL);////np节点下面有很多个属性，比如compatible，reg，testprop,mytest； test_list_string 你要获取那个属性里面等于的值就必须先获取属性	
	if(prop){
	printk("find property\n");
	printk("compatible value : %s\n",prop->value);//打印该属性=的参数
	printk("compatible name : %s\n",prop->name);  //打印该属性的名字
	
	}	
	else{
	printk("not find property\n");
	}
	if(of_device_is_compatible(np,"fsl,test_node"))//这个函数就不用像of_find_property(np,"compatible",NULL)这样非要在引号里面去写属性名称，它是直接就默认有compatible这个属性，然后就去调用该属性的值来判断这个值是否存在，该方法可以用来做条件判断
	{

		printk("fsl test_node\n");

	}
	else
		printk("not fsl test_node erro\n");

	/*读取属性reg中的整数数组*/
	ret=of_property_read_u32_array(np,"reg",regdata,4);
	if(!ret){
		int i;
		printk("get reg array success\n");
		for(i=0;i<4;i++){

			//printk("-------reg = %d\n",regdata[i]);//按照10进制打印是错误的，因为设备树定义的是16进制
			printk("-------reg = %x\n",regdata[i]);//打印16进制就对了
		}
	}
	else
		printk("get reg array failed\n");


	/*读取属性中字符串数组*/
	const char *pstr[3];//先定义个二级数组来存放设备树的字符串
	//ret=of_property_read_string(np,"test_list_string",pstr);//这个函数只能获取一个字符串，如果是多个字符串获取就要出问题
	
		int z;
		for(z=0;z<3;z++){
		if(!ret){
			ret=of_property_read_string_index(np,"test_list_string",z,&pstr[z]);//这个函数是指定字符串数组的第几个然后再去获取，并不一次性将设备树的字符串数组获取完
			printk("pstr = %s\n",pstr[z]);

		}
		else
		
		printk("get string erro\n");
	}
	
	/*设备树属性值为空，但是可以作为标志来用*/
		if(of_find_property(np,"testprop,mytest",NULL)){  //其实和of_device_is_compatible是一样的用来确定设备树有没有这个标志，有这个标志实现什么功能，没有这个标志实现什么功能
		 printk("testprop,mytest TRUE\n");
		}
		else
			printk("testprop,mytest failed\n");
		
		/*从设备树中获取中断号*/
		irqnumber=irq_of_parse_and_map(np,0);//从设备树中获取中断号15
        printk("irqnumber = %d\n\n",irqnumber);
        
		/*irqnumber=irq_of_parse_and_map(np,0);//从设备树中获取中断号
		printk("irq0 = %d\n",irqnumber); //打印获取的中断号
		irqnumber=irq_of_parse_and_map(np,1);//从设备树中获取中断号
		printk("irq1 = %d\n",irqnumber); //打印获取的中断号
		irqnumber=irq_of_parse_and_map(np,2);//从设备树中获取中断号
		printk("irq2 = %d\n",irqnumber); //打印获取的中断号*/
/*这三个外部中断号分别是15 14 13 这时irq_of_parse_and_map(np,0);函数的第二位参数就是要取设备树该节点下中断,第几个尖括号里面的中断号*/

		/*比如：
		 * 设备树interrupts = <15 2>,<14 2>,<13 2>;
		 * irqnumber=irq_of_parse_and_map(np,0); //代表取<15 2> 里面的15号中断
		 * irqnumber=irq_of_parse_and_map(np,1); //代表取<14 2> 里面的14号中断
		 * irqnumber=irq_of_parse_and_map(np,2); //代表取<13 2> 里面的13号中断*/

	return 0;
}


static void __exit dt_drv_exit(void){

free_irq(irqnumber,NULL);
}


module_init(dt_drv_init);
module_exit(dt_drv_exit);
MODULE_LICENSE("GPL");
