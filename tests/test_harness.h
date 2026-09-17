#pragma once
#include <iostream>
#include <string>

namespace mju::test {
class Runner {
public:
    void expect(bool condition, const char* expression, const char* file, int line) {
        if (condition) return;
        ++failures_;
        std::cerr << file << ':' << line << ": check failed: " << expression << '\n';
    }

    int finish() const {
        if (failures_ == 0) {
            std::cout << "PASS\n";
            return 0;
        }
        std::cerr << failures_ << " check(s) failed\n";
        return 1;
    }

private:
    int failures_ = 0;
};
}

#define MJU_EXPECT(runner, expression) \
    (runner).expect(static_cast<bool>(expression), #expression, __FILE__, __LINE__)
