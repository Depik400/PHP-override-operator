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

zend_string *operator_method_name(zend_uchar opcode)
{
	switch (opcode)
	{
	case ZEND_ADD:
		return zend_string_init("__add", strlen("__add"), 1);

	default:
		return zend_string_init("__any_action", strlen("__any_action"), 1);
	}
}

zend_bool operator_get_method(zend_string *method, zval *obj,
							  zend_fcall_info *fci,
							  zend_fcall_info_cache *fcc)
{
	memset(fci, 0, sizeof(zend_fcall_info));
	fci->size = sizeof(zend_fcall_info);
	fci->object = Z_OBJ_P(obj);
	ZVAL_STR(&(fci->function_name), method);

	if (!zend_is_callable_ex(&(fci->function_name), fci->object,
							 IS_CALLABLE_CHECK_SILENT,
							 NULL, fcc, NULL))
	{
		return 0;
	}
	/* Disallow dispatch via __call */
	if (fcc->function_handler == Z_OBJCE_P(obj)->__call)
	{
		return 0;
	}
	if (fcc->function_handler->type == ZEND_USER_FUNCTION)
	{
		zend_op_array *oparray = (zend_op_array *)(fcc->function_handler);
		if (oparray->fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)
		{
			return 0;
		}
	}

	return 1;
}
static int op_handler(zend_execute_data *execute_data)
{
	// printf("hello here");
	zend_op *opline = execute_data->opline;
	zval *op1 = EX_VAR(opline->result.var);
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	// zend_string *method = operator_method_name(opline->opcode);
	ZVAL_LONG(op1, 5);
	execute_data->opline++;
	return ZEND_USER_OPCODE_CONTINUE;
}

static int op_handler_object(zend_execute_data *execute_data)
{
	// printf("hello here");
	zend_op *opline = execute_data->opline;
	zval *op1 = EX_VAR(opline->op1.var);
	zval *op2 = EX_VAR(opline->op2.var);
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_string *method = operator_method_name(opline->opcode);
	if (Z_TYPE_P(op1) != IS_OBJECT || !operator_get_method(method, op1, &fci, &fcc))
		return ZEND_USER_OPCODE_DISPATCH;

	fci.retval = EX_VAR(opline->result.var);
	fci.params = op2;
	fci.param_count = op2 ? 1 : 0;
	if (FAILURE == zend_call_function(&fci, &fcc))
	{
		php_error(
			E_WARNING,
			"Failed calling %s::%s()",
			Z_OBJCE_P(op1)->name,
			Z_STRVAL(fci.function_name));
		ZVAL_NULL(fci.retval);
	}
	EX(opline) = opline + 1;
	return ZEND_USER_OPCODE_CONTINUE;
}

static PHP_MINIT_FUNCTION(test)
{
	zend_set_user_opcode_handler(ZEND_ADD, op_handler_object);
	// zend_set_user_opcode_handler(ZEND_SUB, op_handler);
	//  zend_set_user_opcode_handler(3, op_handler);
	//  zend_set_user_opcode_handler(4, op_handler);
	//  printf("hello world");
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