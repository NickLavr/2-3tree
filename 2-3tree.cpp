#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

using std::string;
using std::swap;

template<class ValueType>
class Set {
 public:
    static const int ChildrenAmount = 4;
    struct Node {
        Node* sons[ChildrenAmount];
        Node* parent;
        ValueType MaxValue;
        size_t sonsSize;
        Node(const ValueType& value = ValueType(), Node* _parent = nullptr): MaxValue(value) {
            sonsSize = 0;
            for (int i = 0; i < ChildrenAmount; ++i) {
                sons[i] = nullptr;
            }
            parent = _parent;
        }
    };

    Set() {
        root = nullptr;
        amount = 0;
    }


    template<class It>
    Set(It begin, It end): Set() {
        for (auto it = begin; it != end; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<ValueType> initializer_list):
            Set(initializer_list.begin(), initializer_list.end()) {}

    ~Set() {
        DeleteTree(root);
        amount = 0;
    }

    Set(const Set& other): Set() {
        for (auto it = other.begin(); it != other.end(); ++it) {
            insert(*it);
        }
    }

    Set& operator=(Set other) {
        swap(root, other.root);
        swap(amount, other.amount);
        return *this;
    }

    void insert(const ValueType& value) {
        auto pos = find(root, value);
        if (pos == nullptr || (pos->MaxValue < value || value < pos->MaxValue)) {
            ++amount;
            insert(root, value);
        }
    }

    class iterator: public std::iterator<std::bidirectional_iterator_tag, ValueType>{
     public:
        iterator(Node* pos = nullptr, Node* _root = nullptr): position(pos), root(_root) {}
        iterator& operator++() {
            position = nextLeaf(position);
            return *this;
        }
        iterator& operator-- () {
            position = previousLeaf(position, root);
            return *this;
        }
        iterator operator++(int) {
            auto temp = *this;
            position = nextLeaf(position);
            return temp;
        }
        iterator operator--(int) {
            auto temp = *this;
            position = previousLeaf(position, root);
            return temp;
        }

        const ValueType& operator* () const {
            return position->MaxValue;
        }
        ValueType* operator-> () const {
            return &position->MaxValue;
        }

        bool operator==(const iterator& other) const {
            return position == other.position;
        }

        bool operator!=(const iterator& other) const {
            return position != other.position;
        }
        Node* GetLink() const {
            return position;
        }

     private:
        Node* position;
        Node* root;
    };

    iterator begin() const {
        Node* tree = root;
        if (root == nullptr) {
            return nullptr;
        }
        while (tree->sonsSize != 0) {
            tree = tree->sons[0];
        }
        return iterator(tree, root);
    }

    iterator end() const {
        return iterator(nullptr, root);
    }

    iterator find(const ValueType& value) const {
        auto pos = find(root, value);
        if (pos == nullptr || (pos->MaxValue < value || value < pos->MaxValue)) {
            return end();
        }
        return iterator(pos, root);
    }

    iterator lower_bound(const ValueType& value) const {
        auto pos = find(root, value);
        if (pos == nullptr || pos->MaxValue < value) {
            return end();
        }
        return iterator(pos, root);
    }

    void erase(const ValueType& value) {
        auto pos = find(value);
        if (pos != end()) {
            --amount;
            TreeErase(pos.GetLink());
        }
    }

    size_t size() const {
        return amount;
    }

    bool empty() const {
        return amount == 0;
    }

 private:
    void DeleteTree(Node* root) {
        if (root != nullptr) {
            for (int i = 0; i < root->sonsSize; ++i) {
                DeleteTree(root->sons[i]);
            }
            delete root;
        }
    }

    static Node* nextLeaf(Node* leaf) {
        while (leaf->parent != nullptr && getParentInex(leaf) == leaf->parent->sonsSize - 1) {
            leaf = leaf->parent;
        }
        if (leaf->parent == nullptr) {
            return nullptr;
        }
        leaf = leaf->parent->sons[getParentInex(leaf) + 1];
        while (leaf->sonsSize != 0) {
            leaf = leaf->sons[0];
        }
        return leaf;
    }

    static Node* previousLeaf(Node* leaf, Node* root) {
        if (leaf != nullptr) {
            while (leaf->parent != nullptr && getParentInex(leaf) == 0) {
                leaf = leaf->parent;
            }
            if (leaf->parent == nullptr) {
                return nullptr;
            }
            leaf = leaf->parent->sons[getParentInex(leaf) - 1];
        } else {
            leaf = root;
            if (root == nullptr) {
                return nullptr;
            }
        }
        while (leaf->sonsSize != 0) {
            leaf = leaf->sons[leaf->sonsSize - 1];
        }
        return leaf;
    }

    Node* find(Node* root, const ValueType& value) const {
        if (root == nullptr) {
            return nullptr;
        }
        if (root->sonsSize == 0) {
            return root;
        }
        for (int i = 0; i + 1 < root->sonsSize; ++i) {
            if (!(root->sons[i]->MaxValue < value)) {
                return find(root->sons[i], value);
            }
        }
        return find(root->sons[root->sonsSize - 1], value);
    }

    void checkSwap(Node* root, int i) {
        if (root->sons[i + 1]->MaxValue < root->sons[i]->MaxValue) {
            std::swap(root->sons[i], root->sons[i + 1]);
        }
    }

    void sortSons(Node* root) {
        if (root->sonsSize == 2) {
            checkSwap(root, 0);
        }
        if (root->sonsSize == 3) {
            checkSwap(root, 0);
            checkSwap(root, 1);
            checkSwap(root, 0);
        }
        if (root->sonsSize == 4) {
            checkSwap(root, 0);
            checkSwap(root, 1);
            checkSwap(root, 2);
            checkSwap(root, 0);
            checkSwap(root, 1);
            checkSwap(root, 0);
        }
    }

    static int getParentInex(Node *root) {
        int index = 0;
        for (; index < root->parent->sonsSize; ++index) {
            if (root->parent->sons[index] == root) {
                return index;
            }
        }
        return -1;
    }

    void fix(Node* root) {
        if (root == nullptr) {
            return;
        }
        if (root->sonsSize == 4) {
            if (root->parent == nullptr) {
                this->root = new Node(root->MaxValue);
                this->root->sons[0] = root;
                this->root->sonsSize = 1;
                root->parent = this->root;
            }
            Node* splitted = new Node();
            root->parent->sons[root->parent->sonsSize++] = splitted;
            splitted->parent = root->parent;

            splitted->sons[0] = root->sons[2];
            splitted->sons[0]->parent = splitted;
            splitted->sons[1] = root->sons[3];
            splitted->sons[1]->parent = splitted;
            splitted->sonsSize = root->sonsSize = 2;
            root->MaxValue = root->sons[root->sonsSize - 1]->MaxValue;
            splitted->MaxValue = splitted->sons[splitted->sonsSize - 1]->MaxValue;

            sortSons(root->parent);
            root->parent->MaxValue = root->parent->sons[root->parent->sonsSize - 1]->MaxValue;
            fix(root->parent);
        } else if (root->sonsSize == 1) {
            if (root->parent == nullptr) {
                Node* temp = this->root;
                this->root = root->sons[0];
                root->sons[0]->parent = nullptr;
                delete temp;
                return;
            }
            int parentIndex = getParentInex(root);
            swap(root->parent->sons[parentIndex], root->parent->sons[root->parent->sonsSize - 1]);
            --root->parent->sonsSize;
            sortSons(root->parent);
            int neighborIndex = 0;
            if (root->parent->sonsSize == 2) {
                if (root->parent->sons[0]->MaxValue < root->MaxValue) {
                    neighborIndex = 1;
                }
            }
            Node* neighbor = root->parent->sons[neighborIndex];
            neighbor->sons[neighbor->sonsSize++] = root->sons[0];
            neighbor->sons[neighbor->sonsSize - 1]->parent = neighbor;
            sortSons(neighbor);
            neighbor->MaxValue = neighbor->sons[neighbor->sonsSize - 1]->MaxValue;
            fix(neighbor);
            delete root;
        } else {
            root->MaxValue = root->sons[root->sonsSize - 1]->MaxValue;
            fix(root->parent);
        }
    }

    void insert(Node* &root, const ValueType& value) {
        if (root == nullptr) {
            root = new Node(value);
            return;
        }
        Node* place = find(root, value);
        if (place->parent == nullptr) {
            Node* newRoot = new Node();
            newRoot->sons[0] = place;
            newRoot->sons[1] = new Node(value);
            newRoot->sonsSize = 2;
            sortSons(newRoot);
            newRoot->MaxValue = newRoot->sons[newRoot->sonsSize - 1]->MaxValue;
            newRoot->sons[0]->parent = newRoot->sons[1]->parent = newRoot;
            root = newRoot;
            return;
        }
        place->parent->sons[place->parent->sonsSize++] = new Node(value, place->parent);
        sortSons(place->parent);
        place->parent->MaxValue = place->parent->sons[place->parent->sonsSize - 1]->MaxValue;
        fix(place->parent);
    }

    void TreeErase(Node* tree) {
        if (tree->parent == nullptr) {
            delete tree;
            this->root = nullptr;
            return;
        }
        int index = getParentInex(tree);
        swap(tree->parent->sons[index], tree->parent->sons[tree->parent->sonsSize - 1]);
        --tree->parent->sonsSize;
        sortSons(tree->parent);
        tree->parent->MaxValue = tree->parent->sons[tree->parent->sonsSize - 1]->MaxValue;
        fix(tree->parent);
        delete tree;
    }

    Node* root;
    size_t amount;
};
