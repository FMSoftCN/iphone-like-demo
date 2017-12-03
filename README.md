编译本程序的前提条件：你需要拥有3.0.2 版本的MiniGUI 源码包或者拥有 3.0.2版本的MiniGUI 产品包。
如果是源码包，则还需要有转换工具包mgadp-tools 和 资源包。 
如果是产品包，则需要先安装好产品包，然后再来编译此程序。

你需要将解压以后生成的文件夹的名字改成iphone-like-demo

如果你是拥有 MiniGUI 源码包，请使用build.sh脚本编译程序。请在iphone-like-demo目录的上层目录使用此脚本。
你需要如下的目录：

  /
  |--mgadp-tools-3.0.2
  |--libminigui-3.0.2
  |--minigui-res-3.0.2
  |--iphone-like-demo

示例：

   ./iphone-like-demo/build.sh 

如果你不拥有MiniGUI 源码包，请使用adp-build.sh脚本编译程序。请在iphone-like-demo目录的上层目录使用此脚本。
你不需要上面情况的目录结构，只需要iphone-like-demo 文件夹
示例：

   ./iphone-like-demo/adp-build.sh
