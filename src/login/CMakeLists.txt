add_library(login)

target_sources(login PRIVATE
	"ipban.cpp"
	"login.cpp"
	"loginchrif.cpp"
	"loginclif.cpp"
	"logincnslif.cpp"
	"loginlog.cpp"
)

if(WIN32)
	target_sources(login PRIVATE
		"ipban.hpp"
		"loginchrif.hpp"
		"loginclif.hpp"
		"logincnslif.hpp"
		"login.hpp"
		"loginlog.hpp"
	)

	set_target_properties(login PROPERTIES FOLDER "Servers")
endif()

target_link_libraries(login PUBLIC
	common
	accountdb
)

# common depends on login for the implementations of some functions
# TODO Remove this circular dependency by refactoring those functions into Core::<func> and overriding them
target_link_libraries(common PUBLIC login)

add_executable(login-server "LoginServerMain.cpp")
target_link_libraries(login-server PRIVATE login)

add_subdirectory(accountdb)
