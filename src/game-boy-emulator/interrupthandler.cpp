#include "interrupthandler.hpp"


void InterruptHandler::set_enable_flag(bool enable) {
    interrupt_master_enable_flag = enable;
}
