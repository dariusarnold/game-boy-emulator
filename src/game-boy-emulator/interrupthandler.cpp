#include "interrupthandler.hpp"


InterruptHandler::InterruptHandler() :
        interrupt_master_enable_flag(false),
        interrupt_flags(0xFF0F, 0xFF0F),
        interrupt_enable(0xFFFF, 0xFFFF) {}

std::vector<IMemoryRange*> InterruptHandler::get_mappable_memory() {
    return {&interrupt_enable, &interrupt_flags};
}

void InterruptHandler::set_enable_flag(bool enable) {
    interrupt_master_enable_flag = enable;
}
