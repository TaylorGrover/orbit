set(source_files
   main.dox
)

foreach(file IN LISTS source_files)
   string(APPEND ORBIT_DOXYGEN_INPUT " \\\n\"${CMAKE_CURRENT_SOURCE_DIR}/${file}\"")
endforeach()

configure_file(Doxyfile.in Doxyfile @ONLY)

add_custom_command(
   OUTPUT "html/index.html"
   COMMAND "${DOXYGEN_EXECUTABLE}"
   WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
   MAIN_DEPENDENCY Doxyfile
   DEPENDS ${source_files}
   COMMENT "Generating HTML documentation"
   VERBATIM
)

add_custom_target(docs ALL SOURCES "html/index.html")
