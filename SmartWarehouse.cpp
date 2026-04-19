#include <iostream>
#include <string>
using namespace std;

// ==========================================
// 1. Linked List - For Customer Orders (Dynamic Size)
// ==========================================
struct OrderNode {
    string customerName;
    string requestedProduct;
    int quantity;
    OrderNode* next;
};

OrderNode* head = NULL;
OrderNode* tail = NULL;

void addOrder(string customer, string product, int qty) {
    OrderNode* newNode = new OrderNode();
    newNode->customerName = customer;
    newNode->requestedProduct = product;
    newNode->quantity = qty;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    cout << "[Customer] Placed Order: " << customer << " requested " << qty << "x '" << product << "'" << endl;
}

// Just returns the head order to be processed
OrderNode* getNextOrder() {
    return head;
}

void removeProcessedOrder() {
    if (head != NULL) {
        OrderNode* temp = head;
        head = head->next;
        if (head == NULL) tail = NULL;
        delete temp;
    }
}

// Delete from Linked List by Customer Name
void cancelOrder(string targetCustomer) {
    if (head == NULL) {
        cout << "[System] No orders to cancel." << endl;
        return;
    }

    if (head->customerName == targetCustomer) {
        OrderNode* temp = head;
        head = head->next;
        if (head == NULL) tail = NULL;
        delete temp;
        cout << "[System] Order for '" << targetCustomer << "' cancelled successfully." << endl;
        return;
    }
    
    OrderNode* current = head;
    OrderNode* prev = NULL;
    
    while (current != NULL && current->customerName != targetCustomer) {
        prev = current;
        current = current->next;
    }
    
    if (current == NULL) {
        cout << "[System] Order for '" << targetCustomer << "' not found." << endl;
        return;
    }
    
    prev->next = current->next;
    if (current == tail) tail = prev;
    delete current;
    cout << "[System] Order for '" << targetCustomer << "' cancelled successfully." << endl;
}

void displayOrders() {
    if (head == NULL) {
        cout << "No pending orders." << endl;
        return;
    }
    cout << "Pending Orders: ";
    OrderNode* current = head;
    while (current != NULL) {
        cout << "[" << current->customerName << ": " << current->quantity << "x " << current->requestedProduct << "] -> ";
        current = current->next;
    }
    cout << "NULL" << endl;
}

// ==========================================
// 2. Standard Binary Tree - For Warehouse Zones & Inventory
// ==========================================
struct ZoneNode {
    string zoneName;
    string productName;
    int quantity;
    ZoneNode* left;
    ZoneNode* right;
};

ZoneNode* root = NULL;

ZoneNode* createNode(string zName, string pName, int qty) {
    ZoneNode* newNode = new ZoneNode();
    newNode->zoneName = zName;
    newNode->productName = pName;
    newNode->quantity = qty;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void addZoneRecursive(ZoneNode* current, string zName, string pName, int qty) {
    char choice;
    cout << "At Zone: [" << current->zoneName << "]" << endl;
    cout << "Add new zone to (L)eft or (R)ight? ";
    cin >> choice;
    
    if (choice == 'L' || choice == 'l') {
        if (current->left == NULL) {
            current->left = createNode(zName, pName, qty);
            cout << "Zone added to the Left." << endl;
        } else {
            cout << "Left occupied. Moving down..." << endl;
            addZoneRecursive(current->left, zName, pName, qty);
        }
    } 
    else if (choice == 'R' || choice == 'r') {
        if (current->right == NULL) {
            current->right = createNode(zName, pName, qty);
            cout << "Zone added to the Right." << endl;
        } else {
            cout << "Right occupied. Moving down..." << endl;
            addZoneRecursive(current->right, zName, pName, qty);
        }
    } 
    else {
        cout << "Invalid choice. Aborting..." << endl;
    }
}

void addZone(string zName, string pName, int qty) {
    if (root == NULL) {
        root = createNode(zName, pName, qty);
        cout << "Root Zone created." << endl;
    } else {
        addZoneRecursive(root, zName, pName, qty);
    }
}

// Recursive search for a product inside the Warehouse Zones
ZoneNode* searchProduct(ZoneNode* node, string pName) {
    if (node == NULL) return NULL;
    
    // Check if current zone has the product and enough quantity
    if (node->productName == pName && node->quantity >= 0) {
        return node;
    }
    
    // Check Left Branch
    ZoneNode* leftResult = searchProduct(node->left, pName);
    if (leftResult != NULL) return leftResult;
    
    // Check Right Branch
    return searchProduct(node->right, pName);
}

// Recursive deletion of a whole sub-tree
void deleteSubTree(ZoneNode* node) {
    if (node == NULL) return;
    deleteSubTree(node->left);
    deleteSubTree(node->right);
    delete node;
}

// Delete Zone from Binary Tree recursively checking children
bool removeZone(ZoneNode*& node, string zName) {
    if (node == NULL) return false;
    
    if (node->left != NULL && node->left->zoneName == zName) {
        deleteSubTree(node->left);
        node->left = NULL;
        return true;
    }
    if (node->right != NULL && node->right->zoneName == zName) {
        deleteSubTree(node->right);
        node->right = NULL;
        return true;
    }
    
    if (removeZone(node->left, zName)) return true;
    if (removeZone(node->right, zName)) return true;
    
    return false;
}

void displayWarehouseMap(ZoneNode* node, int depth = 0) {
    if (node == NULL) return;
    for (int i = 0; i < depth; i++) cout << "   |-- ";
    cout << node->zoneName << " [Item: " << node->productName << " | Qty: " << node->quantity << "]" << endl;
    
    displayWarehouseMap(node->left, depth + 1);
    displayWarehouseMap(node->right, depth + 1);
}

// ==========================================
// 3. Main Program & Fulfillment Logic
// ==========================================
void processNextOrder() {
    OrderNode* order = getNextOrder();
    if (order == NULL) {
        cout << "Error: No pending orders to process!" << endl;
        return;
    }
    
    cout << "[Manager] Handling order for Customer: " << order->customerName << " (Item: " << order->requestedProduct << ", Qty: " << order->quantity << ")" << endl;
    
    // Search the warehouse for the item
    ZoneNode* foundBin = searchProduct(root, order->requestedProduct);
    
    if (foundBin != NULL) {
        if (foundBin->quantity >= order->quantity) {
            foundBin->quantity -= order->quantity; // Deduct inventory
            cout << "[Manager] SUCCESS! Found stock in Zone '" << foundBin->zoneName << "'. Remaining Qty: " << foundBin->quantity << endl;
            cout << "[Manager] Shipment Dispatched to " << order->customerName << "!" << endl;
        } else {
            cout << "[Manager] ERROR! Not enough stock. Available: " << foundBin->quantity << ", Requested: " << order->quantity << endl;
            cout << "[Manager] Partial fulfillment or cancellation required." << endl;
            // For simplicity in this simulation, we still remove it or keep it? User said "process next order".
            // Let's assume it fails if not enough stock.
        }
    } else {
        cout << "[Manager] ERROR! Item '" << order->requestedProduct << "' is out of stock or not found in warehouse!" << endl;
    }
    
    // Remove order from waitlist regardless (handled or failed)
    removeProcessedOrder();
}


int main() {
    int choice;
    string cName, pName, zName;
    int qty;

    // Optional: Pre-fill warehouse with some data to make testing faster
    // root = createNode("MainHub", "None", 0);
    // root->left = createNode("Electronics", "Laptop", 5);

    while (true) {
        cout << "\n--- AMAZON WAREHOUSE SIMULATION ---" << endl;
        cout << "1. Receive Shipment (Add to Binary Tree)" << endl;
        cout << "2. Display Warehouse Inventory & Zones" << endl;
        cout << "3. Remove a Zone & its Stock (Delete from Tree)" << endl;
        cout << "4. Customer Order Placement (Add to Linked List)" << endl;
        cout << "5. Manager: Process Next Order" << endl;
        cout << "6. Cancel a Customer Order (Delete from List)" << endl;
        cout << "7. Show Pending Orders" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter Zone Name (e.g. Electronics): "; cin >> zName;
            cout << "Enter Product Name (e.g. Laptop): "; cin >> pName;
            cout << "Enter Quantity: "; cin >> qty;
            addZone(zName, pName, qty);
        } 
        else if (choice == 2) {
            cout << "\n--- Warehouse Inventory Layout ---" << endl;
            displayWarehouseMap(root);
            cout << "----------------------------------" << endl;
        } 
        else if (choice == 3) {
            cout << "Enter Zone Name to Remove: "; cin >> zName;
            if (root != NULL && root->zoneName == zName) {
                cout << "[System] Cannot remove the Root Main Entrance securely from here!" << endl;
            } else {
                bool deleted = removeZone(root, zName);
                if (deleted) cout << "[System] Zone '" << zName << "' and its items removed successfully." << endl;
                else cout << "[System] Zone not found." << endl;
            }
        }
        else if (choice == 4) {
            cout << "Enter Customer Name: "; cin >> cName;
            cout << "Enter Product to order: "; cin >> pName;
            cout << "Enter Quantity to order: "; cin >> qty;
            
            // Check inventory before accepting order
            ZoneNode* foundBin = searchProduct(root, pName);
            if (foundBin != NULL && foundBin->quantity >= qty) {
                addOrder(cName, pName, qty);
            } else if (foundBin != NULL) {
                cout << "[System] ❌ Cannot accept order! Not enough stock. Available: " << foundBin->quantity << endl;
            } else {
                cout << "[System] ❌ Cannot accept order! Product '" << pName << "' not found in the warehouse." << endl;
            }
        } 
        else if (choice == 5) {
            processNextOrder();
        } 
        else if (choice == 6) {
            cout << "Enter Customer Name to Cancel Order: "; cin >> cName;
            cancelOrder(cName);
        }
        else if (choice == 7) {
            displayOrders();
        }
        else if (choice == 8) {
            break;
        } 
    }
    return 0;
}