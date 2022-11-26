#include "interrupthandler.hpp"


InterruptHandler::InterruptHandler() :
        interrupt_master_enable_flag(false) {}

void InterruptHandler::set_enable_flag(bool enable) {
    interrupt_master_enable_flag = enable;
}
