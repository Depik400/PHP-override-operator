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

static int op_handler(zend_execute_data *execute_data)
{
	// printf("hello here");
	zend_op *opline = execute_data->opline;
	zval *op1 = EX_VAR(opline->result.var);
	ZVAL_LONG(op1, 5);
	execute_data->opline++;
	return ZEND_USER_OPCODE_CONTINUE;
}
static PHP_MINIT_FUNCTION(test)
{
	zend_set_user_opcode_handler(ZEND_ADD, op_handler);
	zend_set_user_opcode_handler(ZEND_SUB, op_handler);
	zend_set_user_opcode_handler(3, op_handler);
	zend_set_user_opcode_handler(4, op_handler);
	// printf("hello world");
}

zend_module_entry test_module_entry = {
	STANDARD_MODULE_HEADER, // #if ZEND_MODULE_API_NO >= 20010901
	"test",					// название модуля
	test_functions,			// указываем экспортируемые функции
	PHP_MINIT(test),		// PHP_MINIT(test), Module Initialization
	NULL,					// PHP_MSHUTDOWN(test), Module Shutdown
	NULL,					// PHP_RINIT(test), Request Initialization
	NULL,					// PHP_RSHUTDOWN(test), Request Shutdown
	NULL,					// PHP_MINFO(test), Module Info (для phpinfo())
	"0.3",					// версия нашего модуля
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
	for (int i = 0; i < num_varargs; i++)
	{
		if (varargs[i].u1.type_info == IS_DOUBLE)
		{
			sum += varargs[i].value.dval;
		}
		if (varargs[i].u1.type_info == IS_LONG)
		{
			sum += varargs[i].value.lval;
		}
		if (varargs[i].u1.type_info == IS_STRING)
		{
			sum += 10;
		}
	}
	RETURN_DOUBLE(sum);
}