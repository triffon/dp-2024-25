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

class Component {
    std::string name;
public:
    Component(std::string const& _name) : name(_name) {}
    virtual Component& operator[](size_t index) = 0;
    virtual void print() const {
        std::cout << name;
    }
    virtual ~Component() = default;
};

class Tool : public Component {
    std::string type;
public:
    Tool(std::string const& _name, std::string const& _type) : Component(_name), type(_type) {}
    void print() const {
        Component::print();
        std::cout << ' ' << type;
    }

    Component& operator[](size_t) {
        return *this;
    }
};

class Box : public Component {
    std::vector<Component*> children;
public:
    Box(std::string const& name) : Component(name) {}
    Box(Box const&) = delete;
    Box& operator=(Box const&) = delete;

    Box& addComponent(Component* comp) {
        children.push_back(comp);
        return *this;
    }

    void print() const {
        Component::print();
        std::cout << ", съдържаща: {" << std::endl;
        for(Component* comp : children) {
            comp->print();
            std::cout << std::endl;
        }
        std::cout << "}";
    }

    Component& operator[](size_t index) {
        return *children[index];
    }

    ~Box() {
        for(Component* comp : children)
            delete comp;
    }
};

void testToolBox() {
    Box b("Кутия1");
    b.addComponent(new Tool("Оранжевите", "Клещи")).addComponent(new Tool("Малката", "Отвертка"))
     .addComponent(&(new Box("Кутия2"))->addComponent(new Tool("Скъпият", "Фазомер")))
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