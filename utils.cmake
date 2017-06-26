
SET(CMAKE_USE_RELATIVE_PATHS ON)
#include directories
include("FindPythonInterp")
include_directories("include")
include_directories(".")
#api
macro(add_api prefix apiname outdir)
	set(api_FILES )	
	aux_source_directory(${prefix}/${apiname} api_FILES)
	set_source_files_properties(${api_FILES} PROPERTIES COMPILE_FLAGS ${CFLAGS}\ -I${prefix}/${apiname}/include)
	add_library(${apiname} ${api_FILES})
endmacro(add_api)

macro(add_plugin prefix plugin apidir outdir)
	set(plugin_FILES )
	string(TOUPPER ${plugin} plugin_flag)
	set(plugin_flag -DPLUGIN_${plugin_flag})
	aux_source_directory(${prefix}/${plugin} plugin_FILES)
	set_source_files_properties(${plugin_FILES} PROPERTIES COMPILE_FLAGS ${CFLAGS}\ -I${apidir}/include\ -I${prefix}/include\ ${plugin_flag})
	add_library(${plugin} SHARED ${plugin_FILES})
	target_link_libraries(${plugin} pthread)
	set_target_properties(${plugin} PROPERTIES LINK_FLAGS ${LDFLAGS}\ -fPIC)
	set_target_properties(${plugin} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outdir})
endmacro(add_plugin)

#test
macro(add_utest testname)
	set(__sources__ test/${testname}.c)
	set_source_files_properties(${__sources__} PROPERTIES COMPILE_FLAGS ${CFLAGS})
	add_executable(test/test_${testname} ${__sources__})
	target_link_libraries(test/test_${testname} glimpse)
	target_link_libraries(test/test_${testname} pthread)
	set_target_properties(test/test_${testname} PROPERTIES LINK_FLAGS ${LDFLAGS})
endmacro(add_utest)

macro(add_utest_with_api testname api apidir)
	set(__sources__ test/${testname}.c)
	set_source_files_properties(${__sources__} PROPERTIES COMPILE_FLAGS ${CFLAGS}\ -I${apidir}/include\ -Iplugin/include)
	add_executable(test/test_${testname} ${__sources__})
	target_link_libraries(test/test_${testname} ${api})
	target_link_libraries(test/test_${testname} glimpse)
	target_link_libraries(test/test_${testname} dl)
	target_link_libraries(test/test_${testname} pthread)
	set_target_properties(test/test_${testname} PROPERTIES LINK_FLAGS ${LDFLAGS})
endmacro(add_utest_with_api)

macro(install_headers incdir dir)
	file(GLOB files "${incdir}/${dir}/*.h")
	foreach(header ${files})
		install(FILES ${header} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glimpse/${dir})
	endforeach(header ${files})
endmacro(install_headers)

macro(install_api apidir apiname)
	install(TARGETS ${apiname} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/glimpse/)
	file(GLOB files "${apidir}/${apiname}/include/*.h")
	foreach(header ${files})
		install(FILES ${header} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glimpse/${apiname})
	endforeach(header ${files})
endmacro(install_api)

macro(install_plugin pdir pname)
	install(TARGETS ${pname} DESTINATION ${plugin_dir})
	file(GLOB files "${pdir}/include/${pname}/*.h")
	foreach(header ${files})
		install(FILES ${header} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/glimpse/plugin/${pname})
	endforeach(header ${files})
endmacro(install_plugin)

