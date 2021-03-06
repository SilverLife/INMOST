project(FVDiscr)
set(SOURCE main.cpp)

add_executable(FVDiscr ${SOURCE})

target_link_libraries(FVDiscr inmost)

if(USE_SOLVER)
  if(USE_SOLVER_ANI)
    message("linking FVDiscr with ani3d and BLAS")
    target_link_libraries(FVDiscr ani3d ${BLAS_LIBRARIES})
    if(BLAS_LINKER_FLAGS)
      set_target_properties(FVDiscr PROPERTIES LINK_FLAGS "${BLAS_LINKER_FLAGS}")
    endif()
  endif()
  if(USE_SOLVER_PETSC)
    message("linking FVDiscr with PETSc")
    target_link_libraries(FVDiscr ${PETSC_LIBRARIES})
  endif()
  if(USE_SOLVER_TRILINOS)
    message("linking FVDiscr with Trilinos")
    target_link_libraries(FVDiscr ${Trilinos_LIBRARIES} ${Trilinos_TPL_LIBRARIES})
  endif()
  if(USE_SOLVER_METIS)
    message("linking FVDiscr with Metis")
    target_link_libraries(FVDiscr ${METIS_LIBRARIES})
  endif()
  if(USE_SOLVER_MONDRIAAN)
    message("linking FVDiscr with Mondriaan")
    target_link_libraries(FVDiscr ${MONDRIAAN_LIBRARIES})
  endif()
  if(USE_SOLVER_SUPERLU)
    message("linking FVDiscr with SuperLU")
    target_link_libraries(FVDiscr ${SUPERLU_LIBRARIES})
  endif()
endif()


if(USE_PARTITIONER)
  if(USE_PARTITIONER_ZOLTAN)
    message("linking FVDiscr with Zoltan")
    target_link_libraries(FVDiscr ${ZOLTAN_LIBRARIES})
  endif()
  if(USE_PARTITIONER_PARMETIS)
    message("linking FVDiscr with ParMETIS")
    target_link_libraries(FVDiscr ${PARMETIS_LIBRARIES})
  endif()
endif()

if(USE_MPI)
  message("linking FVDiscr with MPI")
  target_link_libraries(FVDiscr ${MPI_LIBRARIES}) 
  if(MPI_LINK_FLAGS)
    set_target_properties(FVDiscr PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
  endif() 
endif(USE_MPI)

install(TARGETS FVDiscr EXPORT inmost-targets RUNTIME DESTINATION bin)
