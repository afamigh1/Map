#ifndef MAP_HPP
#define MAP_HPP
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <utility>
#include <random>
#include <stdexcept>
#include <string>

/*notes*/
/*
	SkipList implementation taken and modified from:
	http://www.drdobbs.com/cpp/skip-lists-in-c/184403579?pgno=1
*/
namespace cs540 {
	
	template <typename Key_T, typename Mapped_T>
	using ValueType = std::pair<const Key_T, Mapped_T>;

	template <typename Key_T, typename Mapped_T>
	class Map;
	
	class Iterator;
	class ConstIterator;
	class ReverseIterator;
	
	template <typename Key_T, typename Mapped_T> 
	class SkipList;
	template <typename Key_T, typename Mapped_T> 
	class SkipNode;
	
	template <typename Key_T, typename Mapped_T> 
	class DataNode;
	
	class RandomHeight {
		public:
			RandomHeight(int maxlevel, float prob);
			~RandomHeight() {}
			int newLevel(void);
		private:
			int maxLevel;
			float probability;
	};
	
	RandomHeight::RandomHeight(int maxlevel, float prob) {
		srand( time(NULL) );
		maxLevel = maxlevel;
		probability = prob;
	}
	
	int RandomHeight::newLevel(void) {
		int tmplevel = 1;
		
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		
		while(( r < probability ) && (tmplevel < maxLevel))
			tmplevel++;
		return tmplevel;
	}
	
	template <typename Key_T, typename Mapped_T>
	class DataNode : public SkipNode<Key_T, Mapped_T> {
		public:		
			DataNode() {}
			DataNode(ValueType<Key_T, Mapped_T>, int);
			DataNode(const DataNode &);
			~DataNode();
			
			ValueType<Key_T, Mapped_T> pear;
		private:
			
			
	};
	
	template <typename Key_T, typename Mapped_T>
	DataNode<Key_T, Mapped_T>::DataNode(ValueType<Key_T, Mapped_T> pair_in, int heightIn) 
										: SkipNode<Key_T, Mapped_T>(heightIn), pear(pair_in) {}

	template <typename Key_T, typename Mapped_T>
	class SkipNode {
		public:
			
			SkipNode() {}
			SkipNode(int);
			~SkipNode();
		
			int getHeight(void);
			
			SkipNode** bckNodes;
			SkipNode** fwdNodes;
			
			bool isSentinel = false;
			int nodeHeight;			
	};
	
	template <typename Key_T, typename Mapped_T>
	SkipNode<Key_T, Mapped_T>::SkipNode(int heightIn) {
		nodeHeight = heightIn;
		
		bckNodes = new SkipNode<Key_T, Mapped_T>* [heightIn + 1];
		fwdNodes = new SkipNode<Key_T, Mapped_T>* [heightIn + 1];
		
		for( int i = 1; i <= nodeHeight; i++ ) {
			fwdNodes[i] = (SkipNode<Key_T, Mapped_T>*) NULL;
			bckNodes[i] = (SkipNode<Key_T, Mapped_T>*) NULL;
		}
	}
	
	template <typename Key_T, typename Mapped_T>
	SkipNode<Key_T, Mapped_T>::~SkipNode() {
		//std::cout << "in the skipnode dest" << std::endl;
		delete [] fwdNodes;
		delete [] bckNodes;
	}
	
	template <typename Key_T, typename Mapped_T>
	class SkipList {
		public:
			SkipList() {}
			SkipList(float, int);
			~SkipList();
			
			bool insert(ValueType<Key_T, Mapped_T>, DataNode<Key_T, Mapped_T>*);
			bool remove(Key_T);
			SkipNode<Key_T, Mapped_T>* search(Key_T key_in);
		
			SkipNode<Key_T, Mapped_T>* head;
			SkipNode<Key_T, Mapped_T>* tail;
			
			float probability;
			int maxHeight;
			int currHeight;
			
			RandomHeight* randGen;	
	};
	
	template <typename Key_T, typename Mapped_T>
	SkipList<Key_T, Mapped_T>::SkipList(float probIn, int maxHeightIn) {
		currHeight = 1;
		probability = probIn;
		maxHeight = maxHeightIn;
		
		randGen = new RandomHeight(maxHeightIn, probIn);
		
		tail = new SkipNode<Key_T, Mapped_T>(maxHeightIn);
		head = new SkipNode<Key_T, Mapped_T>(maxHeightIn);
		
		head->isSentinel = true;
		tail->isSentinel = true;
		
		for(int i=1; i <= maxHeight; i++) {
			head->fwdNodes[i] = tail;
			tail->bckNodes[i] = head;
		} 
	}
	
	template <typename Key_T, typename Mapped_T>
	bool SkipList<Key_T, Mapped_T>::remove(Key_T key_in) {
		SkipNode<Key_T, Mapped_T>** updateVec =	new SkipNode<Key_T, Mapped_T>* [maxHeight+1];
		SkipNode<Key_T, Mapped_T>* new_node = head;

		// Find the node we need to delete
		int height = currHeight;
		for (; height > 0; height-- ) {
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) new_node->fwdNodes[height];
			
			while ( (data_node->isSentinel != true) && data_node->pear.first < key_in ) {
				new_node = new_node->fwdNodes[height];
				data_node = (DataNode<Key_T, Mapped_T>*) new_node->fwdNodes[height];
			}
			updateVec[height] = new_node;
		}
		
		new_node = new_node->fwdNodes[1];
		DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) new_node;

		if ( data_node->pear.first == key_in ) {
			for ( int i = 1; i <= currHeight; i++ ) {
				if ( new_node != updateVec[i]->fwdNodes[i] ) break;
				updateVec[i]->fwdNodes[i] = new_node->fwdNodes[i];
				updateVec[i]->bckNodes[i] = new_node->bckNodes[i];
			}
			
			delete new_node;
			
			DataNode<Key_T, Mapped_T>* tmp_tail = (DataNode<Key_T, Mapped_T>*) tail;
			DataNode<Key_T, Mapped_T>* tmp_head = (DataNode<Key_T, Mapped_T>*) head->fwdNodes[currHeight];
			
			while ( ( currHeight > 1 ) && ( ( tmp_head->pear.first == tmp_tail->pear.first ) ) )
				currHeight--;
			return true;
		} else {
			return false;
		}
		
		delete [] updateVec;
	}
	
	template <typename Key_T, typename Mapped_T>
	bool SkipList<Key_T, Mapped_T>::insert(const ValueType<Key_T, Mapped_T> pair_in, DataNode<Key_T, Mapped_T> *data_in) {
		int level = 0, height = 0;
		
		SkipNode<Key_T, Mapped_T>** updateVec = new SkipNode<Key_T, Mapped_T>* [maxHeight+1];
		
		SkipNode<Key_T, Mapped_T>* new_node = head;
	
		// Figure out where new node goes
		for ( height = currHeight; height >= 1; height-- ) {
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) new_node->fwdNodes[height];
			
			while ( (data_node->isSentinel != true) && data_node->pear.first < pair_in.first ) {
				new_node = new_node->fwdNodes[height];
				data_node = (DataNode<Key_T, Mapped_T>*) new_node->fwdNodes[height];
			}
			updateVec[height] = new_node;
		}
		
		new_node = new_node->fwdNodes[1];
		DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) new_node;
		
		// If duplicate, return false
		if ( data_node->pear.first == pair_in.first ) {
			data_in = data_node;
			return false;
		} else {
			// Perform an insert
			level = randGen->newLevel();
			if ( level > currHeight ) {
				for ( int i = currHeight + 1; i <= level; i++ ) {
					updateVec[i] = head;
				}
				currHeight = level;
			}
			// Insert new element
			DataNode<Key_T, Mapped_T>* newNode = new DataNode<Key_T, Mapped_T>(pair_in, level);
			for ( int i = 1; i <= level; i++ ) {
				newNode->fwdNodes[i] = updateVec[i]->fwdNodes[i];
				newNode->bckNodes[i] = updateVec[i];
				updateVec[i]->fwdNodes[i]->bckNodes[i] = newNode;
				updateVec[i]->fwdNodes[i] = newNode;
			}
			data_in = newNode;
		}
		
		delete [] updateVec;
		
		return true;
	}
	
	template <typename Key_T, typename Mapped_T>
	SkipNode<Key_T, Mapped_T>* SkipList<Key_T, Mapped_T>::search(Key_T key_in) {
		SkipNode<Key_T, Mapped_T>** updateFrontNodes = new SkipNode<Key_T, Mapped_T>* [maxHeight+1];
		SkipNode<Key_T, Mapped_T>* tmp = head;
		DataNode<Key_T, Mapped_T>* data_node;
		
		// Find the key and return the node
		int height = currHeight;
		for (; height >= 1; height-- ) {
			data_node = (DataNode<Key_T, Mapped_T>*) tmp->fwdNodes[height];
			
			while ( (data_node->isSentinel != true) && data_node->pear.first < key_in ) {
				tmp = tmp->fwdNodes[height];
				data_node = (DataNode<Key_T, Mapped_T>*) tmp->fwdNodes[height];
			}
			updateFrontNodes[height] = tmp;
		}
		
		tmp = tmp->fwdNodes[1];
		data_node = (DataNode<Key_T, Mapped_T>*) tmp;
	
		delete [] updateFrontNodes;
		
		if ( data_node->pear.first == key_in ) {
			//std::cout << "fouuund" << std::endl;
			return data_node;
		} else {
			//std::cout << "not fouuund" << std::endl;
			return tail;
		}
	}
	
	
	template <typename Key_T, typename Mapped_T>
	SkipList<Key_T, Mapped_T>::~SkipList() {
		//std::cout << "in skiplist descturot" << std::endl;
		
		delete randGen;
		
		// Walk 0 level nodes and delete all
		SkipNode<Key_T, Mapped_T>* tmp;
		SkipNode<Key_T, Mapped_T>* nxt;
		
		tmp = head;
		
		while ( tmp ) {
			nxt = tmp->fwdNodes[1];
			delete tmp;
			tmp = nxt;
		}
	}
	

	/*	
	This maps from Key_T objects to Mapped_T objects	
	*/
	template <typename Key_T, typename Mapped_T> 
	class Map {
		
	private: 
		SkipList<Key_T, Mapped_T> mapSkipList = SkipList<Key_T, Mapped_T>((float).5, 20);
		int map_size;
	public:
		
		class Iterator {
			
		public:
			DataNode<Key_T, Mapped_T>* it_DataNode;
		
			Iterator() { 
				//std::cout << "in iterator con" << std::endl;
			}
	
			Iterator(const Iterator &it_in) {
				it_DataNode = it_in.it_DataNode;
			}
	
			~Iterator() {
				//std::cout << "iterator destructor " << std::endl;
			}
	
			Iterator& operator++() {
				Iterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->fwdNodes[1];
				return (*this);
			}
	
			Iterator operator++(int) {
				Iterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->fwdNodes[1];
				return it;
			}	
	
			Iterator &operator--() {
				Iterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->bckNodes[1];
				return (*this);
			}
	
			Iterator operator--(int) {
				Iterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->bckNodes[1];
				return it;
			}


			ValueType<Key_T, Mapped_T>& operator*() const {
				return (it_DataNode->pear);
			}
	
			ValueType<Key_T, Mapped_T> *operator->() const {
				return &(it_DataNode->pear);
			}
		};


		class ConstIterator {
		
		public:
		
			DataNode<Key_T, Mapped_T>* it_DataNode;
			
			ConstIterator() {}
			
			ConstIterator(const ConstIterator &it_in) {
				it_DataNode = it_in.it_DataNode;
			}
	
			ConstIterator(const Iterator &) {
	
			}
	
			const ValueType<Key_T, Mapped_T> &operator*() const {
				return (it_DataNode->pear);
			}
	
			const ValueType<Key_T, Mapped_T> *operator->() const {
				return &(it_DataNode->pear);
			}

			~ConstIterator() {
			//	delete it_DataNode;
			}
	
			ConstIterator& operator=(const Iterator &it_in) {
				it_DataNode = it_in.it_DataNode;
			}
	
			ConstIterator& operator++() {
				ConstIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->fwdNodes[1];
				return (*this);
			}
	
			ConstIterator operator++(int) {
				ConstIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->fwdNodes[1];
				return it;
			}	
	
			ConstIterator &operator--() {
				ConstIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->bckNodes[1];
				return (*this);
			}
	
			ConstIterator &operator--(int) {
				ConstIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->bckNodes[1];
				return it;
			}
		};

		class ReverseIterator {
		
		public:
		
			DataNode<Key_T, Mapped_T>* it_DataNode;
			
			ReverseIterator() {}
		
			ReverseIterator(const ReverseIterator &r_in) {
				it_DataNode = r_in.it_DataNode;
			}
	
			~ReverseIterator() {
	
			}
	
			ReverseIterator& operator=(const Iterator &) {
	
			}
	
			ReverseIterator& operator++() {
				ReverseIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->fwdNodes[1];
				return (*this);
			}
	
			ReverseIterator operator++(int) {
				ReverseIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->fwdNodes[1];
				return it;
			}	
	
			ReverseIterator &operator--() {
				ReverseIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->bckNodes[1];
				return (*this);
			}
	
			ReverseIterator &operator--(int) {
				ReverseIterator it(*this);
				it_DataNode = (DataNode<Key_T, Mapped_T>*) it_DataNode->bckNodes[1];
				return it;
			}
			
			ValueType<Key_T, Mapped_T> &operator*() const {
				return (it_DataNode->pear);
			}
	
			ValueType<Key_T, Mapped_T> *operator->() const {
				return &(it_DataNode->pear);
			}
		};
		
/* ========= Constructors and Assignment Operator ======== */
	
		Map() { 
			//std::cout << "in constructor" << std::endl;
			map_size = 0;
		}
	
		/* 
		Copy constructor 
		*/
		Map(const Map &map_in) {
			mapSkipList = map_in.mapSkipList; 
			map_size = map_in.map_size;
		}
	
		/*
		Copy assignment operator. You must be able to handle self assignment
		*/
		Map &operator=(const Map &map_in) {
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) map_in.head->fwdNodes[1];
			
			while( map_in.tail != data_node ) {
				mapSkipList.insert(data_node->pear);
				data_node = data_node->fwdNodes[1];
			}
			
			return *this;
		}
	
		/*
		Initializer list constructor. Support for creation of map
		with initial values such as: Map<string, int> m{{"key1", 1}, {"key2", 2}}
		*/
		Map(std::initializer_list< std::pair<const Key_T, Mapped_T> > list_in) {
			for( auto pair : list_in ) {
				mapSkipList.insert(pair);
			}
		}
	
		/*
		Destructor. Release any acquired resources.
		*/
		~Map() {
			//std::cout << "in map destructor" << std::endl;
		}
	
/* ===================== Size ================ */
	
		/*
		Returns # of elements in map
		*/
		size_t size() const {
			return map_size;
		}
	
		/*
		Returns true if the map has no entries in it
		false otherwise
		*/
		bool empty() const {
			return (map_size == 0) ? true : false;
		}
	
/* ================= Iterators =============== */	

		/*
		Returns an iterator pointing to the first element, in order
		*/
		Iterator begin() {
			Iterator it;
			it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.head->fwdNodes[1];
			return it;
		}

		/*
		Returns an iterator pointing one past the last element, in order
		*/
		Iterator end() {
			Iterator it;
			it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.tail;
			return it;
		}
	
		/*
		Returns an iterator pointing to the first element, in order
		*/
		ConstIterator begin() const {
			ConstIterator it;
			it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.head->fwdNodes[1];
			return it;
		}
	
		/*
		Returns an iterator pointing one past the last element, in order
		*/
		ConstIterator end() const {
			ConstIterator it;
			it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.tail->bckNodes[1];
			return it;
		}
	
		/*
		Returns a reverseiterator to the first element in reverse order
		which is the last element in normal order
		*/
		ReverseIterator rbegin() {
			ReverseIterator it;
			it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.tail->bckNodes[1];
			return it;
		}
	
		/*
		Returns a reverseiterator pointing to one past the last element
		in reverse order, which is one before the first element
		in normal order
		*/
		ReverseIterator rend() {
			ReverseIterator it;
			it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.head->fwdNodes[1];
			return it;
		}


/* ============= Element Access ============ */

		/*
		Returns an iterator to the given key. If the key is not found, 
		these functions return the end() iterator.
		*/
		Iterator find(const Key_T &key_in) {
			Iterator it;
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) mapSkipList.search(key_in);
			
			if( mapSkipList.tail != data_node ) {
				it.it_DataNode = data_node;
				return it;
			} else {
				it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.tail;
				return it;
			}
		}
	
		/*
		Returns an iterator to the given key. If the key is not found, 
		these functions return the end() iterator.
		*/
		ConstIterator find(const Key_T &key_in) const {
			ConstIterator const_it;
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) mapSkipList.search(key_in);
			
			if( data_node != mapSkipList.tail ) {
				const_it.it_DataNode = data_node;
				return const_it;
			} else {
				const_it.it_DataNode = (DataNode<Key_T, Mapped_T>*) mapSkipList.tail;
				return const_it;
			}
		}
	
		/*
		Returns a reference to the mapped object at the specified key. 
		If the key is not in the Map, throws std::out_of_range.
		*/
		Mapped_T &at(const Key_T &key_in) {
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) mapSkipList.head;
			data_node = (DataNode<Key_T, Mapped_T>*) data_node->fwdNodes[1];
			
			for(; (DataNode<Key_T, Mapped_T>*) data_node->fwdNodes[1] != NULL; data_node = (DataNode<Key_T, Mapped_T>*) data_node->fwdNodes[1]) {
				if( data_node->pear.first == key_in ) {
					return data_node->pear.second;
				}
			}
			
			throw std::out_of_range("out of range");
			
		}
	
		/*
		Returns a const reference to the mapped object at the specified key. 
		If the key is not in the map, throws std::out_of_range.
		*/
		const Mapped_T &at(const Key_T &key_in) const {
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) mapSkipList.head;
			data_node = (DataNode<Key_T, Mapped_T>*) data_node->fwdNodes[1];
			
			for(; (DataNode<Key_T, Mapped_T>*) data_node->fwdNodes[1] != NULL; data_node = (DataNode<Key_T, Mapped_T>*)data_node->fwdNodes[1]) {
				if( data_node->pear.first == key_in ) {
					return data_node->pear.second;
				}
			}
			
			throw std::out_of_range("out of range");
			
		}

		/*
		If key is in the map, return a reference to the corresponding mapped object.
		If it is not, value initialize a mapped object for that key and returns a
		reference to it. This operator may not be used for a Mapped_T class type that
		does not support default construction.
		*/
		Mapped_T &operator[](const Key_T &key_in) {
			DataNode<Key_T, Mapped_T>* data_node = (DataNode<Key_T, Mapped_T>*) mapSkipList.search(key_in);
			
			if(data_node == mapSkipList->tail) {
				std::pair<Key_T, Mapped_T> pair(key_in, Mapped_T());
				Iterator it = mapSkipList->insert(pair);
				return it.it_DataNode->pear.second;
			}
			
			return data_node->pear.second;
		}


/* ============= Modifiers ============ */
	
		std::pair<Iterator, bool> insert(const ValueType<Key_T, Mapped_T> &pair_in) {
			Iterator it;
			std::pair<Iterator, bool> retval;
			
			if( mapSkipList.insert(pair_in, it.it_DataNode) ) {
				retval.first = it;
				retval.second = true;
				map_size++;
			} else {
				retval.first = it;
				retval.second = false;
			}
			
			return retval;
		}

		template <typename IT_T> void insert(IT_T range_beg, IT_T range_end) {
			while( range_end != range_beg ) {
				mapSkipList.insert(*range_beg);
				range_beg++;
			}
		}
	
		/*
		Removes the given object from the map. The object may be indicated
		by iterator or by key. If given by key, throws std::out_of_range
		if the key is not in the Map
		*/
		void erase(Iterator pos) {
			mapSkipList.remove(pos.it_DataNode->pear.first);
			map_size--;
		}
	
		/*
		Removes the given object from the map. The object may be indicated
		by iterator or by key. If given by key, throws std::out_of_range
		if the key is not in the Map
		*/
		void erase(const Key_T &key_in) {
			if( !(mapSkipList.remove(key_in)) ) throw std::out_of_range("out of range");
			map_size--;		
		}
	
		/*
		Removes all elements from the map
		*/
		void clear() {
			/*DataNode<Key_T, Mapped_T>* test = (DataNode<Key_T, Mapped_T>*) mapSkipList.head;
			test = (DataNode<Key_T, Mapped_T>*) test->fwdNodes[1];
			
			for(; (DataNode<Key_T, Mapped_T>*) test->fwdNodes[1] != NULL; test = (DataNode<Key_T, Mapped_T>*)test->fwdNodes[1]) {
				mapSkipList.remove(test->pear.first);
			}
			*/
			map_size = 0;
		}
		
		/*	DEBUGGING	*/
		void printList() {
			DataNode<Key_T, Mapped_T>* test = (DataNode<Key_T, Mapped_T>*) mapSkipList.head;
			test = (DataNode<Key_T, Mapped_T>*) test->fwdNodes[1];
			for(; (DataNode<Key_T, Mapped_T>*) test->fwdNodes[1] != NULL; test = (DataNode<Key_T, Mapped_T>*)test->fwdNodes[1]) {
				std::cerr << "Key: " << ( test->pear.first ) << "\tValue: " << ( test->pear.second ) << std::endl;
			}
		}
	
/* ============= Comparison for Map Class ============ */
	
		bool operator==(const Map &map2) {
			if( map_size != map2.map_size ) return false;
			
			Iterator i1 = begin();
			Iterator i2 = map2.begin();
			
			while( i1 != end() ) {
				if( i1 != i2 ) return false;
				i1++;
				i2++;
			}
			
			return true;
		}

		bool operator!=(const Map &map2) {
			return !( *this == map2 );
		}
	
		bool operator<(const Map &map2) {
			Iterator i1 = begin();
			Iterator i2 = map2.begin();
			
			while( i1 != end() && i2 != map2.end() ) {
				if( i2.first < i1.first ) return false;
				if( i1.first < i2.first ) return true;
				i1++;
				i2++;
			}	
			
			return ( map_size < map2.map_size ) ? true : false;
		}
	
/* ============= Comparison for Iterators ============ */
		
		friend bool operator==(const Iterator &i1, const Iterator &i2) {
			return i2.it_DataNode == i1.it_DataNode;
		}
		
		friend bool operator==(const ConstIterator &i1, const ConstIterator &i2) {
			return i2.it_DataNode == i1.it_DataNode;
		}
	
		friend bool operator==(const Iterator &i1, const ConstIterator &i2) {
			return i2.it_DataNode == i1.it_DataNode;
		}

		friend bool operator==(const ConstIterator &i1, const Iterator &i2) {
			return i2.it_DataNode == i1.it_DataNode;
		}
	
		friend bool operator!=(const Iterator &i1, const Iterator &i2) {
			return !(i2.it_DataNode == i1.it_DataNode);
		}
	
		friend bool operator!=(const ConstIterator &i1, const ConstIterator &i2) {
			return !(i2.it_DataNode == i1.it_DataNode);
		}
	
		friend bool operator!=(const Iterator &i1, const ConstIterator &i2) {
			return !(i2.it_DataNode == i1.it_DataNode);
		}
	
		friend bool operator!=(const ConstIterator &i1, const Iterator &i2) {
			return !(i2.it_DataNode == i1.it_DataNode);
		}
	
		friend bool operator==(const ReverseIterator &i1, const ReverseIterator &i2) {
			return i2.it_DataNode == i1.it_DataNode;
		}
	
		friend bool operator!=(const ReverseIterator &i1, const ReverseIterator &i2) {
			return !(i2.it_DataNode == i1.it_DataNode);
		}

	};
}


#endif

