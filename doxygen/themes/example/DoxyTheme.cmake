#### Example theme with custom style,header,footer and logo

set(DOXYGEN_HTML_EXTRA_STYLESHEET ${DOXYGEN_THEME_DIR}/customdoxygen.css)
set(DOXYGEN_HTML_HEADER ${DOXYGEN_THEME_DIR}/customheader.html)
set(DOXYGEN_HTML_FOOTER ${DOXYGEN_THEME_DIR}/customfooter.html)

# max size 200x55px
set(DOXYGEN_PROJECT_LOGO  ${DOXYGEN_THEME_DIR}/logo.png)
# Color style
set(DOXYGEN_HTML_COLORSTYLE_HUE 231)
set(DOXYGEN_HTML_COLORSTYLE_SAT 90)
set(DOXYGEN_HTML_COLORSTYLE_GAMMA 55)