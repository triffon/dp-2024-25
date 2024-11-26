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

class Indent {
  size_t indent;
  static char const INDENT_CHAR = ' ';
  static size_t const INDENT_OFFSET = 2;
public:
    Indent(size_t _indent = 0) : indent(_indent) {}
    void printIndent() const {
        std::cout << std::string(indent, INDENT_CHAR);
    }
    Indent offset() const {
        return Indent(indent + INDENT_OFFSET);
    }
};

class Item;

class Product {
    Item* item;
public:
    Product(Item* _item);
    virtual ~Product();
    virtual void print(Indent indent) const;
    virtual void print() const;
    void setIndent(Indent indent);
    bool isUnique() const;
    Item* getItem() { return item; }
};

class Item {
protected:
    std::string name;
public:
    Item(std::string const& _name) : name(_name) {}
    virtual Item& operator[](size_t index) = 0;
    virtual void print(Indent const& indent) const {
        indent.printIndent();
        std::cout << name;
    }
    void print() const {
        print(Indent());
    }
    virtual void setIndent(Indent) = 0;
    virtual bool isUnique() const = 0;

    virtual ~Item() = default;
};

class Tool : public Item {
    std::string type;
public:
    Tool(std::string const& _name, std::string const& _type) : Item(_name), type(_type) {}
    void print(Indent const& indent) const {
        Item::print(indent);
        std::cout << ' ' << type;
    }

    Item& operator[](size_t) {
        return *this;
    }

};

class UniqueTool : public Tool {
    Indent indent;
public:
    UniqueTool(std::string const& _name, std::string const& _type) :
        Tool(_name, _type) {}
    void setIndent(Indent _indent) {
        indent = _indent;
    }
    void print(Indent const&) const {
        Tool::print(indent);
    }

    bool isUnique() const { return true; }
    ~UniqueTool() {
        std::cout << "Изтриване на уникален инструмент " << name << std::endl;
    }
};

class SharedTool : public Tool {
public:
    SharedTool(std::string const& _name, std::string const& _type) : Tool(_name, _type) {}
    bool isUnique() const { return false; }
    void setIndent(Indent) {}

    ~SharedTool() {
        std::cout << "Изтриване на споделен инструмент " << name << std::endl;
    }
};

class ChildrenPrintStrategy {
public:
    virtual void print(Indent indent, std::vector<Product*> const& children) const = 0;
    virtual ~ChildrenPrintStrategy() = default;
};

class OneLinePrintStrategy : public ChildrenPrintStrategy {
public:
    virtual void print(Indent, std::vector<Product*> const& children) const {
        std::cout << ", съдържаща: { ";
        for(Product* product : children) {
            // двоен отстъп за споделените инструменти
            product->print();
            std::cout << ", ";
        }
        std::cout << " }";
    }
};



class IndentedPrintStrategy : public ChildrenPrintStrategy {
public:
    virtual void print(Indent indent, std::vector<Product*> const& children) const {
        std::cout << ", съдържаща: {" << std::endl;
        for(Product* product : children) {
            // двоен отстъп за споделените инструменти
            product->print(indent.offset().offset());
            std::cout << std::endl;
        }
        indent.printIndent();
        std::cout << "}";
    }
};

class NullPrintStrategy : public ChildrenPrintStrategy {
public:
    virtual void print(Indent, std::vector<Product*> const&) const {}
};


class Box : public Item {
    std::vector<Product*> children;
    Indent indent;
    ChildrenPrintStrategy* childrenPrintStrategy;
public:
    Box(std::string const& name) : Item(name), childrenPrintStrategy(new NullPrintStrategy()) {}
    Box(Box const&) = delete;
    Box& operator=(Box const&) = delete;

    Box& addProduct(Product* product) {
        product->setIndent(indent.offset());
        children.push_back(product);
        return *this;
    }
    void setIndent(Indent _indent) {
        indent = _indent;
        for(Product* product : children)
            product->setIndent(indent.offset());
    }

    void print(Indent const&) const {
        Item::print(indent);
        childrenPrintStrategy->print(indent, children);
    }

    Item& operator[](size_t index) {
        return *(children[index]->getItem());
    }

    void setStrategy(ChildrenPrintStrategy* newStrategy) {
        delete childrenPrintStrategy;
        childrenPrintStrategy = newStrategy;
    }

    ~Box() {
        for(Product* product : children)
            if (product->isUnique())
                delete product;
        delete childrenPrintStrategy;
    }

    bool isUnique() const { return true; }
};

class BrandedProduct : public Product {
    std::string brand;
public:
    BrandedProduct(Item* _item, std::string const& _brand) : Product(_item), brand(_brand) {}
    void print(Indent indent) const {
        Product::print(indent);
        std::cout << " [" << brand << "]";
    }
};

class ToolFactory {
    std::map<std::string, Product*> sharedTools;
    static size_t const NAME_THRESHOLD = 2;

    Product* createProduct(Tool* _tool, std::string const& _brand) {
        if (_brand != "")
            return new BrandedProduct(_tool, _brand);
        return new Product(_tool);  
    }
public:
    Product* createTool(std::string const& _name, std::string const& _type, std::string const& _brand = "") {
        if (_name.size() <= NAME_THRESHOLD) {
            // използваме споделен обект 
            if (sharedTools.find(_name) == sharedTools.end())
                sharedTools[_name] = createProduct(new SharedTool(_name, _type), _brand);
            return sharedTools[_name];
        } else
            // правим уникален обект
            return createProduct(new UniqueTool(_name, _type), _brand);
    }
    ~ToolFactory() {
        for(auto& it : sharedTools)
            delete it.second;
    }
};

inline Product::Product(Item* _item) : item(_item) {}

inline Product::~Product() {
    delete item;
}

inline void Product::print(Indent indent) const {
    item->print(indent);
}

inline void Product::print() const {
    item->print();
}

inline void Product::setIndent(Indent indent) {
    item->setIndent(indent);
}

inline bool Product::isUnique() const {
    return item->isUnique();
}

void testToolBox() {
    ToolFactory tf;
    Box b("Кутия1");
    b.addProduct(tf.createTool("Оранжевите", "Клещи", "ToolMaster")).addProduct(tf.createTool("Малката", "Отвертка"))
     .addProduct(tf.createTool("10", "Ключ"))
     .addProduct(tf.createTool("12", "Ключ"))
     .addProduct(new BrandedProduct(&(new Box("Кутия2"))->addProduct(tf.createTool("Скъпият", "Фазомер"))
                                    .addProduct(tf.createTool("10", "Ключ")), "Bosch"));
    b.Item::print();
    std::cout << std::endl;

    b.setStrategy(new IndentedPrintStrategy());
    b.Item::print();
    std::cout << std::endl;

    dynamic_cast<Box&>(b[4]).setStrategy(new OneLinePrintStrategy());
    b.Item::print();
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