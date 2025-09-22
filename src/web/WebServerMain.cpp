#include "web.hpp"

using rathena::server_web::WebServer;


int main(int argc, char *argv[]) {
	return main_core<WebServer>(argc, argv);
}
