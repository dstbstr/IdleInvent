#pragma once

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

// Simple Wrapper around a sale
template<typename T>
struct Sale {
    T Item;
};

template<typename T>
void SellAll(std::vector<T>& items) {
    auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<T>>>();
    for(const auto& e : items) {
        pubSub.Publish({e});
    }
    items.clear();
}

template<typename T>
void Sell(std::vector<T>& items, size_t index) {
    auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<T>>>();
    if(index < items.size()) {
        pubSub.Publish({items[index]});
        items.erase(items.begin() + index);
    }
}