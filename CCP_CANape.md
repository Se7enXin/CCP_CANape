# CANape与ECU 

## 1、CANape的数据库.a2l文件

### （1）描述：  

	.a2l文件中包含了程序的参数和地址，由程序编译后bin文件夹中的.map或者.pdb或者.elf等文件生成

### （2）生成步骤：  

<font color = "blue">**1、打开ASAP2 Editor**</font>  
![](http://i.imgur.com/RXOgg2x.jpg)  

<font color = "blue">**2、在菜单栏中选择File，点击New**</font>    
![](http://i.imgur.com/s2HH7S5.jpg)

<font color = "blue">**3、在菜单栏中选择Tool，点击Settings..**</font>  
![](http://i.imgur.com/U0H7Rj1.jpg)

<font color = "blue">**4、在新弹出的窗口中点击MAP File(s)，选择导入外部的数据库，将bin文件夹中编译后的文件加载进来并选择对应的格式（.map对应Hware HC12），之后点击OK,再弹出窗口也点击OK。**</font>  
![](http://i.imgur.com/4tRjgRU.jpg)

<font color = "blue">**5、右键，点击New，选择Parameter**</font>  
![](http://i.imgur.com/I3HKJlT.jpg)

<font color = "blue">**6、对Parameter设置**</font>   
<font color = "blue">**name：**</font>   
![](http://i.imgur.com/GaWn0ox.jpg) 

<font color = "blue">**Address（点击Link，选择具体的参数）：**</font>  
![](http://i.imgur.com/Xg8OB7A.jpg)

<font color = "blue">**数据类型&字节序（一定注意字节序）：**</font>  
 ![](http://i.imgur.com/zISwDh0.jpg)

<font color = "blue">**7、完成，保存，退出：**</font>   
<font color = "blue">**6步骤完成后一个参数就新建成功了，可以根据需求添加其他的参数或者其他。此时保存后退出，.a2l文件就完成了。**</font> 
![](http://i.imgur.com/nMqIIjS.jpg)

## 2、CANape模型

### （1）描述：

	在Vector CANape中搭建模型，使ECU与上位机通过CANape建立连接。

### （2）生成步骤： 

<font color = "blue">**1、打开CANape上位机软件，依次加入Symbol Explorer、Device Window、Trace Window（导入.a2l后才可加入），完成后界面如下图：**</font>  
![](http://i.imgur.com/G7yeUF3.jpg)

<font color = "blue">**2、上位机CCP的配置，主要对下图中方框中的部分进行配置：**</font>  
![](http://i.imgur.com/9wA8GTV.jpg)

<font color = "blue">**波特率配置：**</font>  
![](http://i.imgur.com/Ajoj2yZ.jpg)

<font color = "blue">**字节序、CCP协议版本配置：**</font>  
![](http://i.imgur.com/0HMmEhA.jpg)

<font color = "blue">**CRO、DTO、ECU Address、波特率配置：**</font>  
![](http://i.imgur.com/LDKdMNS.jpg)

<font color = "blue">**CAN通道选择：**</font> 
![](http://i.imgur.com/JMev6C3.jpg)

<font color = "blue">**3、加入参数（将.a2l中的参数加入）：**</font>  
![](http://i.imgur.com/T8Z7cP5.jpg)

<font color = "blue">**最后的界面：**</font>  
![](http://i.imgur.com/dpF9EQY.jpg)