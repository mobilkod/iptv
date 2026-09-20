#include <emscripten.h>
#include <cstdio>

int main() {
    std::printf("IPTV WASM Player started\n");

    EM_ASM({
        console.log("[IPTV WASM] Player started");
    });

    return 0;
}