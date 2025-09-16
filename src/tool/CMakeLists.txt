add_executable(mapcache)
target_link_libraries(mapcache PRIVATE minicore)
target_sources(mapcache PRIVATE "mapcache.cpp")
if(WIN32)
	set_target_properties(mapcache PROPERTIES FOLDER "Tools")
endif()

add_executable(yamlupgrade)
target_link_libraries(yamlupgrade PRIVATE minicore)
target_sources(yamlupgrade PRIVATE "yamlupgrade.cpp")
if(WIN32)
	set_target_properties(yamlupgrade PROPERTIES FOLDER "Tools")
	target_sources(yamlupgrade PRIVATE
		"yamlupgrade.hpp"
		"yaml.hpp"
	)
endif()

add_custom_target(tools DEPENDS mapcache yamlupgrade map-server-generator)
