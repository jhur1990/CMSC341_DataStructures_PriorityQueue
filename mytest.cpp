/**********************************************
 ** File: mytest.cpp
 ** Project: CMSC 341 Project 3, Fall 2023
 ** Author: Joshua Hur
 ** Date: 11/14/23
 ** Section: 2
 ** E-mail: jhur1@umbc.edu
 **
 ** This file contains the test functions that test priority queues with a skew or a leftist-heap data structure with a max heap or min-heap property.
 ************************************************************************/

#include "pqueue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

// Priority functions compute an integer priority for a patient.  Internal
// computations may be floating point, but must return an integer.

int priorityFn1(const Patient & patient);
int priorityFn2(const Patient & patient);

// a name database for testing purposes
const int NUMNAMES = 20;
string nameDB[NUMNAMES] = {
    "Ismail Carter", "Lorraine Peters", "Marco Shaffer", "Rebecca Moss",
    "Lachlan Solomon", "Grace Mclaughlin", "Tyrese Pruitt", "Aiza Green",
    "Addie Greer", "Tatiana Buckley", "Tyler Dunn", "Aliyah Strong",
    "Alastair Connolly", "Beatrix Acosta", "Camilla Mayo", "Fletcher Beck",
    "Erika Drake", "Libby Russo", "Liam Taylor", "Sofia Stewart"
};

// We can use the Random class to generate the test data randomly!
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

class Tester{
    public:
    
    // testMinHeap(PQueue& queue)
    // Case: Verify min-heap property of the heap
    // Expected result: Return true if the min-heap property is satisfied at every node, else return false
    bool testMinHeap(PQueue& queue) {
        return minHeapProperty(queue.getRoot(), queue.getPriorityFn());
    }
    
    // minHeapProperty(Node* node, prifn_t priFn)
    // Recursive helper function of testMinHeap(PQueue& queue) that check each node have min-heap prtopety
    bool minHeapProperty(Node* node, prifn_t priFn) {
        if (!node) return true;

        bool left = (!node -> m_left) || (priFn(node -> m_patient) <= priFn(node -> m_left -> m_patient) && minHeapProperty(node -> m_left, priFn));

        bool right = (!node -> m_right) || (priFn(node -> m_patient) <= priFn(node -> m_right -> m_patient) && minHeapProperty(node -> m_right, priFn));
        
        return left && right;
    }
    
    // testMaxHeap(PQueue &queue)
    // Case: Verify max-heap property of the heap
    // Expected result: Return true if the max-heap property is satisfied at every node, else return false
    bool testMaxHeap(PQueue &queue) {
        
        return maxHeapProperty(queue.getRoot(), queue.getPriorityFn());
    }
    
    // maxHeapProperty(Node* node, prifn_t priFn)
    // Recursive helper function of testMaxHeap(PQueue& queue) that check each node have max-heap prtopety
    bool maxHeapProperty(Node* node, prifn_t priFn) {
        if (!node) return true;

        bool left = (!node -> m_left) || (priFn(node -> m_patient) >= priFn(node -> m_left -> m_patient) && maxHeapProperty(node -> m_left, priFn));

        bool right = (!node -> m_right) || (priFn(node -> m_patient) >= priFn(node -> m_right -> m_patient) && maxHeapProperty(node -> m_right, priFn));
        
        return left && right;
    }
    
    // testMinHeapRemoval(PQueue& queue)
    // Case: Check if remove the heap happen in the correct order
    // Expected result: Return true if all removals happen in the min-heap property, else return false
    bool testMinHeapRemoval(PQueue& queue) {
        int lastPriority = 71;
        
        // Start with the highest priority integer - 71 is the lowest in min-heap
        for (int i = 0; i < 300; i++) {
            Patient removedPatient = queue.getNextPatient();
            int currentPriority = queue.getPriorityFn()(removedPatient);
            
            // If there is a priority number that is lower than the highest priority, it violates min-heap property
            if (currentPriority < lastPriority) {
                return false;
            }
            
            lastPriority = currentPriority;
        }
        
        return true;
    }
    
    // testMaxHeapRemoval(PQueue& queue)
    // Case: Check if remove the heap happen in the correct order
    // Expected result: Return true if all removals happen in the max-heap property, else return false
    bool testMaxHeapRemoval(PQueue& queue) {
        int lastPriority = 242;
        
        // Start with the highest priority integer - 242 is the highest in max-heap
        for (int i = 0; i < 300; i++) {
            Patient removedPatient = queue.getNextPatient();
            int currentPriority = queue.getPriorityFn()(removedPatient);
            
            // If there is a priority number that is higher than the highest priority, it violates max-heap property
            if (currentPriority > lastPriority) {
                return false;
            }

            lastPriority = currentPriority;
        }
        
        return true;
    }
    
    // testNPLValues(PQueue& queue)
    // Case: Verify all the nodes in a leftist heap have the correct NPL value
    // Expected result: Return true if the calculated NPL value of every node preserves the correct heap property, else return false
    bool testNPLValues(PQueue& queue) {
        return testNPLHelper(queue.getRoot());
    }
    
    // testNPLHelper(Node* root)
    // Recursive helper function of testNPLValues(PQueue& queue) to calculate each node's NPL value and is always greater one than its right node's NPL value
    bool testNPLHelper(Node* root) {
        if (root == nullptr) {
            return true;
        }
        
        int rightNPL;
        
        if (root -> m_right != nullptr) {
            rightNPL = root -> m_right -> m_npl;
            
        } else {
            rightNPL = -1;
        }
        
        bool isCurrentNPLCorrect = (root -> m_npl == rightNPL + 1);
        
        return isCurrentNPLCorrect && testNPLHelper(root -> m_left) && testNPLHelper(root -> m_right);
    }
    
    // testLeftistProperty(PQueue& queue)
    // Case: Verify all the nodes in a leftist heap preserves the property of such a heap
    // Expected result: Return true if a node's left child's NPL value is greater than the right child's NPL value, else return false
    bool testLeftistProperty(PQueue& queue) {
        return testLeftistHelper(queue.getRoot());
    }
    
    // testLeftistHelper(Node* root)
    // Recursive helper function of testLeftistProperty(PQueue& queue) that checks if a node's left child's NPL value is greater than the right child's NPL value
    bool testLeftistHelper(Node* root) {
        if (root == nullptr) {
            return true;
        }
        
        int leftNPL = 0;
        int rightNPL = 0;
        
        if (root -> m_left != nullptr) {
            leftNPL = root -> m_left -> m_npl;
        } else {
            leftNPL = -1;
        }
        
        if (root -> m_right != nullptr) {
            rightNPL = root -> m_right -> m_npl;
        } else {
            rightNPL = -1;
        }
        
        bool isLeftistProperty = leftNPL >= rightNPL;
        
        return isLeftistProperty && testLeftistHelper(root -> m_left) && testLeftistHelper(root -> m_right);
    }
    
    // testPriorityFunction(PQueue& queue)
    // Case: Verify a correct heap is rebuilt after chanigng the priority function
    // Expected result: Return true if the smallest number is the highest priority in min-heap and the largest number is the highest priority in max-heap, else return false
    bool testPriorityFunction(PQueue& queue) {
        bool isRebuiltCorrectly = true;
        int prevPriority = 0;
        
        // Check if priority function 2 and its heap type is min-heap
        if (queue.getPriorityFn() == priorityFn2 && queue.getHeapType() == MINHEAP) {
            prevPriority = 71;
            
            // Check 71 is the smallest number in the heap
            while (queue.numPatients() > 0) {
                Patient topPatient = queue.getNextPatient();
                int currentPriority = priorityFn2(topPatient);
                if (currentPriority < prevPriority) {
                    isRebuiltCorrectly = false;
                    break;
                }
                
                prevPriority = currentPriority;
            }
            isRebuiltCorrectly = true;

        // Check if priority function 1 and its heap type is max-heap
        } else if (queue.getPriorityFn() == priorityFn1 && queue.getHeapType() == MAXHEAP) {
            prevPriority = 242;
            
            // Check 242 is the largest number in the heap
            while (queue.numPatients() > 0) {
                Patient topPatient = queue.getNextPatient();
                int currentPriority = priorityFn1(topPatient);
                if (currentPriority > prevPriority) {
                    isRebuiltCorrectly = false;
                    break;
                }
                prevPriority = currentPriority;
            }
            isRebuiltCorrectly = true;
            
        } else {
            isRebuiltCorrectly = false;
        }
        return isRebuiltCorrectly;
    }
    
    // testMergeEmptyQueue(PQueue& emptyQueue, PQueue& populatedQueue)
    // Case: Verify if the merge of an empty queue with the queue that is not empty is successful
    // Expected result: Return true if the merge is successful, else return false
    bool testMergeEmptyQueue(PQueue& emptyQueue, PQueue& populatedQueue) {
        bool isEmptyMergeCorrectly = true;
        
        // Check if the queue to merge is empty
        if (emptyQueue.numPatients() == 0) {
            int sizeBeforeMerge = populatedQueue.numPatients();
            populatedQueue.mergeWithQueue(emptyQueue);
            int sizeAfterMerge = populatedQueue.numPatients();
            
            if (sizeAfterMerge != sizeBeforeMerge) {
                isEmptyMergeCorrectly = false;
                
            } else {
                isEmptyMergeCorrectly = true;
            }
            
        } else {
            isEmptyMergeCorrectly = false;
        }
        
        return isEmptyMergeCorrectly;
    }
    
    // testCopyConstructorNormal(PQueue& originalQueue, PQueue& copiedQueue)
    // Case: Verify if the copy constructor successfully makes a copy of the original queue
    // Expected result: Return true if the copied heap has the same nodes in the original, else return false
    bool testCopyConstructorNormal(PQueue& originalQueue, PQueue& copiedQueue) {
        return areTreesEqual(originalQueue.getRoot(), copiedQueue.getRoot());
    }
    
    // testCopyConstructorEdge(PQueue& originalQueue, PQueue& copiedQueue)
    // Case: Verify if the copy constructor successfully makes an empty copy of the original empty queue
    // Expected result: Return true if the copied heap is empty as the original, else return false
    bool testCopyConstructorEdge(PQueue& originalQueue, PQueue& copiedQueue) {
        return areTreesEqual(originalQueue.getRoot(), copiedQueue.getRoot());
    }
    
    // testAssignmentOperatorNormal(PQueue& originalQueue, PQueue& copiedQueue)
    // Case: Verify the assignment operator successfully makes a copy of the original queue
    // Expected result: Return true if the copied heap has the same nodes in the original, else return false
    bool testAssignmentOperatorNormal(PQueue& originalQueue, PQueue& copiedQueue) {
        return areTreesEqual(originalQueue.getRoot(), copiedQueue.getRoot());
    }
    
    // testAssignmentOperatorEdge(PQueue& originalQueue, PQueue& copiedQueue)
    // Case: Verify the assignment operator successfully makes an empty copy of the original empty queue
    // Expected result: Return true if the copied heap is empty as the original, else return false
    bool testAssignmentOperatorEdge(PQueue& originalQueue, PQueue& copiedQueue) {
        return areTreesEqual(originalQueue.getRoot(), copiedQueue.getRoot());
    }
    
    // areTreesEqual(Node* originalRoot, Node* copiedRoot)
    // Recursive helper function of testCopyConstructorNormal, testCopyConstructorEdge, testAssignmentOperatorNormal, and testAssignmentOperatorEdge(PQueue& originalQueue, PQueue& copiedQueue) that checks two heaps are identical
    bool areTreesEqual(Node* originalRoot, Node* copiedRoot) {
        
        // Both nodes are NULL, heaps are still identical
        if (originalRoot == nullptr && copiedRoot == nullptr) {
            return true;
        }

        // One of them is NULL, heaps are not identical
        if (originalRoot == nullptr || copiedRoot == nullptr) {
            return false;
        }

        // If the values are different, heaps are not identical
        if (originalRoot -> m_patient == copiedRoot -> m_patient) {
            return areTreesEqual(originalRoot -> m_left, copiedRoot -> m_left) &&
                   areTreesEqual(originalRoot -> m_right, copiedRoot -> m_right);
        }
        
        return false;
    }
    
    // testDequeueEmptyQueue(PQueue& emptyQueue)
    // Case: Verify dequeue from the empty queue throws an exception
    // Expected result: Return true if it throws an exception, else return false
    bool testDequeueEmptyQueue(PQueue& emptyQueue) {
        try {
            emptyQueue.getNextPatient();
            return false;
            
        } catch (const out_of_range& e) {
            return true;
        }
    }
    
    // testMergeDifferentPriorityFunctions(PQueue& queueOne, PQueue& queueTwo)
    // Case: Verify merge queues with different priority functions throws an exception
    // Expected result: Return true if it throws an exception, else return false
    bool testMergeDifferentPriorityFunctions(PQueue& queueOne, PQueue& queueTwo) {
        try {
            queueOne.mergeWithQueue(queueTwo); // Attempt to merge the queues queue
            return false;
            
        } catch (const domain_error& e) {
            return true;
        }
    }
};

int main(){
    Tester tester;
    
    Random nameGen(0,NUMNAMES-1);
    Random temperatureGen(MINTEMP,MAXTEMP);
    Random oxygenGen(MINOX,MAXOX);
    Random respiratoryGen(MINRR,MAXRR);
    Random bloodPressureGen(MINBP,MAXBP);
    Random nurseOpinionGen(MINOPINION,MAXOPINION);
    
    PQueue minHeapQueue(priorityFn2, MINHEAP, LEFTIST);
    PQueue maxHeapQueue(priorityFn1, MAXHEAP, LEFTIST);
    for (int i = 0; i < 300; i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        minHeapQueue.insertPatient(patient);
        maxHeapQueue.insertPatient(patient);
    }
    
    //minHeapQueue.dump();
    if (tester.testMinHeap(minHeapQueue)){
        cout << "Min-Heap test is passed!" << endl;
    } else {
        cout << "Min-Heap test is failed!" << endl;
    }
    
    //maxHeapQueue.dump();
    if (tester.testMaxHeap(maxHeapQueue)){
        cout << "Max-Heap test is passed!" << endl;
    } else {
        cout << "Max-Heap test is failed!" << endl;
    }
    
    PQueue minHeapQueueRemove(priorityFn2, MINHEAP, LEFTIST);
    PQueue maxHeapQueueRemove(priorityFn1, MAXHEAP, LEFTIST);
    for (int i = 0; i < 300; i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        minHeapQueueRemove.insertPatient(patient);
        maxHeapQueueRemove.insertPatient(patient);
    }
    
    if (tester.testMinHeapRemoval(minHeapQueueRemove)) {
        cout << "Test passed: Min-Heap removal is correct." << endl;
    } else {
        cout << "Test failed: Removal order is incorrect." << endl;
    };
    
    if (tester.testMaxHeapRemoval(maxHeapQueueRemove)){
        cout << "Test passed: Max-Heap removal is correct." << endl;
    } else {
        cout << "Test failed: Removal order is incorrect." << endl;
    }
    
    PQueue leftistHeapOne(priorityFn2, MINHEAP, LEFTIST);
    PQueue leftistHeapTwo(priorityFn2, MINHEAP, LEFTIST);
    PQueue TreeOne(priorityFn2, MINHEAP, LEFTIST);

    for (int i = 0; i < 5; i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        leftistHeapOne.insertPatient(patient);
        leftistHeapTwo.insertPatient(patient);
        TreeOne.insertPatient(patient);
    }
     
    //TreeOne.dump();
    
    if (tester.testNPLValues(leftistHeapOne)) {
        cout << "Test passed: All nodes in a leftist heap have the correct NPL values." << endl;
    } else {
        cout << "Test failed: Not every nodes in a leftist heap have the correct NPL values." << endl;
    }
    
    if (tester.testNPLValues(leftistHeapTwo)) {
        cout << "Test passed: Leftist heap property is maintained!" << endl;
    } else {
        cout << "Test failed: Not every node the NPL value of the left child is greater than or equal to the NPL value of the right child!" << endl;
    }

    TreeOne.setPriorityFn(priorityFn1, MAXHEAP);
    if (tester.testPriorityFunction(TreeOne)) {
        cout << "Test Passed: Heap rebuilt correctly with new priority function." << endl;
    } else {
        cout << "Test Failed: Heap not rebuilt correctly with new priority function." << endl;
    }
    
    PQueue emptyQueue(priorityFn2, MINHEAP, LEFTIST);
    PQueue populatedQueue(priorityFn2, MINHEAP, LEFTIST);
    
    PQueue queueOne(priorityFn2, MINHEAP, LEFTIST);
    PQueue queueTwo(priorityFn2, MINHEAP, LEFTIST);

    for (int i = 0; i < 10; i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        populatedQueue.insertPatient(patient);
        queueOne.insertPatient(patient);
    }
    
    if (tester.testMergeEmptyQueue(emptyQueue, populatedQueue)) {
        cout << "Test Passed: Merge with empty queue successful." << endl;
        
    } else {
        cout << "Test Failed: Merge with empty queue unsuccessful." << endl;
    }
    
    PQueue queueThree(priorityFn2, MINHEAP, LEFTIST);
    for (int i = 0; i < 10; i++){
        Patient patient(nameDB[nameGen.getRandNum()],
                    temperatureGen.getRandNum(),
                    oxygenGen.getRandNum(),
                    respiratoryGen.getRandNum(),
                    bloodPressureGen.getRandNum(),
                    nurseOpinionGen.getRandNum());
        
        queueThree.insertPatient(patient);
    }
    
    PQueue queueFour(queueThree);
    if (tester.testCopyConstructorEdge(queueThree, queueFour)){
        cout << "Normal Test Passed: The copy constructor successfully makes a deep copy." << endl;
    } else {
        cout << "Normal Test Passed: The copy constructor fail to make a deep copy." << endl;
    }
    
    PQueue queueFive(priorityFn2, MINHEAP, LEFTIST);
    PQueue queueSix(queueFive);
    
    if (tester.testCopyConstructorEdge(queueThree, queueFour)){
        cout << "Edge Test Passed: The copy constructor successfully makes a deep copy." << endl;
    } else {
        cout << "Edge Test Passed: The copy constructor fail to make a deep copy." << endl;
    }
    
    queueTwo = queueOne;
    if (tester.testAssignmentOperatorNormal(queueOne, queueTwo)) {
        cout << "Normal Test Passed: The assignment operator successfully creates an exact same copy." << endl;
    } else {
        cout << "Normal Test Failed: The assignment operator fail to create an exact same copy." << endl;
    }
    
    PQueue queueSeven(priorityFn2, MINHEAP, LEFTIST);
    PQueue queueEight(priorityFn2, MINHEAP, LEFTIST);
    
    queueEight = queueSeven;
    if (tester.testAssignmentOperatorEdge(queueSeven, queueEight)) {
        cout << "Edge Test Passed: The assignment operator successfully creates an exact same copy." << endl;
        
    } else {
        cout << "Edge Test Failed: The assignment operator fail to create an exact same copy." << endl;
    }
    
    PQueue queueNine(priorityFn2, MINHEAP, LEFTIST);
    if (tester.testDequeueEmptyQueue(queueNine)) {
        cout << "Test passed: Caught out_of_range exception." << endl;
        
    } else {
        cout << "Test failed: No exception thrown for dequeue from empty queue." << endl;
    }
    
    PQueue queueTen(priorityFn2, MINHEAP, LEFTIST);
    PQueue queueEleven(priorityFn1, MAXHEAP, LEFTIST);
    if (tester.testMergeDifferentPriorityFunctions(queueTen, queueEleven)) {
        cout << "Test passed: Caught domain_error exception." << endl;
        
    } else {
        cout << "Test failed: No exception thrown for merging queues with different priority functions." << endl;
    }
    
    return 0;
}

int priorityFn1(const Patient & patient) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [115-242]
    //temperature + respiratory + blood pressure
    //the highest priority would be 42+40+160 = 242
    //the lowest priority would be 35+10+70 = 115
    //the larger value means the higher priority
    int priority = patient.getTemperature() + patient.getRR() + patient.getBP();
    return priority;
}

int priorityFn2(const Patient & patient) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [71-111]
    //nurse opinion + oxygen
    //the highest priority would be 1+70 = 71
    //the lowest priority would be 10+101 = 111
    //the smaller value means the higher priority
    int priority = patient.getOpinion() + patient.getOxygen();
    return priority;
}
