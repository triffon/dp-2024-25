#include <iostream>
#include <vector>

class IntTree {
private:
    int data;
    std::vector<IntTree> children;
public:
    IntTree(int _data) : data(_data) {}
    IntTree& addChild(IntTree && it) {
        children.push_back(std::move(it));
        return *this;
    }
    void print() const {
        std::cout << data << std::endl;
        for(IntTree const& it : children)
            it.print();
    }
};

void testIntTree() {
    IntTree t1(3);
    t1.addChild(IntTree(1)).addChild(IntTree(2));
    IntTree t2(7);
    t2.addChild(IntTree(4)).addChild(IntTree(5)).addChild(IntTree(6));
    IntTree t(8);
    t.addChild(std::move(t1)).addChild(std::move(t2));
    t.print();
}

class Item {
    std::string name;
public:
    Item(std::string const& _name) : name(_name) {}
    virtual Item& operator[](size_t index) = 0;
    virtual void print() const {
        std::cout << name;
    }
    virtual ~Item() = default;
};

class Tool : public Item {
    std::string type;
public:
    Tool(std::string const& _name, std::string const& _type) : Item(_name), type(_type) {}
    void print() const {
        Item::print();
        std::cout << ' ' << type;
    }

    Item& operator[](size_t) {
        return *this;
    }
};

class Box : public Item {
    std::vector<Item*> children;
public:
    Box(std::string const& name) : Item(name) {}
    Box(Box const&) = delete;
    Box& operator=(Box const&) = delete;

    Box& addItem(Item* item) {
        children.push_back(item);
        return *this;
    }

    void print() const {
        Item::print();
        std::cout << ", съдържаща: {" << std::endl;
        for(Item* item : children) {
            item->print();
            std::cout << std::endl;
        }
        std::cout << "}";
    }

    Item& operator[](size_t index) {
        return *children[index];
    }

    ~Box() {
        for(Item* item : children)
            delete item;
    }
};

void testToolBox() {
    Box b("Кутия1");
    b.addItem(new Tool("Оранжевите", "Клещи")).addItem(new Tool("Малката", "Отвертка"))
     .addItem(&(new Box("Кутия2"))->addItem(new Tool("Скъпият", "Фазомер")))
     .print();
    std::cout << std::endl;
    b[2][0].print();
    std::cout << std::endl;
    b[1][1][1][1][1][1].print();
    std::cout << std::endl;
}

int main() {
    // testIntTree();
    testToolBox();
    return 0;
}