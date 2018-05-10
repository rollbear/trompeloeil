file(GLOB files "*.cpp")

foreach(file ${files})

  get_filename_component(file_name "${file}" NAME)

  try_compile(COMPILE_SUCCEEDED
              ${CMAKE_BINARY_DIR}/compilation_errors
              ${file})

  if(COMPILE_SUCCEEDED)
    message(FATAL_ERROR "${file_name} FAIL!")
  else()
    message("${file_name} PASS!")
  endif()

endforeach()