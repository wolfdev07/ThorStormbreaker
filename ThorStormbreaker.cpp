// ThorStormbreaker.cpp: define the entry point app.
#include "ThorStormbreaker.h"
#include "WebViewUI.h"
#include "WebBindServices/FingerEnrollBind.h"

#include <iostream>
#include <memory>
#include <chrono>

using namespace std;

static void initUI() {
    auto fingerEnrollBind = std::make_shared<FingerEnrollBind>();

    try {

        WebViewUI webViewUI;

        webViewUI.bind("fingerEnrollService", [fingerEnrollBind](const std::string& request, auto resolve) {
            fingerEnrollBind->fingerEnroll(request, resolve);
        });

        webViewUI.loadUrl("http://localhost:8000");
        webViewUI.run();
    }
    catch (const exception& e) {
        cerr << "[ERROR] " << e.what() << endl;
    }
}

int main() {
    initUI();
    return 0;
}
