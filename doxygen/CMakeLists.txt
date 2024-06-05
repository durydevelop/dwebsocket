

# https://www.doxygen.nl/manual/customize.html
# https://cmake.org/cmake/help/latest/module/FindDoxygen.html#command:doxygen_add_docs

# TODO:
# https://github.com/kracejic/EmptyDoxygenCMake/tree/master?tab=readme-ov-file
# PlantUML = for easy generation of custom diagrams

# sudo apt-get install graphviz
# sudo apt-get install mscgen
# sudo apt-get install dia

set(DOXYGEN_THEME_NAME "Custom theme folder" default)

find_package(
    Doxygen
    OPTIONAL_COMPONENTS dot mscgen dia
)
message_c(${BOLD_MAGENTA} "Doxygen found ${DOXYGEN_FOUND}")

if(DOXYGEN_FOUND)
    # Project info
    set(DOXYGEN_PROJECT_NAME ${CMAKE_PROJECT_NAME})
    set(DOXYGEN_PROJECT_NUMBER ${VERSION})

    # Files
    set(DOXYGEN_FILE_PATTERNS
        *.h
        *.hpp
        *.hh
        *.c
        *.cc
        *.cpp.in
        *.cpp
        *.md
    )
    set(DOXYGEN_EXCLUDE_PATTERNS _*)

    # Navigation
    set(DOXYGEN_DISABLE_INDEX YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_FULL_SIDEBAR NO)

    # Locale
    if (DOXYGEN_LANGUAGE)
        set(DOXYGEN_OUTPUT_LANGUAGE ${DOXYGEN_LANGUAGE})
    endif()
    
    # Misc
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")
    set(DOXYGEN_ALWAYS_DETAILED_SEC YES)
    set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
    set(DOXYGEN_QUIET YES)

    # Warnings
    set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
    set(DOXYGEN_WARN_NO_PARAMDOC NO)

    # Theme
    if (DOXYGEN_THEME_NAME AND NOT DOXYGEN_THEME_NAME MATCHES "default")
        # defined from variable
        message_c(${BOLD_GREEN} "Custom theme: ${DOXYGEN_THEME_NAME}")
        set(DOXYGEN_GENERATE_HTML YES)
        #add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/themes/${DOXYGEN_THEME})
        set(DOXYGEN_THEME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/themes/${DOXYGEN_THEME_NAME})
        include(${DOXYGEN_THEME_DIR}/DoxyTheme.cmake)
    endif()
        
    # Graphviz
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_GRAPHICAL_HIERARCHY YES)
    set(DOXYGEN_DIRECTORY_GRAPH YES)
    set(DOXYGEN_GENERATE_LEGEND YES)
    set(DOXYGEN_INCLUDED_BY_GRAPH YES)
    set(DOXYGEN_INCLUDE_GRAPH YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT png:cairo:cairo)

    # in class diagrams, you will have members and such
    # Also they will be bigger
    #set(DOXYGEN_UML_LOOK YES)
    #set(DOXYGEN_UML_LIMIT_NUM_FIELDS 16)

    # If the REFERENCED_BY_RELATION tag is set to YES then for each documented
    # function all documented functions referencing it will be listed.
    # The default value is: NO.
    set(DOXYGEN_REFERENCED_BY_RELATION YES)

    # More insight to templates, generaly not needed
    set(DOXYGEN_TEMPLATE_RELATIONS YES)

    # should all pictures be collapsed?
    set(DOXYGEN_HTML_DYNAMIC_SECTIONS NO)

    # we want all we can get
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)

    # this makes first sentence from comment block a brief description.
    # It is VERY useful
    set(DOXYGEN_JAVADOC_AUTOBRIEF YES)

    # Why not...
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

    # Do we want source code browser? YES! Do we want strip comments? NO
    set(DOXYGEN_SOURCE_BROWSER YES)
    set(DOXYGEN_STRIP_CODE_COMMENTS NO)

    # Use SHORT_NAMES to prevent Doxygen from generating filenames with double-quotes.
    # ALLOW_UNICODE_NAMES does not escape double-quotes.
    set(DOXYGEN_SHORT_NAMES YES)

    # use with: /// @todo Do more stuff.
    set(DOXYGEN_GENERATE_TODOLIST YES)

    # https://cmake.org/cmake/help/latest/module/FindDoxygen.html#command:doxygen_add_docs
    doxygen_add_docs(doc
        ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generate Documentation"
        #CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/DoxyCustom
    )
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc DESTINATION share COMPONENT doc EXCLUDE_FROM_ALL)
else()
    message("Install Doxygen to generate the documentation.")
endif()
