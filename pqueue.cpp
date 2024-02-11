/**********************************************
 ** File: pqueue.cpp
 ** Project: CMSC 341 Project 3, Fall 2023
 ** Author: Joshua Hur
 ** Date: 11/14/23
 ** Section: 2
 ** E-mail: jhur1@umbc.edu
 **
 ** This file creates a mock hospital emergency room priority queue using 
 ** a skew or a leftist-heap data structure with a max heap or a min-heap property.
 ** The program randomly creates patients' information and its urgency.
 ** It builds a priority queue, as a patient in the most critical situation goes first in the queue.
 ************************************************************************/

#include "pqueue.h"

// PQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure)
// The default constructor with the required initializations
PQueue::PQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}

// ~PQueue()
// The destructor deallocates the memory
PQueue::~PQueue() {
    clear();
}

// clear()
// Clear the queue and deletes all the nodes in the heap, leaving the heap empty
void PQueue::clear() {
    clearRecursively(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

// clearRecursively(Node* node)
// Recursive helper function of clear() to deallocates all memory of the heap
void PQueue::clearRecursively(Node* node) {
    if (node != nullptr) {
        clearRecursively(node -> m_left);
        clearRecursively(node -> m_right);
        delete node;
    }
}

// PQueue(const PQueue& rhs)
// The copy constructor makes a deep copy of the rhs object
PQueue::PQueue(const PQueue& rhs) {
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_heap = nullptr;
    m_size = 0;
    m_heap = copyRecursively(rhs.m_heap);
}

// operator=(const PQueue& rhs)
// Assignment operator creates an exact same copy of rhs
PQueue& PQueue::operator=(const PQueue& rhs) {
    
    // Check for self-assignment
    if (this != &rhs) {
        clear();
        m_priorFunc = rhs.m_priorFunc;
        m_heapType = rhs.m_heapType;
        m_structure = rhs.m_structure;
        m_heap = copyRecursively(rhs.m_heap);
        m_size = rhs.m_size;
    }
    
    return *this;
}

// copyRecursively(Node* node)
// Recursive helper function of PQueue(const PQueue& rhs) and operator=(const PQueue& rhs)
// that performs an exact same copy of rhs
Node* PQueue::copyRecursively(Node* node) {
    if (!node) {
        return nullptr;
    }
    
    Node* newNode = new Node (node -> m_patient);
    newNode -> m_left = copyRecursively(node -> m_left);
    newNode -> m_right = copyRecursively(node -> m_right);
    
    return newNode;
}

// mergeWithQueue(PQueue& rhs)
// Merge the queue with the another
void PQueue::mergeWithQueue(PQueue& rhs) {
    
    // Check if queues have the same priority functions and data structures
    if (this != &rhs && m_priorFunc == rhs.m_priorFunc && m_structure == rhs.m_structure) {
        m_heap = merge(m_heap, rhs.m_heap);
        rhs.m_heap = nullptr;
        m_size += rhs.m_size;
        rhs.m_size = 0;
    
    // Self-merging isn't possible
    } else if (this == &rhs && m_priorFunc == rhs.m_priorFunc && m_structure == rhs.m_structure) {
        throw domain_error("Cannot merge queue with itself.");
    
    // Merge of diffrent priority functions and data structures aren't allowed
    } else {
        throw domain_error("Queues have different priority functions or data structures.");
    }
    
}

// merge(Node* a, Node* b)
// Recursive helper function of mergeWithQueue(PQueue& rhs), insertPatient(const Patient& patient), reinsertNodes(Node* node), and getNextPatient() to merge two queues with the same priority  functions and data structures
Node* PQueue::merge(Node* a, Node* b) {
    
    // Check if one of the queues is empty
    if (!a) return b;
    if (!b) return a;
    
    // Check the heap type
    if (m_heapType == MAXHEAP) {
        if (m_priorFunc(a -> m_patient) < m_priorFunc(b -> m_patient)) {
            swap(a, b);
        }
        
    } else {
        if (m_priorFunc(a -> m_patient) > m_priorFunc(b -> m_patient)) {
            swap(a, b);
        }
    }

    // Now 'a' is guaranteed to have higher priority (or is equal) than 'b'
    // Merges the right child of 'a' with 'b', and swap the children
    if (m_structure == SKEW) {
        a -> m_right = merge(a -> m_right, b);
        swap(a -> m_left, a -> m_right);
        
    } else {
        
        // Leftist heap merging strategy remains the same as before
        a -> m_right = merge(a -> m_right, b);
        
        // Ensure the leftist property (the left child has higher NPL)
        if (getNPL(a -> m_right) > getNPL(a -> m_left)) {
            swap(a -> m_left, a -> m_right);
        }
        
        // Update NPL for leftist heap
        a -> m_npl = getNPL(a -> m_right) + 1;
    }

    return a;
}

// insertPatient(const Patient& patient)
// Insert a patient into the queue
void PQueue::insertPatient(const Patient& patient) {
    
    Node * newNode = new Node (patient);
    m_heap = merge(m_heap, newNode);
    m_size++;
    
    if (m_structure == LEFTIST) {
        m_heap -> m_npl = getNPL(m_heap);
    }
}

// getNPL(Node* node) const
// Recursive helper function of insertPatient(const Patient& patient) to update the NPL of each nodes
int PQueue::getNPL(Node* node) const {
    if (!node) return -1;
    if (!node -> m_right) return 0;
    return node -> m_right -> m_npl + 1;
}

// setPriorityFn(prifn_t priFn, HEAPTYPE heapType)
// Sets the new priority function and its corresponding heap type and rebuild the heap
void PQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {

    m_priorFunc = priFn;
    m_heapType = heapType;
    
    if (m_structure == SKEW) {
        rebuildAsSkewHeap();
        
    } else if (m_structure == LEFTIST) {
        rebuildAsLeftistHeap();
    }
}

// setStructure(STRUCTURE structure)
// Sets the data structure of the heap and rebuild the heap
void PQueue::setStructure(STRUCTURE structure) {

    m_structure = structure;
    
    if (m_structure == SKEW) {
        rebuildAsSkewHeap();
        
    } else if (m_structure == LEFTIST) {
        rebuildAsLeftistHeap();
    }
}

// rebuildAsSkewHeap()
// Rebuild the heap with skew heap property
void PQueue::rebuildAsSkewHeap() {
    
    // Copy the original heap
    Node* copiedHeap = copyRecursively(m_heap);
    clear();
    
    // Set the structure to SKEW
    m_structure = SKEW;

    // Reinsert nodes into the new skew heap
    reinsertNodes(copiedHeap);
}

// rebuildAsLeftistHeap()
// Rebuild the heap with leftist heap property
void PQueue::rebuildAsLeftistHeap() {
    
    // Copy the original heap
    Node* copiedHeap = copyRecursively(m_heap);
    clear();
    
    // Set the structure to LEFTIST
    m_structure = LEFTIST;

    // Reinsert nodes into the new leftist heap
    reinsertNodes(copiedHeap);
}

// reinsertNodes(Node* node)
// Recursive helper function of rebuildAsSkewHeap() and rebuildAsLeftistHeap() to rebuild the heap with the new data structure
void PQueue::reinsertNodes(Node* node) {
    if (node == nullptr) {
        return;
    }

    Node* left = node->m_left;
    Node* right = node->m_right;

    // Detach the children to prevent them from being deleted
    node->m_left = nullptr;
    node->m_right = nullptr;

    // Merge the current node into the skew heap
    m_heap = merge(m_heap, node);
    m_size++;

    // Recursively reinsert left and right children
    reinsertNodes(left);
    reinsertNodes(right);
}

// printPatientQueue() const
// Print the list of the queue based on priority number using preorder traversal
void PQueue::printPatientQueue() const {
    printPreorder(m_heap);
}

// printPreorder(Node* node) const
// Recursive helper function of printPatientQueue() const to print each patient's information by traversing the heap
void PQueue::printPreorder(Node* node) const {
    if (node != nullptr) {
        cout << "[" << m_priorFunc(node -> m_patient) << "] " << node -> m_patient << endl;
        printPreorder(node -> m_left);
        printPreorder(node -> m_right);
    }
}

// getNextPatient()
// Remove and return the highest priority patient from the queue
Patient PQueue::getNextPatient() {
    
    // Flag when the queue is empty and call this function
    if (!m_heap) {
        throw out_of_range("The queue is empty.");
    }
    
    // Traverse the queue, remove the highest priority patient, and adjust the queue
    Node* root = m_heap;
    Patient patient = root -> m_patient;
    m_heap = merge(root -> m_left, root -> m_right);
    delete root;
    m_size--;
    return patient;
}

// getPriorityFn() const
// Return the current priority function
prifn_t PQueue::getPriorityFn() const {
    return m_priorFunc;
}

// numPatients() const
// Return the current number of patients in the queue
int PQueue::numPatients() const {
    return m_size;
}

// getHeapType() const
// Return the current type of the heap
HEAPTYPE PQueue::getHeapType() const {
    return m_heapType;
}

// getRoot() const
// Helper function to get the root node of the queue
Node* PQueue::getRoot() const {
    return m_heap;
}

// dump() const
// Visualize the desired data structure
void PQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
      
  } else {
    dump(m_heap);
  }
  cout << endl;
}

// dump(Node *pos) const
// Helper function of dump() const to visualize the data structure by traversing
void PQueue::dump(Node *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos -> m_left);
      
    if (m_structure == SKEW)
        cout << m_priorFunc(pos -> m_patient) << ":" << pos -> m_patient.getPatient();
    else
        cout << m_priorFunc(pos -> m_patient) << ":" << pos -> m_patient.getPatient() << ":" << pos -> m_npl;
      
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Patient& patient) {
  sout  << patient.getPatient()
        << ", temperature: " << patient.getTemperature()
        << ", oxygen: " << patient.getOxygen()
        << ", respiratory: " << patient.getRR()
        << ", blood pressure: " << patient.getBP()
        << ", nurse opinion: " << patient.getOpinion();
  return sout;
}

ostream& operator<<(ostream& sout, const Node& node) {
  sout << node.getPatient();
  return sout;
}
