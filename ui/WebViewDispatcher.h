//
// Created by artur on 19/01/2026.
//
#pragma once
#include <functional>
#include <queue>
#include <mutex>

class WebViewDispatcher {
public:
    void dispatch(std::function<void()> fn) {
        std::lock_guard<std::mutex> lock(mutex);
        queue_.push(std::move(fn));
    };

    void process() {
        std::queue<std::function<void()>> local;

        {
            std::lock_guard<std::mutex> lock(mutex);
            std::swap(local, queue_);
        }

        while (!local.empty()) {
            local.front()();
            local.pop();
        }
    }

private:
    std::mutex mutex;
    std:: queue<std::function<void()>> queue_;
};