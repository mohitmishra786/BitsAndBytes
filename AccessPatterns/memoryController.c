#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <algorithm>
#include <random>
#include <chrono>

class MemoryController {
public:
    static constexpr size_t MEMORY_SIZE = 1024 * 1024;
    static constexpr size_t CACHE_SIZE = 64 * 1024;
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t NUM_CACHE_LINES = CACHE_SIZE / CACHE_LINE_SIZE;

    enum class AccessMode { Read, Write };

    enum class ReplacementPolicy { LRU, Random };

    MemoryController() : 
        main_memory_(std::make_unique<uint8_t[]>(MEMORY_SIZE)),
        cache_(NUM_CACHE_LINES),
        replacement_policy_(ReplacementPolicy::LRU),
        rng_(std::chrono::system_clock::now().time_since_epoch().count()) {
        std::cout << "Memory Controller initialized with " << MEMORY_SIZE << " bytes of main memory and "
                  << CACHE_SIZE << " bytes of cache." << std::endl;
    }

    void access(size_t address, AccessMode mode, uint8_t data = 0) {
        size_t cache_line = (address / CACHE_LINE_SIZE) % NUM_CACHE_LINES;
        size_t tag = address / CACHE_LINE_SIZE;
        size_t offset = address % CACHE_LINE_SIZE;

        bool cache_hit = false;
        for (auto& entry : cache_[cache_line]) {
            if (entry && entry->tag == tag) {
                cache_hit = true;
                updateLRU(entry);
                if (mode == AccessMode::Read) {
                    std::cout << "Cache hit! Read data: " << static_cast<int>(entry->data[offset]) << std::endl;
                } else {
                    entry->data[offset] = data;
                    entry->dirty = true;
                    std::cout << "Cache hit! Wrote data: " << static_cast<int>(data) << std::endl;
                }
                break;
            }
        }

        if (!cache_hit) {
            std::cout << "Cache miss. ";
            auto new_entry = std::make_shared<CacheLine>();
            new_entry->tag = tag;
            std::memcpy(new_entry->data.data(), &main_memory_[address - offset], CACHE_LINE_SIZE);

            if (cache_[cache_line].size() < 4) {  // 4-way set associative
                cache_[cache_line].push_back(new_entry);
            } else {
                auto victim = chooseCacheLineToEvict(cache_[cache_line]);
                if (victim->dirty) {
                    writeBackToMemory(victim, (tag * NUM_CACHE_LINES + cache_line) * CACHE_LINE_SIZE);
                }
                *victim = *new_entry;
            }

            if (mode == AccessMode::Read) {
                std::cout << "Read data: " << static_cast<int>(new_entry->data[offset]) << std::endl;
            } else {
                new_entry->data[offset] = data;
                new_entry->dirty = true;
                std::cout << "Wrote data: " << static_cast<int>(data) << std::endl;
            }
        }
    }

    void setReplacementPolicy(ReplacementPolicy policy) {
        replacement_policy_ = policy;
        std::cout << "Replacement policy set to " << (policy == ReplacementPolicy::LRU ? "LRU" : "Random") << std::endl;
    }

    void displayCacheStatus() const {
        std::cout << "\nCache Status:" << std::endl;
        for (size_t i = 0; i < NUM_CACHE_LINES; ++i) {
            std::cout << "Set " << i << ": ";
            for (const auto& entry : cache_[i]) {
                if (entry) {
                    std::cout << "[Tag: " << entry->tag << ", Dirty: " << (entry->dirty ? "Yes" : "No") << "] ";
                } else {
                    std::cout << "[Empty] ";
                }
            }
            std::cout << std::endl;
        }
    }

private:
    struct CacheLine {
        std::array<uint8_t, CACHE_LINE_SIZE> data;
        size_t tag;
        bool dirty = false;
        std::chrono::steady_clock::time_point last_used = std::chrono::steady_clock::now();
    };

    std::unique_ptr<uint8_t[]> main_memory_;
    std::vector<std::vector<std::shared_ptr<CacheLine>>> cache_;
    ReplacementPolicy replacement_policy_;
    std::mt19937 rng_;

    void updateLRU(std::shared_ptr<CacheLine>& entry) {
        entry->last_used = std::chrono::steady_clock::now();
    }

    std::shared_ptr<CacheLine> chooseCacheLineToEvict(std::vector<std::shared_ptr<CacheLine>>& cache_set) {
        if (replacement_policy_ == ReplacementPolicy::LRU) {
            return *std::min_element(cache_set.begin(), cache_set.end(),
                [](const auto& a, const auto& b) { return a->last_used < b->last_used; });
        } else {  // Random
            std::uniform_int_distribution<> dis(0, cache_set.size() - 1);
            return cache_set[dis(rng_)];
        }
    }

    void writeBackToMemory(const std::shared_ptr<CacheLine>& victim, size_t address) {
        std::memcpy(&main_memory_[address], victim->data.data(), CACHE_LINE_SIZE);
        std::cout << "Writing back dirty cache line to main memory at address " << address << std::endl;
    }
};

int main() {
    MemoryController mc;

    mc.access(0, MemoryController::AccessMode::Write, 42);
    mc.access(0, MemoryController::AccessMode::Read);
    mc.access(64, MemoryController::AccessMode::Write, 100);
    mc.access(128, MemoryController::AccessMode::Write, 200);
    mc.access(0, MemoryController::AccessMode::Read);

    mc.displayCacheStatus();

    // Change replacement policy
    mc.setReplacementPolicy(MemoryController::ReplacementPolicy::Random);

    mc.access(1024, MemoryController::AccessMode::Write, 50);
    mc.access(2048, MemoryController::AccessMode::Write, 75);
    mc.access(64, MemoryController::AccessMode::Read);

    mc.displayCacheStatus();

    return 0;
}
