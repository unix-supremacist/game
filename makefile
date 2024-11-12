include make/cco.mk

SAMU != which samu
NINJA != which ninja

.if "${SAMU}" == ""
.if "${NINJA}" == ""
printf "no buildtool"
.else
BUILD_TOOL = ninja
.endif
.else
BUILD_TOOL = samu
.endif

.if "${CC}" == "gcc"
OPTFLAGS += $(gcc)
.elif "${CC}" == "clang"
OPTFLAGS = $(clang)
.endif

include make/make.mk