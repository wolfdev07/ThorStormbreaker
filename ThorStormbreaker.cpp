// ThorStormbreaker.cpp: define el punto de entrada de la aplicación.
#include "ThorStormbreaker.h"
#include "WebViewUI.h"
#include <iostream>

using namespace std;

static void initUI() {
    try {
        WebViewUI webView;
        webView.loadUrl("http://localhost:8000");
        webView.run();
    }
    catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << endl;
    }
}

int main() {
    initUI();
    return 0;
}
