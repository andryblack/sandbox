#include "../../src/application.h"

void register_vk_auth(lua_State* L){}

int main(int argc,char** argv) {
    Application* app = new Application();
    return GHL_StartApplication(app,argc,argv);
}
