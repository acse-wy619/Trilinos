IF(ROL_ENABLE_MiniTensor)
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    test_01
    SOURCES test_01.cpp
    ARGS PrintItAll
    COMM serial mpi  
    PASS_REGULAR_EXPRESSION "PASSED"
    ADD_DIR_TO_NAME
    )
ENDIF()
