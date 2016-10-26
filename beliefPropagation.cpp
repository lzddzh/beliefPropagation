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
    }
    void setPsiNode(PsiNode s) {
        this->psi = s;
        // give msg dimension of psi.size();
        this->msgFromParent.values = vector<double>(this->psi.values.size(), 0);
    }
    void addChild(node *child, PsiEdge edgeFunc) {
        this->children.push_back(child);
        child->edgeFunction = edgeFunc;
        // give msg dimension of psi.size();
        message temp;
        temp.values = vector<double>(this->psi.values.size(), 0);
        msgFromChildren[child->index] = temp;
    }
    bool isLeave() {return !children.size();}
    
    void sendMsgToParent() {
        message &msg = this->parent->msgFromChildren[this->index];
        for (int i = 0; i < msg.values.size(); i++) {
            msg.values[i] = 0;
            for (int j = 0; j < this->psi.values.size(); j++) {
                double product = this->psi.values[j] * this->edgeFunction.m[make_pair(i, j)];
                if (!this->isLeave()) {
                    for (int k = 0; k < this->children.size(); k++) {
                        product *= this->msgFromChildren[children[k]->index].values[i];
                    }
                }
                msg.values[i] += product;
            }
        }  
    }
    void sendMsgToChilrend() {
    
    }

    void collect() {
        if (this->isLeave()) {
            
        }
    }
    void distribute() {

    }

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

vector<pair<int, double> > computeMarginal(node *root) {
    
}

void sum_product(node *root) {
    root->collect();
    root->distribute();
    vector<pair<int, double> > marginal = computeMarginal(root);
    for (int i = 0; i < marginal.size(); i++) {
        cout << "The marginal of node " << marginal[i].first << " is " << marginal[i].second << endl;
    }
}

int main() {
    PsiEdge pEdge;
    pEdge.m[make_pair(0,0)] = 1;
    pEdge.m[make_pair(0,1)] = 0.5;
    pEdge.m[make_pair(1,0)] = 1;
    pEdge.m[make_pair(1,1)] = 0.5;


    PsiNode P135;
    P135.values.push_back(0.7);
    P135.values.push_back(0.3);

    PsiNode P246;
    P246.values.push_back(0.1);
    P246.values.push_back(0.9);


    node *n1 = new node(0);
    n1->setPsiNode(P135);
    
    node *n2 = new node(1);
    n2->setPsiNode(P246);

    node *n3 = new node(3);
    n3->setPsiNode(P135);

    n1->addChild(n2, pEdge);
    n1->addChild(n3, pEdge);

    n2->sendMsgToParent();
    for (int i = 0; i < n1->msgFromChildren[2].values.size(); i++) {
        cout << n1->msgFromChildren[2].values[i] << endl;
    }
    
    return 0;
}
