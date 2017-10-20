---

---
# **译文::**
---

---

# PHP 源码编码标准翻译

任何想要添加或更改PHP源码的开发者都应该遵循这个文档下面列出的几个标准。此文件从PHP v3.0版本的开发末期阶段被添加,PHP的代码还没有完全遵循下面的标准, 但是会一直朝着这方向发展, 现在PHP5已经发布了, 大部分已经遵循了下面的规则

---

## 代码实现规则

1. **在源文件和技术手册中对你的代码做好描述**

2. **函数不应该释放传入的指针资源**    
    例如 `function int mail(char *to, char *from)` 不应该释放参数 to 或 from .

    除了以下情况:
    * 函数本身被设计为释放资源 例如 efree() .
    * 给定的函数参数包含一个布尔值, 这个参数被用来控制是否释放,如果为真 则函数释放参数, 如果为假 则不释放 .
    * 与底层解析器程序紧密集成的令牌缓存或内存拷贝开销最小的语法分析器生成器(GNU bison)代码,(可以理解为C语言内嵌的一些汇编代码) .

3. **在同一个模块(module)中和其它函数耦合和彼此依赖的函数，应做好注释并用static声明。最好避免有这种情况**

4. **尽量使用definitions和macros，使常量有一个有意义的名字方便操作。除了0和1分别被当作false, true时可以例外,所有其它使用数值常量指定不同行为的情况都应该使用#define**

5. **当编写和字符串处理相关的函数时,要知道PHP会记住每个字符串的长度属性(使用zend_parse_parameters_ex获取到的字符串长度),不要用strlen来计算它的长度(如果不是\0结尾的字符串strlen的计算是不准确的) .**
    这样编写的函数能充分利用长度属性，对效率和二进制安全都有好处。
    
    使用函数改变字符串并获得他们的新长度的同时,应该返回新的长度,这样就不用strlen重新计算了(例如 php_addslashes()) .

6. **绝对不要使用 strncat()函数. 如果你确定知道你在做什么. 请再三查看该函数的使用手册, 想清楚再使用. 即使这样也请尽量避免使用.**

7. **在PHP的源码部分使用宏 PHP_* , 在Zend引擎源码部分使用宏 ZEND_***
    
    尽管几乎所有的宏 PHP_* 只是宏 ZEND_* 的一个别名. 但是它可以使代码的阅读性更好, 更容易理解你调用的是哪种宏 .

8. **当注释掉一段代码的时候不要只使用 #if 0 来声明.**

    用git的账号加上_0 来代替, 例如 #if FOO_0, FOO表示你的git账号. 这使得更容易跟踪代码被注释掉了的原因, 尤其是在捆绑库的时候.

9. **不要定义不可用的函数**
    
    例如 一个库缺少一个函数或者这个函数没有定义响应的PHP版本 . 如果在使用方法的时候也没有报出方法不存在的运行时错误 , 用户需要使用 function_exists() 去测试这个方法是否存在 .

10. **emalloc(), efree(), estrdup() 等函数是对标准C库的一个封装.**

    这些方法实现了一个内部安全的机制,以确保回收没有被释放的内存 , 而且这些函数在debug模式下还提供了非常有用的分配和溢出的信息 .
    
    在绝大多数情况下, 引擎中使用的内存必须使用 emalloc()函数来分配 .
    
    只有在有限情况下才可能需要用malloc()等C库函数来控制或释放内存 比如使用第三方库 , 或者当这块内存的生命周期需要在多个请求中生存的时候 .

---

## 用户函数/方法名编码规则约定

1. **用户级函数的函数名应该用封闭的PHP_FUNCTION()宏**

    函数名应该是小写的,用字母和下划线组合,尽量缩短函数名 ,但也不要用缩写,会降低函数名本身的的可读性 :
    ```code
    Good:
    'mcrypt_enc_self_test'
    'mysql_list_fields'

    Ok:
    'mcrypt_module_get_algo_supported_key_sizes'
    (could be 'mcrypt_mod_get_algo_sup_key_sizes'?)
    'get_html_translation_table'
    (could be 'html_get_trans_table'?)

    Bad:
    'hw_GetObjectByQueryCollObj'
    'pg_setclientencoding'
    'jf_n_s_i'
    ```
2. **如果函数是父集的一部分, 父集名称应该包含在这个函数名中**

    并且应该明确该函数在父集中的关联性, 应该用 parent_* 形式来命名 , 例如和foo相关的一组函数集 :
    ```code
    Good:
    'foo_select_bar'
    'foo_insert_baz'
    'foo_delete_baz'

    Bad:
    'fooselect_bar'
    'fooinsertbaz'
    'delete_foo_baz'
    ```
3. **功能型内部使用的函数名应该加前缀_php_**

    紧随其后的应该是一个单词或用下划线分割的一组词, 并用小写字母描述这个方法 .
    如果可以的话, 这里尽量加static关键字 [这条可以去参考PHP源码里的_php_stream_write_buffer函数] .

4. **变量名必须是有意义的. 避免使用一个字母的变量名**

    除非这个变量名没有实际意义或微不足道的意义. 例如for循环中的i .

5. **变量名应该用小写字母. 词与词之间用下划线分割**

6. **方法名如'studlyCaps'(可参考驼峰命名法) 命名约定.**

    尽量简化名字的字母个数,开头的字母名字是小写的并且名字新词的首字母是大写的 :
    ```code
    Good:
    'connect()'
    'getData()'
    'buildSomeWidget()'

    Bad:
    'get_Data()'
    'buildsomewidget'
    'getI()'
    ```

7. **类应该给出描述性的名称. 尽可能避免使用缩写.**

    每个词的类名应该用大写字母开始. 没有下划线分隔符(CamelCaps从一个大写字母开始) , 类名应该有"父集"的名称前缀(例如扩展的名称[可参考PDO的命名规则]) :
    ```code
    Good:
    'Curl'
    'FooBar'

    Bad:
    'foobar'
    'foo_bar'
    ```
---

## 内部函数命名约定

1. **为了避免冲突,外部API函数应该用'php_modulename_function()' 这种形式命名**

    并且小写,用下划线分割. 对外暴露的API必须定义在你自己的 'php_modulename.h'头文件中. 例如 : `PHPAPI char *php_session_create_id(PS_CREATE_SID_ARGS);`

    不对外暴露的api不要定义在你的'php_modulename.h' 头文件中,而且应该用 static关键字修饰. 例如 : `static int php_session_destroy(TSRMLS_D)`

2. **主模块的源文件必须命名为 'modulename.c'.[ modulename 是你自己起的模块的名字 ]**

3. **主模块的头文件被包含在其他的源文件中,必须命名为'php_modulename.h'.**

---

## 语法和缩进

1. **不要使用C++风格的注释(也就是 // 注释)**

    用C风格的注释去代替它. PHP是用C写的, 并且意图在任何 ANSI-C 编译器下编译 .
    
    尽管许多编译器能够支持在C代码中使C++风格的注释, 但是为了确保你的代码兼容所有编译器最好还是使用C风格的注释 .
    
    唯一例外的是在win32下的代码, 因为Win32 使用的是 MS-Visual C++ ,这个编译器是明确支持在C代码中使用C++风格注释的 .

2. **使用K&R 风格. 当然 我们不能也不想强迫任何人用或不用这种风格**

    但是 最起码, 当你写的代码提交到PHP的核心 或 标准模块之一 时, 请保持 K&R 风格 .
    
    这种风格可以应用到所有事物中去, 上到函数声明语法 下到缩进 注释风格 .
    
    也可去看缩进风格 Indentstyle: http://www.catb.org/~esr/jargon/html/I/indent-style.html

3. **慷慨的空格和括号**

    变量的声明和语句块之间留一个空行, 同样在代码逻辑语句块之间也要有空行. 
    
    两个函数之间留一个空行,两个更好 :
    ```code
    if (foo) {
        bar;
    }

    to:

    if(foo)bar;
    ```
    
4. **使用tab制表符进行缩进,一个tab占用四个空格**

5. **预处理语句(例如 #if)必须写在第一列,如果要缩进预处理语句也要把 # 号放在一行的开始, 紧接着是任意数量的空格.**

---

## 测试

1. **PHP扩展应该用*.phpt 来测试. 请阅读 README.TESTING.**

---

## 文档和可折叠钩子(Documentation and Folding Hooks)

为了确保在线文档与代码保持一致 , 每个用户级函数在有它的用户级函数原型之前,应该有一行简单的对函数的描述 .

它看起来应该是这样的:
```c
/* {{{ proto int abs(int number)
 Returns the absolute value of the number */
PHP_FUNCTION(abs)
{
 ...
}
/* }}} */
```

在Emacs 和 vim 中 符号 {{{ 在折叠模式下 默认是折叠符号 (set fdm=marker) .
    
在处理大文件的时候折叠是非常有用的, 因为你能快速滚动文件并展开你希望工作的功能 , 符号 }}} 在每个函数的结尾标志着折叠的结束, 并且应该在单独的一行 .

那里的"原型(proto)"关键字只是辅助doc/genfuncsummary脚本生成所有函数摘要的 , 该关键字在函数原型的前面允许我们放在折叠块里,不影响函数摘要 .

可选参数像下面这样写:
```c
/* {{{ proto object imap_header(int stream_id, int msg_no [, int from_length [, int subject_length [, string default_host]]])
Returns a header object with the defined parameters */
```
没错, 请保持原型在单独的一行, 即使这行可能会很长.

---

## 新的或实验性的函数

在首次公开一套新的函数集实现时 , 为了减少相关问题有人建议在这个函数的目录包含一个有“实验”标签的文件 , 并且这个函数在最初实现的时候要遵循下面的标准前缀约定 .

EXPERIMENTAL文件应包括以下信息:
* 任何编辑信息(已知的bugs, 模块的未来方向)
* 不适合用git comments的当前的状态记录

一般新特性应该提交到PECL或实验分支,除非有特殊原因会直接添加到核心发布

---

## 别名 & 遗留文档(Aliases & Legacy Documentation)

对同一个名称，你也可能有一些弃用的别名, 例如 somedb_select_result and somedb_selectresult

文档只会记录最新的名字, 别名会记录在父函数的文档列表中.

为了便于参考，对于功能相同的名称不同的用户函数（如highlight_file和show_source），应分别描述说明。

描述函数别名的原型仍应包括在内 , 只要这部分代码能够被维护, 功能和名称应该尽可能保持向后兼容

---

---
# **原文::**
---

---

```code
========================
  PHP Coding Standards
========================

This file lists several standards that any programmer adding or changing
code in PHP should follow.  Since this file was added at a very late
stage of the development of PHP v3.0, the code base does not (yet) fully
follow it, but it's going in that general direction.  Since we are now
well into version 5 releases, many sections have been recoded to use
these rules.

Code Implementation
-------------------

0.  Document your code in source files and the manual. [tm]

1.  Functions that are given pointers to resources should not free them

For instance, ``function int mail(char *to, char *from)`` should NOT free
to and/or from.

Exceptions:

- The function's designated behavior is freeing that resource.  E.g. efree()

- The function is given a boolean argument, that controls whether or not
  the function may free its arguments (if true - the function must free its
  arguments, if false - it must not)

- Low-level parser routines, that are tightly integrated with the token
  cache and the bison code for minimum memory copying overhead.

2.  Functions that are tightly integrated with other functions within the
    same module, and rely on each other non-trivial behavior, should be
    documented as such and declared 'static'.  They should be avoided if
    possible.

3.  Use definitions and macros whenever possible, so that constants have
    meaningful names and can be easily manipulated.  The only exceptions
    to this rule are 0 and 1, when used as false and true (respectively).
    Any other use of a numeric constant to specify different behavior
    or actions should be done through a #define.

4.  When writing functions that deal with strings, be sure to remember
    that PHP holds the length property of each string, and that it
    shouldn't be calculated with strlen().  Write your functions in such
    a way so that they'll take advantage of the length property, both
    for efficiency and in order for them to be binary-safe.
    Functions that change strings and obtain their new lengths while
    doing so, should return that new length, so it doesn't have to be
    recalculated with strlen() (e.g. php_addslashes())

5.  NEVER USE strncat().  If you're absolutely sure you know what you're doing,
    check its man page again, and only then, consider using it, and even then,
    try avoiding it.

6.  Use ``PHP_*`` macros in the PHP source, and ``ZEND_*`` macros in the Zend
    part of the source. Although the ``PHP_*`` macro's are mostly aliased to the
    ``ZEND_*`` macros it gives a better understanding on what kind of macro
    you're calling.

7.  When commenting out code using a #if statement, do NOT use 0 only. Instead
    use "<git username here>_0". For example, #if FOO_0, where FOO is your
    git user foo.  This allows easier tracking of why code was commented out,
    especially in bundled libraries.

8.  Do not define functions that are not available.  For instance, if a
    library is missing a function, do not define the PHP version of the
    function, and do not raise a run-time error about the function not
    existing.  End users should use function_exists() to test for the
    existence of a function

9.  Prefer emalloc(), efree(), estrdup(), etc. to their standard C library
    counterparts.  These functions implement an internal "safety-net"
    mechanism that ensures the deallocation of any unfreed memory at the
    end of a request.  They also provide useful allocation and overflow
    information while running in debug mode.

    In almost all cases, memory returned to the engine must be allocated
    using emalloc().

    The use of malloc() should be limited to cases where a third-party
    library may need to control or free the memory, or when the memory in
    question needs to survive between multiple requests.

User Functions/Methods Naming Conventions
------------------

1.  Function names for user-level functions should be enclosed with in
    the PHP_FUNCTION() macro. They should be in lowercase, with words
    underscore delimited, with care taken to minimize the letter count.
    Abbreviations should not be used when they greatly decrease the
    readability of the function name itself::

    Good:
    'mcrypt_enc_self_test'
    'mysql_list_fields'

    Ok:
    'mcrypt_module_get_algo_supported_key_sizes'
    (could be 'mcrypt_mod_get_algo_sup_key_sizes'?)
    'get_html_translation_table'
    (could be 'html_get_trans_table'?)

    Bad:
    'hw_GetObjectByQueryCollObj'
    'pg_setclientencoding'
    'jf_n_s_i'

2.  If they are part of a "parent set" of functions, that parent should
    be included in the user function name, and should be clearly related
    to the parent program or function family. This should be in the form
    of ``parent_*``::

    A family of 'foo' functions, for example:
    
    Good:
    'foo_select_bar'
    'foo_insert_baz'
    'foo_delete_baz'

    Bad:
    'fooselect_bar'
    'fooinsertbaz'
    'delete_foo_baz'

3.  Function names used by user functions should be prefixed
    with ``_php_``, and followed by a word or an underscore-delimited list of
    words, in lowercase letters, that describes the function.  If applicable,
    they should be declared 'static'.

4.  Variable names must be meaningful.  One letter variable names must be
    avoided, except for places where the variable has no real meaning or
    a trivial meaning (e.g. for (i=0; i<100; i++) ...).

5.  Variable names should be in lowercase.  Use underscores to separate
    between words.

6.  Method names follow the 'studlyCaps' (also referred to as 'bumpy case'
    or 'camel caps') naming convention, with care taken to minimize the
    letter count. The initial letter of the name is lowercase, and each
    letter that starts a new 'word' is capitalized::

    Good:
    'connect()'
    'getData()'
    'buildSomeWidget()'

    Bad:
    'get_Data()'
    'buildsomewidget'
    'getI()'

7.  Classes should be given descriptive names. Avoid using abbreviations where
    possible. Each word in the class name should start with a capital letter,
    without underscore delimiters (CamelCaps starting with a capital letter).
    The class name should be prefixed with the name of the 'parent set' (e.g.
    the name of the extension)::

    Good:
    'Curl'
    'FooBar'

    Bad:
    'foobar'
    'foo_bar'

Internal Function Naming Conventions
----------------------

1.  Functions that are part of the external API should be named
    'php_modulename_function()' to avoid symbol collision. They should be in
    lowercase, with words underscore delimited. Exposed API must be defined
    in 'php_modulename.h'.

    PHPAPI char *php_session_create_id(PS_CREATE_SID_ARGS);

    Unexposed module function should be static and should not be defined in
    'php_modulename.h'.

    static int php_session_destroy()

2.  Main module source file must be named 'modulename.c'.

3.  Header file that is used by other sources must be named 'php_modulename.h'.


Syntax and indentation
----------------------

1.  Never use C++ style comments (i.e. // comment).  Always use C-style
    comments instead.  PHP is written in C, and is aimed at compiling
    under any ANSI-C compliant compiler.  Even though many compilers
    accept C++-style comments in C code, you have to ensure that your
    code would compile with other compilers as well.
    The only exception to this rule is code that is Win32-specific,
    because the Win32 port is MS-Visual C++ specific, and this compiler
    is known to accept C++-style comments in C code.

2.  Use K&R-style.  Of course, we can't and don't want to
    force anybody to use a style he or she is not used to, but,
    at the very least, when you write code that goes into the core
    of PHP or one of its standard modules, please maintain the K&R
    style.  This applies to just about everything, starting with
    indentation and comment styles and up to function declaration
    syntax. Also see Indentstyle.

    Indentstyle: http://www.catb.org/~esr/jargon/html/I/indent-style.html

3.  Be generous with whitespace and braces.  Keep one empty line between the
    variable declaration section and the statements in a block, as well as
    between logical statement groups in a block.  Maintain at least one empty
    line between two functions, preferably two.  Always prefer::

    if (foo) {
        bar;
    }

    to:

    if(foo)bar;

4.  When indenting, use the tab character.  A tab is expected to represent
    four spaces.  It is important to maintain consistency in indenture so
    that definitions, comments, and control structures line up correctly.

5.  Preprocessor statements (#if and such) MUST start at column one. To
    indent preprocessor directives you should put the # at the beginning
    of a line, followed by any number of whitespace.

Testing
-------

1.  Extensions should be well tested using *.phpt tests. Read about that
    in README.TESTING.

Documentation and Folding Hooks
-------------------------------

In order to make sure that the online documentation stays in line with
the code, each user-level function should have its user-level function
prototype before it along with a brief one-line description of what the
function does.  It would look like this::

  /* {{{ proto int abs(int number)
     Returns the absolute value of the number */
  PHP_FUNCTION(abs)
  {
     ...
  }
  /* }}} */

The {{{ symbols are the default folding symbols for the folding mode in
Emacs and vim (set fdm=marker).  Folding is very useful when dealing with
large files because you can scroll through the file quickly and just unfold
the function you wish to work on.  The }}} at the end of each function marks
the end of the fold, and should be on a separate line.

The "proto" keyword there is just a helper for the doc/genfuncsummary script
which generates a full function summary.  Having this keyword in front of the
function prototypes allows us to put folds elsewhere in the code without
messing up the function summary.

Optional arguments are written like this::

  /* {{{ proto object imap_header(int stream_id, int msg_no [, int from_length [, int subject_length [, string default_host]]])
     Returns a header object with the defined parameters */

And yes, please keep the prototype on a single line, even if that line
is massive.

New and Experimental Functions
-----------------------------------
To reduce the problems normally associated with the first public
implementation of a new set of functions, it has been suggested
that the first implementation include a file labeled 'EXPERIMENTAL'
in the function directory, and that the functions follow the
standard prefixing conventions during their initial implementation.

The file labelled 'EXPERIMENTAL' should include the following
information::

  Any authoring information (known bugs, future directions of the module).
  Ongoing status notes which may not be appropriate for Git comments.

In general new features should go to PECL or experimental branches until
there are specific reasons for directly adding it to the core distribution.

Aliases & Legacy Documentation
-----------------------------------
You may also have some deprecated aliases with close to duplicate
names, for example, somedb_select_result and somedb_selectresult. For
documentation purposes, these will only be documented by the most
current name, with the aliases listed in the documentation for
the parent function. For ease of reference, user-functions with
completely different names, that alias to the same function (such as
highlight_file and show_source), will be separately documented. The
proto should still be included, describing which function is aliased.

Backwards compatible functions and names should be maintained as long
as the code can be reasonably be kept as part of the codebase. See
/phpdoc/README for more information on documentation.
```



