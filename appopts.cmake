
# enable/disable cmake debug messages related to this pile
set (APPOPTS_DEBUG_MSG ON)

# make sure support code is present; no harm
# in including it twice; the user, however, should have used
# pileInclude() from pile_support.cmake module.
include(pile_support)

# initialize this module
macro    (appoptsInit
          ref_cnt_use_mode)

    # default name
    if (NOT APPOPTS_INIT_NAME)
        set(APPOPTS_INIT_NAME "AppOpts")
    endif ()

    # compose the list of headers and sources
    set(APPOPTS_HEADERS
        "appopts.h")
    set(APPOPTS_SOURCES
        "appopts.cc")

    pileSetSources(
        "${APPOPTS_INIT_NAME}"
        "${APPOPTS_HEADERS}"
        "${APPOPTS_SOURCES}")

    pileSetCommon(
        "${APPOPTS_INIT_NAME}"
        "0;0;1;d"
        "ON"
        "${ref_cnt_use_mode}"
        ""
        "category1"
        "tag1;tag2")

endmacro ()
