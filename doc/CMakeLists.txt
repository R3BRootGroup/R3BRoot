find_package(Doxygen REQUIRED COMPONENTS dot)

include(FetchContent)
fetchcontent_declare(
    doxygen-awesome-css
    URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip)
fetchcontent_makeavailable(doxygen-awesome-css)

# Save the location the files were cloned into
# This allows us to get the path to doxygen-awesome.css
fetchcontent_getproperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# # Generate the Doxyfile
# set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
# set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
# configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
set(DOXYGEN_EXTRACT_PACKAGE YES)
set(DOXYGEN_EXTRACT_STATIC YES)
# set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_SOURCE_BROWSER YES)
# set(DOXYGEN_UML_LOOK YES)
set(DOXYGEN_TEMPLATE_RELATIONS YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_CLASS_GRAPH NO)
set(DOXYGEN_COLLABORATION_GRAPH NO)
set(DOXYGEN_USE_MATHJAX YES)
set(DOXYGEN_MAX_DOT_GRAPH_DEPTH 2)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
set(DOXYGEN_IMAGE_PATH "${PROJECT_SOURCE_DIR}/doc/pics" "${PROJECT_SOURCE_DIR}/neuland/docs/figs" "${PROJECT_SOURCE_DIR}/neuland/shared")

# For doxygen-awesome
set(DOXYGEN_HTML_EXTRA_STYLESHEET "${AWESOME_CSS_DIR}/doxygen-awesome.css"
                                  "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css")
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_FULL_SIDEBAR NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_HTML_COLORSTYLE LIGHT)
set(DOXYGEN_EXCLUDE_PATTERNS "*.py" "ext_h101_*")



doxygen_add_docs(
    doxygen
    ${PROJECT_SOURCE_DIR}/README.md
    ${PROJECT_SOURCE_DIR}/neuland
    ${PROJECT_SOURCE_DIR}/r3bbase
    ${PROJECT_SOURCE_DIR}/r3bdata
    ${PROJECT_SOURCE_DIR}/r3bsource
    ${PROJECT_SOURCE_DIR}/passive
    ${PROJECT_SOURCE_DIR}/config
    ${PROJECT_SOURCE_DIR}/doc
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generate doxygen documentation")
