PHP_ARG_ENABLE(php_override, Enable test support)

if test "$PHP_PHP_OVERRIDE" = "yes"; then
   AC_DEFINE(HAVE_PHP_OVERRIDE_EXTENSION, 1, [You have test extension])
   PHP_NEW_EXTENSION(php_override, extension/php_override.c, $ext_shared)
fi