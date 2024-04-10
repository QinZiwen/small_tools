#pragma once

#include <string>

#include "message.h"
#include "sender_receiver.h"

namespace ccy {

struct Withdraw {
    std::string account;
    unsigned int amount;
    mutable Sender atm_queue;

    Withdraw(const std::string& account_, unsigned int amount_, Sender atm_queue_)
        : account(account_), amount(amount_), atm_queue(atm_queue_) {}
};

struct Withdraw_ok {};
struct Withdraw_denied {};
struct Withdraw_cancel {
    std::string account;
    unsigned int amount;
    Withdraw_cancel(const std::string& account_, unsigned int amount_)
        : account(account_), amount(amount_) {}
};
struct Withdraw_processed {
    std::string account;
    unsigned int amount;
    Withdraw_processed(const std::string& account_, unsigned int amount_)
        : account(account_), amount(amount_) {}
};
struct Withdraw_pressed {
    unsigned int amount;
    Withdraw_pressed(unsigned int amount_) : amount(amount_) {}
};

struct CardInserted {
    std::string account;
    explicit CardInserted(const std::string& account_) : account(account_) {}
};

struct DigitPressed {
    char digit;
    explicit DigitPressed(char digit_) : digit(digit_) {}
};

struct ClearLastPressed {};
struct EjectCard {};

struct CancelPressed {};

struct IssueMoney {
    unsigned int amount;
    IssueMoney(unsigned int amount_) : amount(amount_) {}
};

struct VerifyPin {
    std::string account;
    std::string pin;
    mutable Sender atm_queue;
    VerifyPin(const std::string& account_, const std::string& pin_, Sender atm_queue_)
        : account(account_), pin(pin_), atm_queue(atm_queue_)
    {}
};

struct PinVerified {};
struct PinIncorrect {};
struct DisplayEnterPin {};
struct DisplayEnterCard {};
struct DisplayInsufficientFunds {};
struct DisplayWithdrawalCancelled {};
struct DisplayPinIncorrectMessage {};
struct DisplayWithdrawalOptions {};

struct GetBalance {
    std::string account;
    mutable Sender atm_queue;
    GetBalance(std::string account_, Sender atm_queue_)
        : account(account_), atm_queue(atm_queue_)
    {}
};

struct Balance {
    unsigned int amount;
    explicit Balance(unsigned int amount_) : amount(amount_) {}
};

struct DisplayBalance {
    unsigned int amount;
    explicit DisplayBalance(unsigned int amount_) : amount(amount_) {}
};

struct BalancePressed {};

class Atm {
public:
    Atm() {
        std::cout << "[Atm::Atm]" << std::endl;
    }
    Atm(const Atm&) = delete;
    Atm operator=(const Atm&) = delete;

    Atm(Sender bank, Sender interface_hardware) : m_bank(bank), m_interface_hardware(interface_hardware) {}

    void done() {
        getSender().send(CloseQueue());
    }

    void run() {
        std::cout << ">>>>>> [Atm::run] <<<<<<" << std::endl;
        state = &Atm::waitingForCard;
        try {
            while (true) {
                (this->*state)();
            }
        } catch (const CloseQueue&) {}

        std::cout << ">>>>>> [Atm::run] done <<<<<<" << std::endl;
    }

    Sender getSender() {
        std::cout << "[Atm::getSender]" << std::endl;
        return m_incoming;
    }

private:
    void precessWithdrawal () {
        m_incoming.wait()
            .handle<Withdraw_ok>([&](const Withdraw_ok& msg) {
                m_interface_hardware.send(IssueMoney(m_withdrawal_amount));
                m_bank.send(Withdraw_processed(m_account, m_withdrawal_amount));
            })
            .handle<Withdraw_denied>([&](const Withdraw_denied& msg) {
                m_interface_hardware.send(DisplayInsufficientFunds());
                state = &Atm::doneProcessing;
            })
            .handle<CancelPressed>([&](const CancelPressed& msg) {
                m_bank.send(Withdraw_cancel(m_account, m_withdrawal_amount));
                m_interface_hardware.send(DisplayWithdrawalCancelled());
                state = &Atm::doneProcessing;
            });
    }

    void precessBalance () {
        std::cout << "[Atm::precessBalance]" << std::endl;
    }

    void waitForAction() {
        std::cout << "[Atm::waitForAction]" << std::endl;
    }

    void verifyingPin() {
        std::cout << "[Atm::verifyingPin]" << std::endl;
    }

    void gettingPin() {
        std::cout << "[Atm::gettingPin]" << std::endl;
        m_incoming.wait()
            .handle<DigitPressed>([&](const DigitPressed& msg) {
                std::cout << "[Atm::gettingPin] digit: " << msg.digit << std::endl;
            });
    }

    void waitingForCard() {
        std::cout << "[Atm::waitingForCard]" << std::endl;
        m_incoming.wait()
            .handle<CardInserted>([&](const CardInserted& msg) {
                m_account = msg.account;
                m_interface_hardware.send(DisplayEnterPin());
                state = &Atm::gettingPin;
                std::cout << "[Atm::waitingForCard] CardInserted m_account: " << m_account << std::endl;
            });
    }

    void doneProcessing() {
        std::cout << "[Atm::doneProcessing]" << std::endl;
    }

private:
    Receiver m_incoming;
    Sender m_bank;
    Sender m_interface_hardware;
    std::string m_account;
    unsigned int m_withdrawal_amount;
    std::string m_pin;

    void (Atm::*state)();
};

class BankMaching {
public:
    BankMaching() : m_balance(199) {
        std::cout << "[BankMaching::BankMaching]" << std::endl;
    }

    void done() {
        std::cout << "[BankMaching::done]" << std::endl;
        getSender().send(CloseQueue());
    }

    void run() {
        std::cout << ">>>>>> [BankMaching::run] <<<<<<" << std::endl;
        try {
            while(true) {
                m_incoming.wait()
                    .handle<VerifyPin>([&](const VerifyPin& msg) {
                        if (msg.pin == "1937") {
                            msg.atm_queue.send(PinVerified());
                        } else {
                            msg.atm_queue.send(PinIncorrect());
                        }
                    })
                    .handle<Withdraw>([&](const Withdraw& msg) {
                        if (m_balance > msg.amount) {
                            msg.atm_queue.send(Withdraw_ok());
                            m_balance -= msg.amount;
                        } else {
                            msg.atm_queue.send(Withdraw_denied());
                        }
                    });
            }
        } catch (const CloseQueue&) {}

        std::cout << ">>>>>> [BankMaching::run] done <<<<<<" << std::endl;
    }

    Sender getSender() {
        std::cout << "[BankMaching::getSender]" << std::endl;
        return m_incoming;
    }

private:
    Receiver m_incoming;
    unsigned int m_balance;
};

class InterfaceMachine {
public:
    InterfaceMachine() {
        std::cout << "[InterfaceMachine::InterfaceMachine]" << std::endl;
    }

    void done() {
        getSender().send(CloseQueue());
    }

    void run() {
        std::cout << ">>>>>> [InterfaceMachine::run] <<<<<<" << std::endl;
        try {
            while (true) {
                m_incoming.wait()
                    .handle<IssueMoney>([&](const IssueMoney& msg) {
                        std::lock_guard<std::mutex> lk(m_iom);
                        std::cout << "[InterfaceMachine::run] IssueMoney: " << msg.amount << std::endl;
                    });
            }
        } catch (const CloseQueue&) {}
        std::cout << ">>>>>> [InterfaceMachine::run] done <<<<<<" << std::endl;
    }

    Sender getSender() {
        std::cout << "[InterfaceMachine::getSender]" << std::endl;
        return m_incoming;
    }

private:
    Receiver m_incoming;
    mutable std::mutex m_iom;
};

}  // namespace ccy