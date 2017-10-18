## 下载php源代码
    * 官网下载:https://secure.php.net/downloads.php
    * 或GITHUB:git clone http://github.com/php/php-src
---


## 配置选项讲解:`./configure --help | less`
使用`--enable-NAME`和`--disable-NAME`开关编译哪些扩展和SAPI .

如果扩展或SAPI具有外部依赖关系,则需要使用`--with-NAME`和`--without-NAME`.

如果NAME所需的库 不在默认位置(例如,因为您自己编译),则可以使用`--with-NAME = DIR`指定其位置.

如果选项是`--enable-NAME`或`--with-NAME`说明该选项默认是关闭的.如果是`--disable-NAME`或`--without-NAME`说明该选项默认是开启的.

[引用链接](https://segmentfault.com/q/1010000009174725)
* enable 是启用 PHP 源码包自带，但是默认不启用的扩展，比如 ftp 和 exif 扩展。with 是指定扩展依赖的资源库的位置，如果是默认位置，就可以留空。
* php 的扩展分为静态编译和动态编译两种，静态编译就是随着PHP的源码一起编译安装，也就是 --enable 和 --with 启用的扩展。动态编译就是在一个已经可以使用的 PHP 环境下，使用 phpize 命令来给 php 增加扩展的方式，这种方式就是生成的 so 文件。所以想要把扩展编译进 php 内核，就需要和 php 一起编译安装。
* php源码包ext目录下的就是官方默认支持的扩展，还有一部分就是 pecl 库里的扩展，pecl 的一部分扩展也是官方支持的，另一部分是第三方开发者支持的。
* 把扩展跟随php一起编译安装，也就是 configure 命令


##### ~~例如:~~
    ./configure --prefix=/usr/local/php
    --enable-opcache \
    --with-mysqli \
    --with-pdo-mysql=mysqlnd \
    --enable-mysqlnd \
    --enable-inline-optimization \
    --enable-sockets \
    --enable-zip \
    --enable-calendar \
    --enable-bcmath \
    --enable-soap \
    --enable-fpm \
    --with-zlib \
    --with-iconv \
    --with-gd \
    --with-xmlrpc \
    --enable-mbstring \
    --without-sqlite3 \
    --without-pdo-sqlite \
    --with-curl \
    --enable-ftp \
    --with-mcrypt  \
    --enable-pcntl \
    --with-freetype-dir \
    --with-jpeg-dir \
    --with-png-dir \
    --disable-ipv6 \
    --disable-debug \
    --with-openssl \
    --disable-maintainer-zts \

##### 比如 configure 命令行可能看起来象这样：
$ ./configure --prefix=/where/to/install/php --enable-debug --enable-maintainer-zts --enable-cgi --enable-cli --with-mysql=/path/to/mysql


## 我们要进行的配置和编译命令:
    ./configure --disable-all --enable-cli --enable-debug
        (--enable-debug启用调试模式,具有多重效果:
        编译将使用 -g运行以生成包括行号、变量的类型和作用域、函数名字、函数参数和函数的作用域等源文件特性的调试信息.
        另外使用-O0,会让gcc编译时不对代码优化.
        此外,调试模式定义了 ZEND_DEBUG宏,它将启动引擎中的各种调试助手.除其他事项外,还将报告内存泄漏以及某些数据结构的不正确使用.)
    make -jN
        (N为CPU数量,作用:make --help查看)


## 导入eclipse并配置
    1: 右击左侧栏目空白处,点击New -> Project -> c/c++ -> makefile project with existing code ->,再选择PHP源代码目录,同时选择GNU autotools toolchain -> finish
    2: 点击菜单栏RUN->run configurations 在弹出窗口双击c/c++ application添加一个配置,在右侧标签点击search project 然后选择php并确定.点击arguments标签并添加如下参数.
        -r "echo 'run-test-ok';"
    3:点击apply -> close.
    4:运行测试,调试测试,切换透视图


## eclipse快捷键
    Ctrl+左键 或 F3 :跳到光标所在标识符的定义代码.
    Alt+左右方向键 :返回上一个/下一个阅读位置.
    Ctrl+H : 打开查找窗口.
    Ctrl+Shift+G :在工作空间中查找引用了光标所在标识符的位置.可以说是与F3相反的快捷键
    Ctrl+O :查看文件概要信息.
    ------------------
    调试快捷键
    F5进入当前方法,如果当前执行语句是函数调用,则会进入函数里面.
    F6单步执行程序,运行下一行代码
    F7退出当前方法,返回到调用层
    F8继续运行直到下一个断点
    ctrl+b:重新编译
    ctrl+f11:重新运行


## PHP内核源码目录结构
    php-744.1.4
    ├── build   //源码编译相关文件
    └── ext     //官方扩展目录,包括了绝大多数PHP的函数的定义和实现
    └── main    //PHP核心基本文件,这里和Zend引擎不一样,Zend引擎主要实现语言最核心的语言运行环境.
    └── pear    //“PHP 扩展与应用仓库”,包含PEAR的核心文件.
    └── sapi    //包含了各种服务器抽象层的代码,例如apache的mod_php,cgi,fastcgi以及fpm等等接口.
    └── tests   //PHP的测试脚本集合,包含PHP各项功能的测试文件
    └── TSRM    //PHP的线程安全是构建在TSRM库之上的,PHP实现中常见的*G宏通常是对TSRM的封装,TSRM(Thread Safe Resource Manager)线程安全资源管理器.
    └── win32   //Windows平台相关的一些实现,比如sokcet的实现在Windows下和*Nix平台就不太一样,同时也包括了Windows下编译PHP相关的脚本.
    └── Zend    //Zend引擎的实现目录,比如脚本的词法语法解析,opcode的执行以及扩展机制的实现等等.
    └── .gdbinit            //gdb命令编写脚本   (gdb) source /home/laruence/package/php-5.2.14/.gdbinit       (gdb) zbacktrace
    └── CODING_STANDARDS    //PHP编码标准
    └── config.guess        //由automake产生,两个用于目标平台检测的脚本
    └── config.log          //configure执行时生成的日志文件
    └── config.nice         //configure执行时生成,记录了上次执行configure时带的详细参数
    └── config.status       //configure执行时生成,实际调用编译工具构建软件的shell脚本
    └── config.sub          //由automake产生,两个用于目标平台检测的脚本
    └── configure           //配置并生成makefile
    └── configure.in        //autoreconf创建,开发者维护,用于生成configure
    └── CREDITS             //开发人员名单
    └── EXTENSIONS          //扩展说明(维护状态,维护人员,版本,适用系统..)
    └── LICENSE             //发布协议
    └── php.ini-development //PHP开发环境示例配置文件
    └── php.ini-production  //PHP生产环境示例配置文件
    └── README.EXT_SKEL     //构建扩展脚本说明
    └── README.GIT-RULES    //GIT提交时的规则
    └── README.namespaces   //命名空间说明
    └── README.PARAMETER_PARSING_API    //新的参数解析函数说明
    └── README.REDIST.BINS              //PHP中引用到的其它程序协议说明
    └── README.RELEASE_PROCESS          //PHP发布过程说明
    └── README.SELF-CONTAINED-EXTENSIONS//创建一个内建的PHP扩展
    └── README.STREAMS                  //PHP Streams(流概念) 说明
    └── README.SUBMITTING_PATCH         //介绍如何提交PHP的增强功能或修补程序
    └── README.TESTING                  //测试说明(run-tests.php)
    └── README.TESTING2                 //测试说明(server-tests.php)
    └── README.UNIX-BUILD-SYSTEM        //PHP编译系统V5概述
    └── README.WIN32-BUILD-SYSTEM       //WIN32编译说明
    └── run-test.php                    //测试脚本
    └── server-test.php                 //测试脚本
    └── sesrver-test-config.php         //测试脚本
    └── UPGRADING                       //版本更新说明
    └── UPGRADING.INTERNALS             //内部更新说明















