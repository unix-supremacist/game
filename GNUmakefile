include make/cco.mk

ifeq ($(shell which samu), )
	ifeq ($(shell which ninja, ))
		$error("ERROR INSTALL NINJA/SAMURAI")
	else
		BUILD_TOOL = ninja
	endif
else
	BUILD_TOOL = samu
endif

ifeq ($(CC),gcc)
	OPTFLAGS += $(gcc)
else ifeq ($(CC),clang)
	OPTFLAGS = $(clang)
endif

include make/make.mk