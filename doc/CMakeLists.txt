find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Create venv
message(STATUS "Creating Sphinx venv")
execute_process(COMMAND "${Python3_EXECUTABLE}" -m venv .venv
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND_ERROR_IS_FATAL ANY
)
set(SPHINX_PYTHON "${CMAKE_CURRENT_SOURCE_DIR}/.venv/bin/python${CMAKE_EXECUTABLE_SUFFIX}")
execute_process(COMMAND "${SPHINX_PYTHON}" -m pip install -r requirements.txt
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND_ERROR_IS_FATAL ANY
)

# Create build info to send to Sphinx
set(SPHINX_BUILD_INFO "{ }")
string(JSON SPHINX_BUILD_INFO SET "${SPHINX_BUILD_INFO}" "name" "\"${PROJECT_NAME}\"")
string(JSON SPHINX_BUILD_INFO SET "${SPHINX_BUILD_INFO}" "author" "\"${PROJECT_AUTHOR}\"")
string(JSON SPHINX_BUILD_INFO SET "${SPHINX_BUILD_INFO}" "version" "\"${PROJECT_VERSION}\"")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/build_info.json" "${SPHINX_BUILD_INFO}")

# Generate doc
add_custom_target(${PROJECT_NAME}_doc_html
        COMMAND "${SPHINX_PYTHON}" -m sphinx -M html "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/html" "${CMAKE_CURRENT_BINARY_DIR}/doctrees"
        COMMENT "Running Sphinx..."
        VERBATIM
)
