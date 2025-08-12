#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
using namespace std;

/* ============================
   Logger (Singleton)
   ============================ */
class Logger {
private:
    Logger() {}
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& instance() {
        static Logger inst;
        return inst;
    }

    void info(const string &msg) {
        cout << "[INFO] " << msg << endl;
    }
};

/* ============================
   Product and Slot
   ============================ */
struct Product {
    int id;
    string name;
    double price;
    Product() = default;
    Product(int id_, const string &name_, double price_) : id(id_), name(name_), price(price_) {}
};

struct Slot {
    Product product;
    int quantity;
    Slot() = default;
    Slot(const Product &p, int q): product(p), quantity(q) {}
};

/* ============================
   Inventory
   ============================ */
class Inventory {
private:
    unordered_map<int, Slot> slots; // key = slot id
public:
    void addSlot(int slotId, const Product &p, int quantity) {
        slots[slotId] = Slot(p, quantity);
    }

    bool hasStock(int slotId) {
        return slots.count(slotId) && slots[slotId].quantity > 0;
    }

    Product getProduct(int slotId) {
        if (!slots.count(slotId)) throw runtime_error("Invalid slot");
        return slots[slotId].product;
    }

    bool consumeOne(int slotId) {
        if (!hasStock(slotId)) return false;
        slots[slotId].quantity--;
        return true;
    }

    int getQuantity(int slotId) {
        if (!slots.count(slotId)) return 0;
        return slots[slotId].quantity;
    }
};

/* ============================
   Payment Strategy
   ============================ */
class PaymentStrategy {
public:
    virtual bool pay(double amount) = 0;
    virtual string name() const = 0;
    virtual ~PaymentStrategy() {}
};

class CashPayment : public PaymentStrategy {
public:
    bool pay(double amount) override {
        Logger::instance().info("Cash payment of " + to_string(amount));
        return true;
    }
    string name() const override { return "Cash"; }
};

class CardPayment : public PaymentStrategy {
public:
    bool pay(double amount) override {
        Logger::instance().info("Card payment of " + to_string(amount));
        return true;
    }
    string name() const override { return "Card"; }
};

/* ============================
   Payment Service
   ============================ */
class PaymentService {
public:
    bool processPayment(PaymentStrategy &strategy, double amount) {
        Logger::instance().info("Using " + strategy.name() + " to pay Rs " + to_string(amount));
        return strategy.pay(amount);
    }
};

/* ============================
   Vending Machine (Singleton)
   ============================ */
class VendingMachine {
private:
    Inventory inventory;
    PaymentService paymentService;
    VendingMachine() {}
public:
    VendingMachine(const VendingMachine&) = delete;
    VendingMachine& operator=(const VendingMachine&) = delete;

    static VendingMachine& instance() {
        static VendingMachine vm;
        return vm;
    }

    void loadProduct(int slotId, const Product &p, int quantity) {
        inventory.addSlot(slotId, p, quantity);
        Logger::instance().info("Loaded " + p.name + " in slot " + to_string(slotId));
    }

    void displayItems() {
        Logger::instance().info("Available items:");
        for (int i = 1; i <= 10; ++i) {
            int q = inventory.getQuantity(i);
            if (q > 0) {
                Product p = inventory.getProduct(i);
                cout << "Slot " << i << ": " << p.name << " (Rs " << p.price << "), Qty: " << q << endl;
            }
        }
    }

    bool buy(int slotId, PaymentStrategy &strategy) {
        if (!inventory.hasStock(slotId)) {
            cout << "Item not available\n";
            return false;
        }

        Product prod = inventory.getProduct(slotId);
        cout << "Selected: " << prod.name << " (Rs " << prod.price << ")\n";

        if (!paymentService.processPayment(strategy, prod.price)) {
            cout << "Payment failed.\n";
            return false;
        }

        inventory.consumeOne(slotId);
        cout << "Dispensing " << prod.name << "\n";
        return true;
    }
};

/* ============================
   Demo main()
   ============================ */
int main() {
    auto &vm = VendingMachine::instance();

    // Load products
    vm.loadProduct(1, Product(101, "Chips", 30.0), 5);
    vm.loadProduct(2, Product(102, "Coke", 50.0), 3);

    vm.displayItems();

    CashPayment cash;
    CardPayment card;

    cout << "\nBuying Chips with Cash:\n";
    vm.buy(1, cash);

    cout << "\nBuying Coke with Card:\n";
    vm.buy(2, card);

    return 0;
}
