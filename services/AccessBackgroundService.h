//
// Created by artur on 20/01/2026.
//
#include <WebViewUI.h>

class AccessBackgroundService {
public:
    explicit AccessBackgroundService(std::shared_ptr<WebViewUI> webView);
    void start() const;
    void stop();

private:
    std::shared_ptr<WebViewUI> webView;
    static void loop();
};
