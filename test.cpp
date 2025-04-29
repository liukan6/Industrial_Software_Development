#include <iostream>
#include <ranges>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5};
    auto it = std::ranges::find(nums, 3);
    if (it != nums.end()) {
        std::cout << "Found 3!" << std::endl;
    }
}
