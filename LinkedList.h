#include <initializer_list>
#include <string>
#include <sstream>

#pragma once

/*
Im not sure if this is a good implementation or not
there's a good chance it isn't but I felt better
about knowing what my collections type was doing
when moving elements around. I also think I fixed
memory leakage, again not sure. The idea would be
that the elements will remain in memory until
they are explicitly deleted. They can be accessed 
while held in memory by the class itself.
*/

using std::string;
using std::stringstream;
using std::initializer_list;

template<typename T>
struct LinkedListElement {
	T value;
	LinkedListElement<T>* next = NULL;
};

template<typename T>
bool byteCompare(T* _ptr1, T* _ptr2) {
	char* ptr1 = (char*)_ptr1;
	char* ptr2 = (char*)_ptr2;
	for (unsigned int i = 0; i < sizeof(T); i++)
		if (ptr1[i] != ptr2[i])
			return false;
	return true;
}

template<typename T>
class LinkedList {
private:
	LinkedListElement<T>* start = NULL;
	unsigned int size = 0;

	void _appendElem(LinkedListElement<T>* ptr) {
		if (this->start == NULL) {
			this->start = ptr;
			this->size++;
			return;
		}

		this->_getAt(this->size - 1)->next = ptr;
		this->size++;
	}
	
	void _insertElem(LinkedListElement<T>* ptr, unsigned int off) {
		if (this->start == NULL) {
			this->start = ptr;
			this->size++;
			return;
		}

		if (off >= this->size) {
			this->_appendElem(ptr);
			this->size++;
			return;
		}

		LinkedListElement<T>* front, *back;
		front = this->_getAt(off);
		ptr->next = front;

		if (off != 0) {
			back = this->_getAt(off - 1);
			back->next = ptr;
		}
		else {
			this->start = ptr;
		}

		this->size++;
	}

	LinkedListElement<T>* _getAt(unsigned int off) {
		if (off >= this->size)
			return NULL;
		LinkedListElement<T>* ptr = this->start;
		for (unsigned int i = 0; i < off; i++)
			ptr = ptr->next;
		return ptr;
	}

public:
	LinkedList() {}
	LinkedList(LinkedListElement<T>* ptr) {
		this->start = ptr;
		this->size = 1;
		while (ptr->next != NULL) {
			this->size++;
			ptr = ptr->next;
		}
	}
	LinkedList(T t, unsigned int count = 1) {
		for (unsigned int i = 0; i < count; i++)
			this->append(t);
	}
	LinkedList(initializer_list<T> lst) {
		for (T t : lst)
			this->append(t);
	}

	void append(T t) {
		LinkedListElement<T>* ptr = new LinkedListElement<T>;
		ptr->value = t;
		ptr->next = NULL;
		this->_appendElem(ptr);
	}

	void prepend(T t) { this->insert(t, 0); }

	void insert(T t, unsigned int off) {
		LinkedListElement<T>* ptr = new LinkedListElement<T>;
		ptr->value = t;
		ptr->next = NULL;
		this->_insertElem(ptr, off);
	}

	void clear() {
		LinkedListElement<T>* ptr = this->start, * next;
		while ((next = ptr->next) != NULL) {
			delete ptr;
			ptr = next;
		}
		delete ptr;
	}

	void replace(T t, unsigned int off) {
		if (off >= this->size)
			return;
		this->_getAt(off)->value = t;
	}

	void remove(unsigned int off) {
		if (off >= this->size)
			return;
		LinkedListElement<T>* ptr = this->_getAt(off);
		LinkedListElement<T>* front = NULL;
		if (off < (this->size - 1))
			front = this->_getAt(off + 1);

		if (off == 0) {
			this->start = front;
		}
		else {
			LinkedListElement<T>* back = this->_getAt(off - 1);
			back->next = front;
		}

		delete ptr;
		this->size--;
	}

	bool contains(T t, bool useEquals = true) { // 'useEquals' is for the edge cases of custom operator functions
		if (useEquals) {
			for (unsigned int i = 0; i < this->size; i++)
				if (t == this->_getAt(i)->value)
					return true;
			return false;
		}
		else {
			for (unsigned int i = 0; i < this->size; i++)
				if (byteCompare(&t, &this->_getAt(i)->value))
					return true;
			return false;
		}
	}

	int find(T t, bool useEquals = true) {
		if (useEquals) {
			for (int i = 0; i < this->size; i++)
				if (t == this->_getAt(i)->value)
					return i;
			return -1;
		}
		else {
			for (int i = 0; i < this->size; i++)
				if (byteCompare(&t, &this->_getAt(i)->value))
					return i;
			return -1;
		}
	}

	unsigned int length() { return this->size; }
	unsigned int memSize() { return this->size * sizeof(LinkedListElement<T>); }

	T& getAt(unsigned int off) { return this->_getAt(off)->value; }
	LinkedListElement<T>* getListPtr() { return this->start; }

	string toString() { // get java'd
		stringstream sstream;
		sstream << '[';
		for (unsigned int i = 0; i < this->size; i++) {
			if (i != 0)
				sstream << ", ";
			sstream << this->_getAt(i)->value;
		}
		sstream << ']';
		return sstream.str();
	}

	T& operator[](const unsigned int& offset) { return this->getAt(offset); }

};