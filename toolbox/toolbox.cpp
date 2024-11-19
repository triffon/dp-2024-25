#include <iostream>
#include <vector>
#include <map>

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
protected:
    std::string name;
public:
    Item(std::string const& _name) : name(_name) {}
    virtual Item& operator[](size_t index) = 0;
    virtual void print() const {
        std::cout << name;
    }
    virtual bool isUnique() const = 0;

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

class UniqueTool : public Tool {
public:
    UniqueTool(std::string const& _name, std::string const& _type) : Tool(_name, _type) {}
    bool isUnique() const { return true; }
    ~UniqueTool() {
        std::cout << "Изтриване на уникален инструмент " << name << std::endl;
    }
};

class SharedTool : public Tool {
public:
    SharedTool(std::string const& _name, std::string const& _type) : Tool(_name, _type) {}
    bool isUnique() const { return false; }

    ~SharedTool() {
        std::cout << "Изтриване на споделен инструмент " << name << std::endl;
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
            if (item->isUnique())
                delete item;

    }

    bool isUnique() const { return true; }
};

class ToolFactory {
    std::map<std::string, Tool*> sharedTools;
    static size_t const NAME_THRESHOLD = 2;
public:
    Tool* createTool(std::string const& _name, std::string const& _type) {
        if (_name.size() <= NAME_THRESHOLD) {
            // използваме споделен обект 
            if (sharedTools.find(_name) == sharedTools.end())
                sharedTools[_name] = new SharedTool(_name, _type);
            return sharedTools[_name];
        } else
            // правим уникален обект
            return new UniqueTool(_name, _type);
    }
    ~ToolFactory() {
        for(auto& it : sharedTools)
            delete it.second;
    }
};

void testToolBox() {
    ToolFactory tf;
    Box b("Кутия1");
    b.addItem(tf.createTool("Оранжевите", "Клещи")).addItem(tf.createTool("Малката", "Отвертка"))
     .addItem(tf.createTool("10", "Ключ"))
     .addItem(tf.createTool("12", "Ключ"))
     .addItem(&(new Box("Кутия2"))->addItem(tf.createTool("Скъпият", "Фазомер"))
                                    .addItem(tf.createTool("10", "Ключ")))
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