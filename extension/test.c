#include "php.h"

PHP_FUNCTION(test);

PHP_FUNCTION(multiply);

ZEND_BEGIN_ARG_INFO_EX(arginfo_test, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_multiply, 0, 0, 0)
ZEND_END_ARG_INFO()

const zend_function_entry test_functions[] = {
	PHP_FE(test, arginfo_test)
		PHP_FE(multiply, arginfo_multiply)
			PHP_FE_END};

zend_module_entry test_module_entry = {
	STANDARD_MODULE_HEADER, // #if ZEND_MODULE_API_NO >= 20010901
	"test",					// название модуля
	test_functions,			// указываем экспортируемые функции
	NULL,					// PHP_MINIT(test), Module Initialization
	NULL,					// PHP_MSHUTDOWN(test), Module Shutdown
	NULL,					// PHP_RINIT(test), Request Initialization
	NULL,					// PHP_RSHUTDOWN(test), Request Shutdown
	NULL,					// PHP_MINFO(test), Module Info (для phpinfo())
	"0.1",					// версия нашего модуля
	STANDARD_MODULE_PROPERTIES};

ZEND_GET_MODULE(test)

PHP_FUNCTION(test)
{
	RETURN_STRING("hello extension");
}

PHP_FUNCTION(multiply)
{
	zend_long num;
	zval *array;
	int i, num_varargs;
	zval *varargs = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &varargs, &num_varargs) == FAILURE)
	{
		return;
	}
	double sum = 0;
	for(int i =0; i < num_varargs;i++) {
		if(varargs[i].u1.type_info == IS_DOUBLE ) {
			sum += varargs[i].value.dval;
		}
		if(varargs[i].u1.type_info == IS_LONG) {
			sum+= varargs[i].value.lval;
		}
	}
	RETURN_DOUBLE(sum);
}