if (NOT APPOPTS_BUILD_MODE)
    set (APPOPTS_BUILD_MODE STATIC)
endif ()

include(pile_support)
pileInclude (AppOpts)
appoptsInit(${APPOPTS_BUILD_MODE})
