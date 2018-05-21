file(GLOB files "*.cpp")

foreach(file ${files})

  get_filename_component(file_name "${file}" NAME)

  try_compile(
    COMPILE_SUCCEEDED
    ${CMAKE_BINARY_DIR}/compilation_errors
    ${file}
    CMAKE_FLAGS
      "-DINCLUDE_DIRECTORIES=${CMAKE_SOURCE_DIR}/include"
      "-DCXX_STANDARD=14"
      "-DCXX_STANDARD_REQUIRED=ON"
      "-DCXX_EXTENSIONS=OFF"
    OUTPUT_VARIABLE TRY_OUT
    )

  if(COMPILE_SUCCEEDED)
    message(FATAL_ERROR "${file_name} FAIL!")
  else()
    message("${file_name} PASS!")
    message("OUTPUT_VARIABLE ${TRY_OUT}")
  endif()

endforeach()
