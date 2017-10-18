## 介绍

在PHP中,函数分为俩种:
一种是zend_internal_function, 这种函数是由扩展或者Zend/PHP内核提供的,用’C/C++’编写的,可以直接执行的函数.
另外一种是zend_user_function, 这种函数呢,就是我们经常在见的,用户在PHP脚本中定义的函数,这种函数最终会被ZE翻译成opcode array来执行

zval : zend_function func 类型成员
EG(function_table)是一个哈希表,记录的就是PHP中所有的函数.

zend_internal_function,zend_function,zend_op_array这三种结构在一定程序上存在公共的元素, 于是这些元素以联合体的形式共享内存,并且在执行过程中对于一个函数,这三种结构对应的字段在值上都是一样的, 于是可以在一些结构间发生完美的强制类型转换.zend_op_array与zend_internal_function结构的起始位置都有common中的几个成员,common可以看作是op_array、internal_function的header,不管是什么哪种函数都可以通过zend_function.common.xx快速访问,zend_function可以与zend_op_array互换,zend_function可以与zend_internal_function互换,但是一个zend_op_array结构转换成zend_function是不能再次转变成zend_internal_function结构的,反之亦然.


### 结构
```c
//zend_compile.h
union _zend_function {
	zend_uchar type;	/* 函数类型 */
	uint32_t   quick_arg_flags;
	struct {
		zend_uchar type;  /* never used */
		zend_uchar arg_flags[3];        /* bitset of arg_info.pass_by_reference */
		uint32_t fn_flags;              //作为方法时的访问类型等,如ZEND_ACC_STATIC等 
		zend_string *function_name;     //函数名称
		zend_class_entry *scope;        //成员方法所属类,面向对象实现中用到
		union _zend_function *prototype;//函数原型
		uint32_t num_args;              //参数数量
		uint32_t required_num_args;     //必传参数数量
		zend_arg_info *arg_info;        //参数信息
	} common;
	zend_op_array op_array;             //函数实际编译为普通的zend_op_array
	zend_internal_function internal_function;
};
//内部函数结构
typedef struct _zend_internal_function {
    /* Common elements */
    zend_uchar type;
    zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
    uint32_t fn_flags;
    zend_string* function_name;
    zend_class_entry *scope;
    zend_function *prototype;
    uint32_t num_args;
    uint32_t required_num_args;
    zend_internal_arg_info *arg_info;
    /* END of common elements */

    void (*handler)(INTERNAL_FUNCTION_PARAMETERS); //函数指针,展开:void (*handler)(zend_execute_data *execute_data, zval *return_value)
    struct _zend_module_entry *module;
    void *reserved[ZEND_MAX_RESERVED_RESOURCES];
} zend_internal_function;
//用户自定义函数结构
struct _zend_op_array {
    /* Common elements common是普通函数或类成员方法对应的opcodes快速访问时使用的字段*/
	zend_uchar type;
	zend_uchar arg_flags[3]; /* bitset of arg_info.pass_by_reference */
	uint32_t fn_flags;
	zend_string *function_name;
	zend_class_entry *scope;
	zend_function *prototype;
	uint32_t num_args;
	uint32_t required_num_args;
	zend_arg_info *arg_info;
	/* END of common elements */

	uint32_t *refcount;

	uint32_t last;      
    zend_op *opcodes;   //opcode指令数组   
    int last_var;       //PHP代码里定义的变量数:op_type为IS_CV的变量,不含IS_TMP_VAR、IS_VAR的,编译前0,然后发现一个新变量这个值就加1
    uint32_t T;         //临时变量数:op_type为IS_TMP_VAR、IS_VAR的变量    
    zend_string **vars; //这个数组在ast编译期间配合last_var用来确定各个变量的编号,非常重要的一步操作//PHP变量名数组
    ...
    HashTable *static_variables;    //静态变量符号表:通过static声明的
    ...
    int last_literal;   //字面量数量    
    zval *literals;     //字面量(常量)数组,这些都是在PHP代码定义的一些值
    int  cache_size;    //运行时缓存数组大小    
    void **run_time_cache;  //运行时缓存,主要用于缓存一些znode_op以便于快速获取数据,后面单独介绍这个机制

    void *reserved[ZEND_MAX_RESERVED_RESOURCES];
};
//函数类型
#define ZEND_INTERNAL_FUNCTION              1   //内置的函数
#define ZEND_USER_FUNCTION                  2   //用户函数
#define ZEND_OVERLOADED_FUNCTION            3   //对象中__call相关
#define ZEND_EVAL_CODE                      4   //eval code
#define ZEND_OVERLOADED_FUNCTION_TEMPORARY  5   //对象中__call相关

```





## 用户自定义函数

PHP在编译阶段将用户自定义的函数编译为独立的opcodes,保存在EG(function_table)中,调用时重新分配新的zend_execute_data(相当于运行栈),然后执行函数的opcodes,调用完再还原到旧的zend_execute_data,继续执行,关于zend引擎execute阶段后面会详细分析.
zend_function的结构中的op_array存储了该函数中所有的操作,当函数被调用时,ZE就会将这个op_array中的opline一条条顺次执行, 并将最后的返回值返回. 从VLD扩展中查看的关于函数的信息可以看出,函数的定义和执行是分开的,一个函数可以作为一个独立的运行单元而存在.


### 函数参数
参数名称也在zend_op_array.vars中,编号首先是从参数开始的,所以按照参数顺序其编号依次为0、1、2...
参数的其它信息通过zend_arg_info结构记录:
```c
typedef struct _zend_arg_info {
    zend_string *name;      //参数名
    zend_string *class_name;//类名
    zend_uchar type_hint;   //显式声明的参数类型,比如(array $param_1)
    zend_uchar pass_by_reference;   //是否引用传参,参数前加&的这个值就是1
    zend_bool allow_null;   //是否允许为NULL,注意:这个值并不是用来表示参数是否为必传的
    zend_bool is_variadic;  //是否为可变参数,即...用法,与golang的用法相同,5.6以上新增的一个用法:function my_func($a, ...$b){...}
} zend_arg_info;
```
每个参数都有一个上面的结构,所有参数的结构保存在zend_op_array.arg_info数组中,这里有一个地方需要注意:zend_op_array->arg_info数组保存的并不全是输入参数,如果函数声明了返回值类型则也会为它创建一个zend_arg_info,这个结构在arg_info数组的第一个位置,这种情况下zend_op_array->arg_info指向的实际是数组的第二个位置,返回值的结构通过zend_op_array->arg_info[-1]读取.

### 编译过程
zend引擎编译课程再讲解


## 内部函数

内部函数的定义非常简单,我们只需要创建一个普通的C函数,然后创建一个zend_internal_function结构添加到 EG(function_table)

### 解析函数参数
ZEND_API int zend_parse_parameters(int num_args TSRMLS_DC, char *type_spec, ...)
    第一个参数num_args表明表示想要接收的参数个数,我们经常使用ZEND_NUM_ARGS() 来表示对传入的参数“有多少要多少”.
    第二参数应该总是宏 TSRMLS_CC .
    第三个参数 type_spec 是一个字符串,用来指定我们所期待接收的各个参数的类型,有点类似于 printf 中指定输出格式的那个格式化字符串.
    剩下的参数就是我们用来接收PHP参数值的变量的指针.


### PHP自带函数讲解
```c
// ext/standard/string.c : line 2744
static void php_ucfirst(char *str)
{
	register char *r;
	r = str;
	*r = toupper((unsigned char) *r);
}
PHP_FUNCTION(ucfirst)
{
	zend_string *str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(str)
	ZEND_PARSE_PARAMETERS_END();

	if (!ZSTR_LEN(str)) {
		RETURN_EMPTY_STRING();
	}

	ZVAL_STRINGL(return_value, ZSTR_VAL(str), ZSTR_LEN(str));
	php_ucfirst(Z_STRVAL_P(return_value));
}
// ext/standard/php_string.h
PHP_FUNCTION(ucfirst);
```

### 注册新函数
在扩展课程再讲解

### 函数注册过程

## 测试
```php
<?php
/*
//列出所有函数
function test(){
	print_r(get_defined_functions());
}
test();
*/
/* 调用用户定义函数
function test(){}
test();
 */
/* 调用内部函数 */
strtoupper('test');
```

# 函数
## 分类
## zval function
## 结构
# 内部函数
## 扩展结构
# opcode
# 编译过程示例


## 参考资料:
    http://www.laruence.com/
    https://github.com/pangudashu/php7-internal/
    http://www.php-internals.com/book



