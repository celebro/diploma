/*
 * stack.hpp
 *
 *  Created on: 20. avg. 2012
 *      Author: jure
 */

#ifndef STACK_HPP_
#define STACK_HPP_

#include <iostream>

using namespace std;


class StackUnderFlowException {
public:
	StackUnderFlowException() {
		cout << "Stack underflow" << endl;
	}
};


class StackOverFlowException {
public:
	StackOverFlowException() {
		cout << "Stack overflow" << endl;
    }
};



template <class T> class ArrayStack {
private:
	int _capacity;
	int _top;
	T* _data;
public:
	ArrayStack(int capacity) {
		_capacity = capacity;
		_data = new T[capacity];
		_top = -1;
    }

    void push(T element) {
        if (_top >= _capacity) throw new StackOverFlowException();
		_data[++_top] = element;
    }

    T pop() {
        if (_top == -1) throw new StackUnderFlowException();
		return _data[_top--];
    }

    T top() {
        return _data[_top];
    }

    int size() {
        return _top + 1;
    }

    bool empty() {
        return (_top == -1 );
    }
};


struct Node {
    int data;
    Node* link;
};

class ListStack {
private:
	Node* top;
	int count;
public:
	ListStack() {
		top = NULL;
		count = 0;
	}

	~ListStack() {
    	Node* n = top;
    	while (n != NULL) {
    		Node* c = n;
    		n = n->link;
    		delete c;
    	}
	}

    void push(int element) {
        Node* temp = new Node();
        temp->data = element;
        temp->link = top;
        top = temp;
        count++;
    }

    int pop() {
        if ( top == NULL ) {
        	throw new StackUnderFlowException();
        }
		int ret = top->data;
		Node* temp = top->link;
		delete top;
		top = temp;
		count--;
		return ret;
    }

    int first() {
        return top->data;
    }

    int size() {
        return count;
    }

    bool isEmpty() {
        return (top == NULL );
    }

    int* toArray() {
    	int *a = new int[size() + 1];
    	Node* n = top;
    	int i = 0;
    	while (n != NULL) {
    		a[i++] = n->data;
    		n = n->link;
    	}
    	a[count] = -1;
    	return a;
    }
};


#endif /* STACK_HPP_ */
