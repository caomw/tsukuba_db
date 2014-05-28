option(BUILD_TSUKUBA_DB_SHARED "Build the Tsukuba DB library shared." ON)

if(BUILD_TSUKUBA_DB_SHARED)
  set(TSUKUBA_DB_LIBRARY_TYPE SHARED)
else(BUILD_TSUKUBA_DB_SHARED)
  set(TSUKUBA_DB_LIBRARY_TYPE STATIC)
endif(BUILD_TSUKUBA_DB_SHARED)

add_library(tsukuba_db ${TSUKUBA_DB_LIBRARY_TYPE} db.cpp)

install(TARGETS tsukuba_db DESTINATION lib)
install(FILES db.h DESTINATION include)
