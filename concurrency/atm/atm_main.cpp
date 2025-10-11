#include "teller.h"

#include <thread>

using namespace ccy;

int main() {
    std::cout << ">>>>>> ATM <<<<<<" << std::endl;

    BankMaching bank;
    InterfaceMachine interface_hardware;
    Atm machine(bank.getSender(), interface_hardware.getSender());

    std::thread bank_thread(&BankMaching::run, &bank);
    std::thread if_thread(&InterfaceMachine::run, &interface_hardware);
    std::thread atm_thread(&Atm::run, &machine);

    Sender atmqueue(machine.getSender());
    bool quit_pressed = false;
    while (!quit_pressed) {
        char c = getchar();
        switch (c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                atmqueue.send(DigitPressed(c));
                break;
            case 'b':
                atmqueue.send(BalancePressed());
                break;
            case 'w':
                atmqueue.send(Withdraw_pressed(50));
                break;
            case 'c':
                atmqueue.send(CancelPressed());
                break;
            case 'q':
                quit_pressed = true;
                break;
            case 'i':
                atmqueue.send(CardInserted("account123456"));
                break;
        }
    }

    bank.done();
    machine.done();
    interface_hardware.done();

    bank_thread.join();
    if_thread.join();
    atm_thread.join();

    return 0;
}