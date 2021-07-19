function(daw_add_define var doc val)
  option("${var}" "${doc}" "${val}")
  if("${${var}}")
    target_compile_definitions(daw_json_link INTERFACE "${var}")
  endif()
endfunction()

daw_add_define(DAW_JSON_PARSER_DIAGNOSTICS "Output debug info while \
parsing" OFF)

daw_add_define(DAW_NO_FLATTEN "Disable function flattening optimization" OFF)

daw_add_define(DAW_JSON_NO_FAIL_ON_NO_NAME_NAME "Ensure that no named mapping \
uses no_name as it's name. Using it can slow down compilation" ON)

option(DAW_JSON_FORCE_INT128 "Force support for 128bit int" OFF)
if(DAW_JSON_FORCE_INT128)
  target_compile_definitions(daw_json_link INTERFACE DAW_HAS_INT128)
endif()

option(DAW_USE_EXCEPTIONS "Throw exceptions when json errors occur" ON)
set(define DAW_DONT_USE_EXCEPTIONS)
if(DAW_USE_EXCEPTIONS)
  set(define DAW_USE_EXCEPTIONS)
endif()
target_compile_definitions(daw_json_link INTERFACE "${define}")
