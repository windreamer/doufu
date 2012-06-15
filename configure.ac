AC_INIT([JXQ], 0.1)
AC_PREREQ(2.59)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([include/config.h])
AC_PROG_CC
AC_PROG_CXX
AC_CHECK_LIB(pthread, pthread_create,,
             [AC_MSG_ERROR([required library pthread missing])])
AC_CONFIG_FILES([Makefile coordinator/Makefile barrier/Makefile])
AC_OUTPUT
