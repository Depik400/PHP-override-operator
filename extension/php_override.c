#include "php.h"

PHP_FUNCTION(php_override);

PHP_FUNCTION(multiply);

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_override, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_multiply, 0, 0, 0)
ZEND_END_ARG_INFO()
const zend_function_entry php_override_functions[] =
	{
		PHP_FE(php_override, arginfo_php_override)
			PHP_FE(multiply, arginfo_multiply)
				PHP_FE_END};

static zval *get_zval_ptr_undef(zend_uchar op_type, znode_op op, zend_execute_data *execute_data)
{
	switch (op_type)
	{
	case IS_TMP_VAR:
	case IS_VAR:
	case IS_CV:
		return EX_VAR(op.var);
	case IS_CONST:
		return RT_CONSTANT(execute_data->opline, op);
	default:
		return NULL;
	}
}

zend_string *operator_method_name(zend_uchar opcode)
{
	switch (opcode)
	{
	case ZEND_ADD:
		return zend_string_init("__add", strlen("__add"), 1);
	case ZEND_PRE_INC:
		return zend_string_init("__pre_inc", strlen("__pre_inc"), 1);
	case ZEND_POST_INC:
		return zend_string_init("__post_inc", strlen("__post_inc"), 1);
	default:
		return NULL;
	}
}

zend_bool operator_get_method(zend_string *method, zval *obj,
							  zend_fcall_info *fci,
							  zend_fcall_info_cache *fcc)
{
	if (method == NULL)
	{
		return 0;
	}
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

static int op_handler_object(zend_execute_data *execute_data)
{
	zend_op *opline = execute_data->opline;
	zval *op1, *op2, *res;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_string *method = operator_method_name(opline->opcode);
	zend_string *any_action = zend_string_init("__any_action", strlen("__any_action"), 1);
	zval args;
	zval *args_t = malloc(sizeof(zval) * 2);
	array_init_size(&args, op2 ? 2 : 1);
	int is_any_action_flag = 0;

	if (opline->result_type == IS_UNUSED)
	{
		/* Assign op */
		op1 = EX_VAR(opline->op1.var);
	}
	else
	{
		op1 = get_zval_ptr_undef(opline->op1_type, opline->op1, execute_data);
	}
	ZVAL_DEREF(op1);

	if (opline->result_type == IS_UNUSED)
	{
		res = op1;
	}
	else
	{
		res = EX_VAR(opline->result.var);
	}

	op2 = get_zval_ptr_undef(opline->op2_type, opline->op2, execute_data);
	if (Z_TYPE_P(op1) != IS_OBJECT)
		return ZEND_USER_OPCODE_DISPATCH;
	if (!operator_get_method(method, op1, &fci, &fcc))
	{
		if (!operator_get_method(any_action, op1, &fci, &fcc))
			return ZEND_USER_OPCODE_DISPATCH;
		else
			is_any_action_flag = 1;
	}
	if (op2 == NULL)
	{
		op2 = malloc(sizeof(zval));
		ZVAL_NULL(op2);
	}
	if (is_any_action_flag)
	{
		zval val;
		int opcode = opline->opcode;
		ZVAL_LONG(&val, opcode);
		//	add_next_index_zval(&args, &val);
		//	add_next_index_zval(&args, op2);

		args_t[0] = val;
		args_t[1] = *op2;
		fci.retval = res;
		fci.params = args_t;
		fci.param_count = 2;
	}
	else
	{
		fci.retval = res;
		fci.params = op2;
		fci.param_count = op2 ? 1 : 0;
	}

	if (FAILURE == zend_call_function(&fci, &fcc))
	{
		php_error(E_WARNING, "Failed calling %s::%s()", Z_OBJCE_P(op1)->name, Z_STRVAL(fci.function_name));
		ZVAL_NULL(fci.retval);
	}
	zend_string_free(method);
	zend_string_free(any_action);
	free(args_t);
	EX(opline) = opline + 1;
	return ZEND_USER_OPCODE_CONTINUE;
}

static int op_handler_object1(zend_execute_data *execute_data)
{
	return ZEND_USER_OPCODE_DISPATCH;
}

static PHP_MINIT_FUNCTION(php_override)
{
	zend_set_user_opcode_handler(ZEND_ADD, op_handler_object);
	zend_set_user_opcode_handler(ZEND_POST_INC, op_handler_object);
	zend_set_user_opcode_handler(ZEND_PRE_INC, op_handler_object);
	// zend_set_user_opcode_handler(ZEND_SUB, op_handler_object);
	//  zend_set_user_opcode_handler(ZEND_SUB, op_handler);
	//   zend_set_user_opcode_handler(3, op_handler);
	//   zend_set_user_opcode_handler(4, op_handler);
}

zend_module_entry php_override_module_entry = {
	STANDARD_MODULE_HEADER,	 // #if ZEND_MODULE_API_NO >= 20010901
	"php_override",			 // название модуля
	php_override_functions,	 // указываем экспортируемые функции
	PHP_MINIT(php_override), // PHP_MINIT(php_override), Module Initialization
	NULL,					 // PHP_MSHUTDOWN(php_override), Module Shutdown
	NULL,					 // PHP_RINIT(php_override), Request Initialization
	NULL,					 // PHP_RSHUTDOWN(php_override), Request Shutdown
	NULL,					 // PHP_MINFO(php_override), Module Info (для phpinfo())
	"0.3",					 // версия нашего модуля
	STANDARD_MODULE_PROPERTIES};

ZEND_GET_MODULE(php_override)

PHP_FUNCTION(php_override)
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