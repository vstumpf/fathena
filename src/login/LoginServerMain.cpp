#include "login.hpp"

using rathena::server_login::LoginServer;

int main(int argc, char *argv[]) {
    return main_core<LoginServer>(argc, argv);
}
