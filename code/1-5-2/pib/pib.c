/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_pib.h"

#define TEMP_CONVERTER_TO_FAHRENHEIT 2
#define TEMP_CONVERTER_TO_CELSIUS 1

static int le_pib;

/* {{{ arginfo */
/* 华氏转摄氏函数参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fahrenheit_to_celsius, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, fahrenheit, IS_DOUBLE, 0)
ZEND_END_ARG_INFO();
/* 摄氏转华氏函数参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_celsius_to_fahrenheit, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, celsius, IS_DOUBLE, 0)
ZEND_END_ARG_INFO();
/* 双向转函数参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_temperature_converter, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, temperature, IS_DOUBLE, 0)
    ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO();
/* 数组批量华氏转摄氏函数参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_multiple_fahrenheit_to_celsius, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, temperatures, 0)
ZEND_END_ARG_INFO();
/* }}} */

static double php_fahrenheit_to_celsius(double f)
{
    return ((double)5/9) * (double)(f - 32);
}
static double php_celsius_to_fahrenheit(double c)
{
    return (((double)9/5) * c) + 32 ;
}


/* {{{ proto double fahrenheit_to_celsius(double f)
    */
PHP_FUNCTION(fahrenheit_to_celsius)
{
	int argc = ZEND_NUM_ARGS();
	double f;

	if (zend_parse_parameters(argc, "d", &f) == FAILURE) 
		return;
	RETURN_DOUBLE(php_fahrenheit_to_celsius(f));
}
/* }}} */

/* {{{ proto double celsius_to_fahrenheit(double c)
    */
PHP_FUNCTION(celsius_to_fahrenheit)
{
	int argc = ZEND_NUM_ARGS();
	double c;

	if (zend_parse_parameters(argc, "d", &c) == FAILURE) 
		return;
	RETURN_DOUBLE(php_celsius_to_fahrenheit(c));
}
/* }}} */

/* {{{ proto string temperature_converter(double t, long mode)
    */
PHP_FUNCTION(temperature_converter)
{
	int argc = ZEND_NUM_ARGS();
	double t;
	zend_long mode = TEMP_CONVERTER_TO_CELSIUS;
	zend_string *result;

	/* 接收参数时|l表示此参数为long类型,并且为可选参数 */
	if (zend_parse_parameters(argc, "d|l", &t, &mode) == FAILURE)
		return;

	switch (mode)
	{
		case TEMP_CONVERTER_TO_CELSIUS:
			/* strpprintf用来构建一个zend_string */
			result = strpprintf(0, "华氏 %.2f 度, 摄氏 %.2f 度", t, php_fahrenheit_to_celsius(t));
			RETURN_STR(result);
		case TEMP_CONVERTER_TO_FAHRENHEIT:
			result = strpprintf(0, "摄氏 %.2f 度, 华氏 %.2f 度", t, php_celsius_to_fahrenheit(t));
			RETURN_STR(result);
		default:
			php_error(E_WARNING, "转换模式参数错误, 可接受的转换模式值为 1 或 2");
	}
}
/* }}} */

/* {{{ proto array multiple_fahrenheit_to_celsius(array temperatures)
    */
PHP_FUNCTION(multiple_fahrenheit_to_celsius)
{
	int argc = ZEND_NUM_ARGS();
	HashTable *temperatures;
	zval *data;

	if (zend_parse_parameters(argc, "h", &temperatures) == FAILURE)
		return;

    if (zend_hash_num_elements(temperatures) == 0) {
        return;
    }
    array_init_size(return_value, zend_hash_num_elements(temperatures));
    ZEND_HASH_FOREACH_VAL(temperatures, data)
        zval dup;
        ZVAL_COPY_VALUE(&dup, data);
        convert_to_double(&dup);
        add_next_index_double(return_value, php_fahrenheit_to_celsius(Z_DVAL(dup)));
    ZEND_HASH_FOREACH_END();

}
/* }}} */




PHP_MINIT_FUNCTION(pib)
{
    /* 根据要注册的常量类型,使用REGISTER_XXX_CONSTANT(),API和宏位于 Zend/zend_constants.h中. */
    REGISTER_LONG_CONSTANT("TEMP_CONVERTER_TO_CELSIUS",TEMP_CONVERTER_TO_CELSIUS, CONST_CS|CONST_PERSISTENT);
    /* CONST_CS--区分大小写的常量,CONST_PERSISTENT--跨请求的持久常量 */
    REGISTER_LONG_CONSTANT("TEMP_CONVERTER_TO_FAHRENHEIT",TEMP_CONVERTER_TO_FAHRENHEIT,CONST_CS|CONST_PERSISTENT);
    return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(pib)
{
	return SUCCESS;
}



PHP_RINIT_FUNCTION(pib)
{
#if defined(COMPILE_DL_PIB) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}



PHP_RSHUTDOWN_FUNCTION(pib)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(pib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pib support", "enabled");
	php_info_print_table_end();

}


const zend_function_entry pib_functions[] = {
    PHP_FE(fahrenheit_to_celsius, arginfo_fahrenheit_to_celsius)
    PHP_FE(celsius_to_fahrenheit,arginfo_celsius_to_fahrenheit)
    PHP_FE(temperature_converter, arginfo_temperature_converter)
    PHP_FE(multiple_fahrenheit_to_celsius, arginfo_multiple_fahrenheit_to_celsius)
    PHP_FE_END
};


zend_module_entry pib_module_entry = {
	STANDARD_MODULE_HEADER,
	"pib",
	pib_functions,
	PHP_MINIT(pib),
	PHP_MSHUTDOWN(pib),
	PHP_RINIT(pib),	
	PHP_RSHUTDOWN(pib),
	PHP_MINFO(pib),
	PHP_PIB_VERSION,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_PIB
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pib)
#endif

