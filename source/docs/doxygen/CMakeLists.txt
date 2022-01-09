find_package (Doxygen)
if (DOXYGEN_FOUND)
  set (DOXYGEN_TAB_SIZE 2)
  set (DOXYGEN_LAYOUT_FILE ${CMAKE_CURRENT_LIST_DIR}/DoxygenLayout.xml)
  set (DOXYGEN_HTML_HEADER ${CMAKE_CURRENT_LIST_DIR}/html/header.html)
  set (DOXYGEN_HTML_FOOTER ${CMAKE_CURRENT_LIST_DIR}/html/footer.html)
  set (DOXYGEN_HTML_STYLESHEET ${CMAKE_CURRENT_LIST_DIR}/html/stylesheet.css)

  set (DOXYGEN_HTML_EXTRA_STYLESHEET
       ${PROJECT_SOURCE_DIR}/external/doxygen-awesome-css/doxygen-awesome.css)

  set (DOXYGEN_GENERATE_TREEVIEW YES)

  doxygen_add_docs (
    optional_ref.doxygen
    gch/optional_ref.hpp
    WORKING_DIRECTORY
      ${PROJECT_SOURCE_DIR}/source/include
  )
else ()
  message (
    WARNING
    "Doxygen installation not found. Build files for documentation will not be generated"
  )
endif ()
