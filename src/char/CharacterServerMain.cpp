#include "char.hpp"

using rathena::server_character::CharacterServer;

int main(int argc, char *argv[]) {
    return main_core<CharacterServer>(argc, argv);
}
