if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/solver_fcbiilu2.h" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/solver_fcbiilu2.cpp" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fcbiilu2.cpp")
    set(SOLVER_DEFINITIONS ${SOLVER_DEFINITIONS} -DHAVE_SOLVER_FCBIILU2 PARENT_SCOPE)
    set(HAVE_SOLVER_FCBIILU2 TRUE PARENT_SCOPE)
    set(SOURCE ${SOURCE}
            ${CMAKE_CURRENT_SOURCE_DIR}/solver_fcbiilu2.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/fcbiilu2.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/SolverFCBIILU2.cpp
            PARENT_SCOPE)
    set(HEADER ${HEADER}
            ${CMAKE_CURRENT_SOURCE_DIR}/solver_fcbiilu2.h
            ${CMAKE_CURRENT_SOURCE_DIR}/SolverFCBIILU2.h
            PARENT_SCOPE)
else ()
    set(HAVE_SOLVER_FCBIILU2 FALSE PARENT_SCOPE)
endif ()