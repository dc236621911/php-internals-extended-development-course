dnl $Id$
dnl config.m4 for extension pib

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(pib, for pib support,
dnl Make sure that the comment is aligned:
dnl [  --with-pib             Include pib support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(pib, whether to enable pib support,
dnl Make sure that the comment is aligned:
[  --enable-pib           Enable pib support])

if test "$PHP_PIB" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-pib -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/pib.h"  # you most likely want to change this
  dnl if test -r $PHP_PIB/$SEARCH_FOR; then # path given as parameter
  dnl   PIB_DIR=$PHP_PIB
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for pib files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PIB_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PIB_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the pib distribution])
  dnl fi

  dnl # --with-pib -> add include path
  dnl PHP_ADD_INCLUDE($PIB_DIR/include)

  dnl # --with-pib -> check for lib and symbol presence
  dnl LIBNAME=pib # you may want to change this
  dnl LIBSYMBOL=pib # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PIB_DIR/$PHP_LIBDIR, PIB_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PIBLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong pib lib version or lib not found])
  dnl ],[
  dnl   -L$PIB_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PIB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pib, pib.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
