install(
    TARGETS SaplingTactics_exe
    RUNTIME COMPONENT SaplingTactics_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
