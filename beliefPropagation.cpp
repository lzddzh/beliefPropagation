#include<iostream>
#include<vector>
#include<map>
using namespace std;

// PsiNode is the node potential function.
struct PsiNode {
    // sum of the values is 1.
    vector<double> values;
};

// PsiEdge is the edge potential function.
// \Psi(Xi, Xj) = PsiEdge.m[make_pair(i,j)]
struct PsiEdge {
    map<pair<int,int>, double> m; 
};

// Message of Xi, for each possible value of Xi, their is a double to store.
struct message {
    vector<double> values;
};

class node {
public:
    node(int index) {
        this->index = index;
        this->parent = NULL;
    }
    void setPsiNode(PsiNode s) {
        this->psi = s;
        // give msg dimension of psi.size();
        this->msgFromParent.values = vector<double>(this->psi.values.size(), 0);
    }
    void addChild(node *child, PsiEdge edgeFunc) {
        this->children.push_back(child);
        child->parent = this;
        child->edgeFunction = edgeFunc;
        // give msg dimension of psi.size();
        message temp;
        temp.values = vector<double>(this->psi.values.size(), 0);
        msgFromChildren[child->index] = temp;
    }
    bool isLeave() { return !this->children.size(); }
    bool isRoot() { return !this->parent; }
    bool isMyParent(node *a) { return this->parent == a; }
    
    void sendMsgToParent() {
        message &msg = this->parent->msgFromChildren[this->index];
        //cout << "size " << msg.values.size() << " parent: " << this->parent->index << endl;
        for (int i = 0; i < msg.values.size(); i++) {
            msg.values[i] = 0;
            for (int j = 0; j < this->psi.values.size(); j++) {
                double product = this->psi.values[j] * this->edgeFunction.m[make_pair(i, j)];
                //cout << "i=" << i << " " << "j=" << j<< " " << this->psi.values[j] << " " << this->edgeFunction.m[make_pair(i,j)]<< endl;
                if (!this->isLeave()) {
                    for (int k = 0; k < this->children.size(); k++) {
                        product *= this->msgFromChildren[children[k]->index].values[i];
                    }
                }
                msg.values[i] += product;
            }
        }  
    }
    void sendMsgToChild(node *child) {
        int ii = 0;
        for (ii = 0; ii < children.size() && children[ii]->index != child->index; ii++) {}
        if (ii >= children.size()) {
            // Didn't find index with in the children vector.
            cout << "error: index not in children list." << endl;
            return; 
        } 
        message &msg = child->msgFromParent;
        for (int i = 0; i < msg.values.size(); i++) {
            msg.values[i] = 0;
            for (int j = 0; j < this->psi.values.size(); j++) {
                double product = this->psi.values[j] * this->edgeFunction.m[make_pair(i, j)];
                if (!this->isRoot()) {
                    product *= this->msgFromParent.values[i];
                }
                for (int k = 0; k < this->children.size(); k++) {
                    if (k != child->index) {
                        product *= this->msgFromChildren[children[k]->index].values[i];
                    }
                }
                msg.values[i] += product;
            }
        }
    }

    int getIndex() { return this->index; }
    vector<node*> getChildren() { return this->children; }
    message getMsgFromParent() { return this->msgFromParent; }
    message getMsgFromChildIndex(int index) { return this->msgFromChildren[index]; }
    PsiNode getPsi() { return this->psi; }
    PsiEdge getEdgeFunction() { return this->edgeFunction; }
     
private:
    int index;
    // message that is sent to this node.
    message msgFromParent;
    map<int, message> msgFromChildren;

    // the node potential function on this node.
    PsiNode psi;
    
    // the edge potential function on the edge to parent.
    PsiEdge edgeFunction;

    vector<node*> children;
    node* parent;
};

void print(message m) {
    if (m.values.size() != 0)
        cout << "(" << m.values[0];
    for (int i = 1; i < m.values.size(); i++) {
        cout << ", " << m.values[i];
    }
    cout << ")" << endl;
}
// For debug.
void printMessages(vector<node*> allNodes) {
    for (int i = 0; i < allNodes.size(); i++) {
        cout << "Message " << allNodes[i]->getIndex() << " --> its parent" << ": ";
        print(allNodes[i]->getMsgFromParent());
        for (int j = 0; j < allNodes[i]->getChildren().size(); i++) {
            cout << "Message " << allNodes[i]->getIndex() << " --> " << allNodes[i]->getChildren()[i]->getIndex() << ": ";
            print(allNodes[i]->getMsgFromChildIndex(allNodes[i]->getChildren()[i]->getIndex()));
        }
    }
}
;

// Send message from a to b.
void sendMessage(node *a, node *b) {
    if (a->isMyParent(b)) {
        // So b is the parent of a.
        a->sendMsgToParent();
    } else if (b->isMyParent(a)) {
        // So a is the parent of b. 
        a->sendMsgToChild(b);
    } else {
        // Can't happen.
        cout << "error: send message not between neighbor." << endl;
    }
}
void collect(node *parent, node *child) {
    if (parent->isLeave()) {
        return; 
    }
    for (int i = 0; i < child->getChildren().size(); i++) {
        collect(child, child->getChildren()[i]);
    }
    sendMessage(child, parent);
}
void distribute(node *parent, node *child) {
    sendMessage(parent, child);
    // if child is a leave, then the loop below won't run since getChildren().size() == 0.
    for (int i = 0; i < child->getChildren().size(); i++) {
        distribute(child, child->getChildren()[i]);
    }
}

vector<double> computeMarginal(node *a) {
    vector<double> m;
    for (int i = 0; i < a->getPsi().values.size(); i++) {
        double mi = a->getPsi().values[i];
        mi *= a->getMsgFromParent().values[i];
        for (int i = 0; i < a->getChildren().size(); i++) {
            mi *= a->getMsgFromChildIndex(a->getChildren()[i]->getIndex()).values[i];
        }
        m.push_back(mi);
    } 
    return m;
}

void sum_product(node *root, vector<node*> allNodes) {
    for (int i = 0; i < root->getChildren().size(); i++) {
        collect(root, root->getChildren()[i]);
    }
    printMessages(allNodes);
    for (int i = 0; i < root->getChildren().size(); i++) {
        distribute(root, root->getChildren()[i]);
    }
    printMessages(allNodes);
    for (int i = 0; i < allNodes.size(); i++) {
        vector<double> result = computeMarginal(allNodes[i]);
        if (result.size() != 0)
            cout << "The marginal of node " << allNodes[i]->getIndex() << " is (" << result[0];
        for (int j = 1; j < result.size(); j++) {
            cout << ", " << result[i];
        }
        cout << ")" << endl;
    }
}

int main() {

    PsiEdge pEdge;
    pEdge.m[make_pair(0,0)] = 1;
    pEdge.m[make_pair(0,1)] = 0.5;
    pEdge.m[make_pair(1,0)] = 0.5;
    pEdge.m[make_pair(1,1)] = 1;

    PsiNode P135;
    P135.values.push_back(0.7);
    P135.values.push_back(0.3);

    PsiNode P246;
    P246.values.push_back(0.1);
    P246.values.push_back(0.9);

    vector<node*> allNodes;
    node *n1 = new node(1);
    n1->setPsiNode(P135);
    
    node *n2 = new node(2);
    n2->setPsiNode(P246);

    node *n3 = new node(3);
    n3->setPsiNode(P135);

    node *n4 = new node(4);
    n4->setPsiNode(P246);

    node *n5 = new node(5);
    n5->setPsiNode(P135);

    node *n6 = new node(6);
    n4->setPsiNode(P246);


    n1->addChild(n2, pEdge);
    n1->addChild(n3, pEdge);
    n2->addChild(n4, pEdge);
    n2->addChild(n5, pEdge);
    n3->addChild(n6, pEdge);

    allNodes.push_back(n1);
    allNodes.push_back(n2);
    allNodes.push_back(n3);
    allNodes.push_back(n4);
    allNodes.push_back(n5);
    allNodes.push_back(n6);

    sum_product(n1, allNodes);
/*
    cout << "The marginal of node " << 1 << " is (0.591, 0.409)"<< endl;
    cout << "The marginal of node " << 2 << " is (0.102, 0.898)"<< endl;
    cout << "The marginal of node " << 3 << " is (0.596, 0.404)"<< endl;
    cout << "The marginal of node " << 4 << " is (0.066, 0.934)"<< endl;
    cout << "The marginal of node " << 5 << " is (0.567, 0.433)"<< endl;
    cout << "The marginal of node " << 6 << " is (0.130, 0.870)"<< endl;
*/
    return 0;
}
