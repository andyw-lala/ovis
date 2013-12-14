dnl SYNOPSIS: OPTION_DEFAULT_ENABLE([name], [enable_flag_var])
dnl EXAMPLE: OPTION_DEFAULT_ENABLE([mysql], [ENABLE_MYSQL])
dnl note: supports hyphenated feature names now.
AC_DEFUN([OPTION_DEFAULT_ENABLE], [
AC_ARG_ENABLE($1, [  --disable-$1     Disable the $1 module],
        [       if test "x$enableval" = "xno" ; then
                        disable_]m4_translit([$1], [-+.], [___])[=yes
                        enable_]m4_translit([$1], [-+.], [___])[=no
			AC_MSG_NOTICE([Disable $1 module requested ])
                fi
        ], [ AC_MSG_NOTICE([Disable $1 module NOT requested]) ])
AM_CONDITIONAL([$2], [test "$disable_]m4_translit([$1], [-+.], [___])[" != "yes"])
])

dnl SYNOPSIS: OPTION_DEFAULT_DISABLE([name], [enable_flag_var])
dnl EXAMPLE: OPTION_DEFAULT_DISABLE([mysql], [ENABLE_MYSQL])
dnl note: supports hyphenated feature names now.
AC_DEFUN([OPTION_DEFAULT_DISABLE], [
AC_ARG_ENABLE($1, [  --enable-$1     Enable the $1 module],
        [       if test "x$enableval" = "xyes" ; then
                        enable_]m4_translit([$1], [-+.], [___])[=yes
                        disable_]m4_translit([$1], [-+.], [___])[=no
			AC_MSG_NOTICE([Enable $1 module requested])
                fi
        ], [ AC_MSG_NOTICE([Enable $1 module NOT requested]) ])
AM_CONDITIONAL([$2], [test "$enable_]m4_translit([$1], [-+.], [___])[" == "yes"])
])

dnl SYNOPSIS: OPTION_WITH([name], [VAR_BASE_NAME])
dnl EXAMPLE: OPTION_WITH([xyz], [XYZ])
dnl NOTE: With VAR_BASE_NAME being XYZ, this macro will set XYZ_INCIDR and
dnl 	XYZ_LIBDIR to the include path and library path respectively.
AC_DEFUN([OPTION_WITH], [
AC_ARG_WITH(
	$1,
	AS_HELP_STRING(
		[--with-$1@<:@=path@:>@],
		[Specify $1 path @<:@default=/usr/local@:>@]
	),
	[WITH_$2=$withval
	 AM_CONDITIONAL([ENABLE_$2], [true])
	],
	[WITH_$2=/usr/local]
)

if test -d $WITH_$2/lib; then
	$2_LIBDIR=$WITH_$2/lib
	$2_LIBDIR_FLAG=-L$WITH_$2/lib
fi
if test "x$$2_LIBDIR" = "x"; then
	$2_LIBDIR=$WITH_$2/lib64
	$2_LIBDIR_FLAG=-L$WITH_$2/lib64
fi
if test -d $WITH_$2/lib64; then
	$2_LIB64DIR=$WITH_$2/lib64
	$2_LIB64DIR_FLAG=-L$WITH_$2/lib64
fi
if test -d $WITH_$2/include; then
	$2_INCDIR=$WITH_$2/include
	$2_INCDIR_FLAG=-I$WITH_$2/include
fi
AC_SUBST([$2_LIBDIR], [$$2_LIBDIR])
AC_SUBST([$2_LIB64DIR], [$$2_LIB64DIR])
AC_SUBST([$2_INCDIR], [$$2_INCDIR])
AC_SUBST([$2_LIBDIR_FLAG], [$$2_LIBDIR_FLAG])
AC_SUBST([$2_LIB64DIR_FLAG], [$$2_LIB64DIR_FLAG])
AC_SUBST([$2_INCDIR_FLAG], [$$2_INCDIR_FLAG])
])

dnl SYNOPSIS: OPTION_WITH_OR_BUILD(featurename,buildincdir,buildlibdirs)
dnl EXAMPLE: OPTION_WITH_OR_BUILD([sos], [../sos/src])
dnl NOTE: With featurename being sos, this macro will set SOS_INCIDR and
dnl 	SOS_LIBDIR to the include path and library path respectively.
dnl if user does not specify prefix of a prior install, the
dnl source tree at relative location builddir will be assumed useful.
dnl The relative location should have been generated by some configure output
dnl prior to its use here.
AC_DEFUN([OPTION_WITH_OR_BUILD], [
AC_ARG_WITH(
	$1,
	AS_HELP_STRING(
		[--with-$1@<:@=path@:>@],
		[Specify $1 path @<:@default=in build tree@:>@]
	),
	[WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[=$withval
	 AM_CONDITIONAL([ENABLE_]m4_translit([$1], [-+.a-z], [___A-Z])[], [true])
	],
	[WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[=build]
)

[if test "x$WITH_]m4_translit([$1], [-a-z], [_A-Z])[" != "xbuild"; then
	if test -d $WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib; then
		]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR=$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib
		]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR_FLAG=-L$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib
	fi
	if test "x$]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR" = "x"; then
		]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR=$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib64
		]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR_FLAG=-L$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib64
	fi
	if test -d $WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib64; then
		]m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR=$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib64
		]m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR_FLAG=-L$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/lib64
	fi
	if test -d $WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/include; then
		]m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR=$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/include
		]m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR_FLAG=-I$WITH_]m4_translit([$1], [-+.a-z], [___A-Z])[/include
	fi
else
	# sosbuilddir should exist by ldms configure time

	tmpsrcdir=`(cd $srcdir/$2 && pwd)`
	dirlist=""
	if test -n "$3"; then
		for dirtmp in $3; do
			tmpbuilddir=`(cd $dirtmp && pwd)` && tmpflag="$tmpflag -L$tmpbuilddir" && dirlist="$dirlist $tmpbuilddir"
		done
	else
		tmpbuilddir=`(cd $2 && pwd)`
		tmpflag="-L$tmpbuilddir"
	fi
	]m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR=$tmpsrcdir
	]m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR_FLAG=-I$tmpsrcdir
	]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR=$dirlist
	]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR_FLAG=$tmpflag
	]m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR_FLAG=""
	]m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR=""
fi
]
AC_SUBST(m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR], [$]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR])
AC_SUBST(m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR], [$]m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR])
AC_SUBST(m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR], [$]m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR])
AC_SUBST(m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR_FLAG], [$]m4_translit([$1], [-+.a-z], [___A-Z])[_LIBDIR_FLAG])
AC_SUBST(m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR_FLAG], [$]m4_translit([$1], [-+.a-z], [___A-Z])[_LIB64DIR_FLAG])
AC_SUBST(m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR_FLAG], [$]m4_translit([$1], [-+.a-z], [___A-Z])[_INCDIR_FLAG])
])

dnl Similar to OPTION_WITH, but a specific case for MYSQL
AC_DEFUN([OPTION_WITH_MYSQL], [
AC_ARG_WITH(
	[mysql],
	AS_HELP_STRING(
		[--with-mysql@<:@=path@:>@],
		[Specify mysql path @<:@default=/usr/local@:>@]
	),
	[	dnl $withval is given.
		WITH_MYSQL=$withval
		mysql_config=$WITH_MYSQL/bin/mysql_config
	],
	[	dnl $withval is not given.
		mysql_config=`which mysql_config`
	]
)

if test $mysql_config
then
	MYSQL_LIBS=`$mysql_config --libs`
	MYSQL_INCLUDE=`$mysql_config --include`
else
	AC_MSG_ERROR([Cannot find mysql_config, please specify
			--with-mysql option.])
fi
AC_SUBST([MYSQL_LIBS])
AC_SUBST([MYSQL_INCLUDE])
])
dnl this could probably be generalized for handling lib64,lib python-binding issues
AC_DEFUN([OPTION_WITH_EVENT],[
  AC_ARG_WITH([libevent],
  [  --with-libevent=DIR      use libevent in DIR],
  [ case "$withval" in
    yes|no)
      AC_MSG_RESULT(no)
      ;;
    *)
      if test "x$withval" != "x/usr"; then
        CPPFLAGS="-I$withval/include"
        EVENTLIBS="-L$withval/lib -L$withval/lib64"
      fi
      ;;
    esac ])
  option_old_libs=$LIBS
  AC_CHECK_LIB(event, event_base_new, [EVENTLIBS="$EVENTLIBS -levent"],
      AC_MSG_ERROR([event_base_new() not found. sock requires libevent.]),[$EVENTLIBS])
  AC_SUBST(EVENTLIBS)
  LIBS=$option_old_libs
])
