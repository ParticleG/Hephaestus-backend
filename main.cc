#include <drogon/drogon.h>

using namespace drogon;
using namespace std;

int main() {
    app().loadConfigFile("config.json");
    app().run();
    return 0;
}
