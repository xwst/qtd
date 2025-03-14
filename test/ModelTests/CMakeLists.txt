FUNCTION(create_model_test test_name source_file)
    QT_ADD_EXECUTABLE(
        ${test_name}
        ${source_file}
        ../testhelpers.cpp
        persistedtreeitemmodelstestbase.cpp
    )
    TARGET_LINK_LIBRARIES(${test_name}
        PRIVATE
        Qt6::Gui
        Qt6::Test
        Qt6::Sql
        models
    )
    ADD_TEST(NAME ${test_name} COMMAND ${test_name})
    QT_ADD_RESOURCES(
        ${test_name} "test-resources"
        PREFIX "/"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/../"
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/../resources/sql/generic/populate_database.sql"
    )
    QT_ADD_RESOURCES(
        ${test_name} "resources"
        PREFIX "/"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        FILES ${RESOURCE_FILES}
    )
ENDFUNCTION()


CREATE_MODEL_TEST(test_treenodes testtreenodes.cpp)
CREATE_MODEL_TEST(test_treeitemmodel testtreeitemmodel.cpp)
CREATE_MODEL_TEST(test_tagitemmodels testtagitemmodels.cpp)
CREATE_MODEL_TEST(test_taskitemmodels testtaskitemmodels.cpp)
