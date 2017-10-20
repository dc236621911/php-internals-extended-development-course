# 介绍


PHP是弱类型语言,在PHP代码中声明或使用变量的时候,并不需要强制指明其数据类型.在PHP内核中定义的变量类型:
```c
/* Zend/zend_type.h : line 304 */
#define IS_UNDEF					0
#define IS_NULL						1
...
#define IS_PTR						17
#define _IS_ERROR					20
```




## zval
PHP底层设计了一个zval(“Zend value”的缩写)的数据结构,可以用来表示任意类型的PHP值,因此,它可能是所有PHP中最重要的结构.本节介绍zvals及其使用背后的基本概念.

```c
/* Zend/zend_type.h */
typedef union _zend_value {
...
} zend_value;
struct _zval_struct {
...
};
```
虽然看起来变得好大, 但其实你仔细看, 全部都是联合体, 这个新的zval在64位环境下,现在只需要16个字节(2个指针size), 它主要分为俩个部分, value和扩充字段, 而扩充字段又分为u1和u2俩个部分, 其中u1是type info, u2是各种辅助字.
其中value部分, 是一个size_t大小(一个指针大小), 可以保存一个指针, 或者一个long, 或者一个double.
而type info部分则保存了这个zval的类型. 扩充辅助字段则会在多个其他地方使用, 比如next, 就用在取代Hashtable中原来的拉链指针, 这部分会在以后介绍HashTable的时候再来详解.

![zval的内存分布](./image/1-3zval.png)

从PHP7开始, 对于在zval的value字段中能保存下的值, 就不再对他们进行引用计数了, 而是在拷贝的时候直接赋值, 这样就省掉了大量的引用计数相关的操作, 这部分类型有:
    IS_LONG
    IS_DOUBLE
当然对于那种根本没有值, 只有类型的类型, 也不需要引用计数了:
    IS_NULL
    IS_FALSE
    IS_TRUE
而对于复杂类型, 一个size_t保存不下的, 那么我们就用value来保存一个指针, 这个指针指向这个具体的值, 引用计数也随之作用于这个值上, 而不在是作用于zval上了.


所以, 在PHP7开始, 我们移除了MAKE_STD_ZVAL/ALLOC_ZVAL宏, 不再支持存堆内存上申请zval. 函数内部使用的zval要么来自外面输入, 要么使用在栈上分配的临时zval.

### 操作zval

对zval的操作定义了很多宏,这维持了一个抽象层次,使意图更清晰,将来的PHP版本zval的内部改变了还可以兼容.
```c
//不正确
zval zv = ...
if (zv_ptr->type == IS_LONG) {
    php_printf("此变量为长整形,值为 %ld\n", zv->value.lval);
} else ...

//推荐使用宏操作,
zval  zv  = ...
if  (Z_TYPE zv ) ==  IS_LONG ) { 
    php_printf (“此变量为长整形,值为 ％ld \ n ” , Z_LVAL_P zv )); 
}  else  ...

//宏有_P后缀表示操作的是zval指针
Z_TYPE(zv);                 // 获取zval类型 = zv.type
Z_TYPE_P(zv_ptr);           // 获取指针指向的zval类型= zv_ptr->type

```

#### zval.value 相关宏
```code
    No. 名称                 变量名       含义                        访问宏
    1   zend_long           lval        整数(integer)                Z_LVAL(zval)
    2   double              dval        浮動小数点数(float/double)    Z_DVAL(zval)
    3   zend_refcounted     *counted    引用计数                      Z_COUNTED(zval)
    4   zend_string         *str        字符串                       Z_STR(zval)
    5   zend_array          *arr        数组                         Z_ARR(zval)
    6   zend_object         *obj        对象                         Z_OBJ(zval)
    7   zend_resource       *res        资源                         Z_RES(zval)
    8   zend_reference      *ref        引用                         Z_REF(zval)
    9   zend_ast_ref        *ast        抽象構文木                    Z_AST(zval)
    10  zval                *zv         zval指针                     Z_INDIRECT(zval)
    11  void                *ptr        任意类型指针                  Z_PTR(zval)
    12  zend_class_entry    *ce         类                          Z_CE(zval)
    13  zend_function       *func       函数                         Z_FUNC(zval)
```
#### zval 相关宏
```code
    ZVAL_UNDEF(z): 表示zval被销毁
    ZVAL_NULL(z): 设置为NULL
    ZVAL_FALSE(z): 设置为false
    ZVAL_TRUE(z): 设置为true
    ZVAL_BOOL(z, b): 设置为布尔型,b为IS_TRUE、IS_FALSE,与上面两个等价
    ZVAL_LONG(z, l): 设置为整形,l类型为zend_long,如:zval z; ZVAL_LONG(&z, 88);
    ZVAL_DOUBLE(z, d): 设置为浮点型,d类型为double
    ZVAL_STR(z, s): 设置字符串,将z的value设置为s,s类型为zend_string*,不会增加s的refcount,支持interned strings
    ZVAL_NEW_STR(z, s): 同ZVAL_STR(z, s),s为普通字符串,不支持interned strings
    ZVAL_STR_COPY(z, s): 将s拷贝到z的value,s类型为zend_string*,同ZVAL_STR(z, s),这里会增加s的refcount
    ZVAL_ARR(z, a): 设置为数组,a类型为zend_array*
    ZVAL_NEW_ARR(z): 新分配一个数组,主动分配一个zend_array
    ZVAL_NEW_PERSISTENT_ARR(z): 创建持久化数组,通过malloc分配,需要手动释放
    ZVAL_OBJ(z, o): 设置为对象,o类型为zend_object*
    ZVAL_RES(z, r): 设置为资源,r类型为zend_resource*
    ZVAL_NEW_RES(z, h, p, t): 新创建一个资源,h为资源handle,t为type,p为资源ptr指向结构
    ZVAL_REF(z, r): 设置为引用,r类型为zend_reference*
    ZVAL_NEW_EMPTY_REF(z): 新创建一个空引用,没有设置具体引用的value
    ZVAL_NEW_REF(z, r): 新创建一个引用,r为引用的值,类型为zval*
```
### 在代码中查看zval





## 参考资料:
    https://github.com/pangudashu/php7-internal/blob/master/7/var.md
    https://net-newbie.com/phpext/7-zval.html
    https://github.com/laruence/php7-internal/blob/master/zval.md



