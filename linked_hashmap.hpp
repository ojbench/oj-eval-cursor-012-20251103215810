/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    /**
     * In linked_hashmap, iteration ordering is differ from map,
     * which is the order in which keys were inserted into the map.
     * You should maintain a doubly-linked list running through all
     * of its entries to keep the correct iteration order.
     *
     * Note that insertion order is not affected if a key is re-inserted
     * into the map.
     */
    
template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class linked_hashmap {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::linked_hashmap as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;

private:
    struct Node {
        value_type *data;
        Node *next_in_bucket;  // For hash collision chain
        Node *prev;            // For insertion order
        Node *next;            // For insertion order
        
        Node() : data(nullptr), next_in_bucket(nullptr), prev(nullptr), next(nullptr) {}
        Node(const value_type &val) : next_in_bucket(nullptr), prev(nullptr), next(nullptr) {
            data = new value_type(val);
        }
        ~Node() {
            if (data) delete data;
        }
    };
    
    Node *head;      // Sentinel node at beginning
    Node *tail;      // Sentinel node at end
    Node **buckets;  // Hash table
    size_t bucket_count;
    size_t element_count;
    Hash hash_func;
    Equal equal_func;
    
    static const size_t INITIAL_BUCKET_COUNT = 16;
    static constexpr double LOAD_FACTOR = 0.75;
    
    void init_sentinels() {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->prev = head;
    }
    
    void init_buckets(size_t count) {
        bucket_count = count;
        buckets = new Node*[bucket_count];
        for (size_t i = 0; i < bucket_count; ++i) {
            buckets[i] = nullptr;
        }
    }
    
    size_t get_bucket_index(const Key &key) const {
        return hash_func(key) % bucket_count;
    }
    
    Node* find_in_bucket(const Key &key, size_t bucket_idx) const {
        Node *node = buckets[bucket_idx];
        while (node) {
            if (equal_func(node->data->first, key)) {
                return node;
            }
            node = node->next_in_bucket;
        }
        return nullptr;
    }
    
    void rehash(size_t new_bucket_count) {
        Node **new_buckets = new Node*[new_bucket_count];
        for (size_t i = 0; i < new_bucket_count; ++i) {
            new_buckets[i] = nullptr;
        }
        
        // Rehash all nodes
        Node *node = head->next;
        while (node != tail) {
            size_t new_idx = hash_func(node->data->first) % new_bucket_count;
            node->next_in_bucket = new_buckets[new_idx];
            new_buckets[new_idx] = node;
            node = node->next;
        }
        
        delete[] buckets;
        buckets = new_buckets;
        bucket_count = new_bucket_count;
    }
    
    void insert_to_order_list(Node *node, Node *pos) {
        node->prev = pos->prev;
        node->next = pos;
        pos->prev->next = node;
        pos->prev = node;
    }
    
    void remove_from_order_list(Node *node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    void deep_copy(const linked_hashmap &other) {
        init_sentinels();
        init_buckets(other.bucket_count);
        element_count = 0;
        
        Node *node = other.head->next;
        while (node != other.tail) {
            insert(*node->data);
            node = node->next;
        }
    }
    
    void clear_all() {
        Node *node = head->next;
        while (node != tail) {
            Node *next = node->next;
            delete node;
            node = next;
        }
        delete head;
        delete tail;
        delete[] buckets;
    }

public:
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = linked_hashmap.begin(); --it;
	 *       or it = linked_hashmap.end(); ++end();
	 */
	class const_iterator;
	class iterator {
        friend class linked_hashmap;
        friend class const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
        Node *node;
        const linked_hashmap *map;
        
	public:
		// The following code is written for the C++ type_traits library.
		// Type traits is a C++ feature for describing certain properties of a type.
		// For instance, for an iterator, iterator::value_type is the type that the 
		// iterator points to. 
		// STL algorithms and containers may use these type_traits (e.g. the following 
		// typedef) to work properly. 
		// See these websites for more information:
		// https://en.cppreference.com/w/cpp/header/type_traits
		// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
		// About iterator_category: https://en.cppreference.com/w/cpp/iterator
		using difference_type = std::ptrdiff_t;
		using value_type = typename linked_hashmap::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::output_iterator_tag;


		iterator() : node(nullptr), map(nullptr) {
		}
        
        iterator(Node *n, const linked_hashmap *m) : node(n), map(m) {
        }
        
		iterator(const iterator &other) : node(other.node), map(other.map) {
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
            if (node == map->tail) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            node = node->next;
            return tmp;
        }
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
            if (node == map->tail) {
                throw invalid_iterator();
            }
            node = node->next;
            return *this;
        }
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            if (node == map->head->next) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            node = node->prev;
            return tmp;
        }
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
            if (node == map->head->next) {
                throw invalid_iterator();
            }
            node = node->prev;
            return *this;
        }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
            return *(node->data);
        }
		bool operator==(const iterator &rhs) const {
            return node == rhs.node && map == rhs.map;
        }
		bool operator==(const const_iterator &rhs) const {
            return node == rhs.node && map == rhs.map;
        }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }
		bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
            return node->data;
        }
	};
 
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
        friend class linked_hashmap;
		private:
			// data members.
            Node *node;
            const linked_hashmap *map;
            
		public:
            using difference_type = std::ptrdiff_t;
            using value_type = typename linked_hashmap::value_type;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::output_iterator_tag;
            
			const_iterator() : node(nullptr), map(nullptr) {
			}
            
            const_iterator(Node *n, const linked_hashmap *m) : node(n), map(m) {
            }
            
			const_iterator(const const_iterator &other) : node(other.node), map(other.map) {
			}
            
			const_iterator(const iterator &other) : node(other.node), map(other.map) {
			}
            
			const_iterator operator++(int) {
                if (node == map->tail) {
                    throw invalid_iterator();
                }
                const_iterator tmp = *this;
                node = node->next;
                return tmp;
            }
            
            const_iterator & operator++() {
                if (node == map->tail) {
                    throw invalid_iterator();
                }
                node = node->next;
                return *this;
            }
            
            const_iterator operator--(int) {
                if (node == map->head->next) {
                    throw invalid_iterator();
                }
                const_iterator tmp = *this;
                node = node->prev;
                return tmp;
            }
            
            const_iterator & operator--() {
                if (node == map->head->next) {
                    throw invalid_iterator();
                }
                node = node->prev;
                return *this;
            }
            
            const value_type & operator*() const {
                return *(node->data);
            }
            
            bool operator==(const iterator &rhs) const {
                return node == rhs.node && map == rhs.map;
            }
            
            bool operator==(const const_iterator &rhs) const {
                return node == rhs.node && map == rhs.map;
            }
            
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
            
            const value_type* operator->() const noexcept {
                return node->data;
            }
	};
 
	/**
	 * TODO two constructors
	 */
	linked_hashmap() : element_count(0) {
        init_sentinels();
        init_buckets(INITIAL_BUCKET_COUNT);
    }
    
	linked_hashmap(const linked_hashmap &other) {
        deep_copy(other);
    }
 
	/**
	 * TODO assignment operator
	 */
	linked_hashmap & operator=(const linked_hashmap &other) {
        if (this != &other) {
            clear_all();
            deep_copy(other);
        }
        return *this;
    }
 
	/**
	 * TODO Destructors
	 */
	~linked_hashmap() {
        clear_all();
    }
 
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
        size_t idx = get_bucket_index(key);
        Node *node = find_in_bucket(key, idx);
        if (!node) {
            throw index_out_of_bound();
        }
        return node->data->second;
    }
    
	const T & at(const Key &key) const {
        size_t idx = get_bucket_index(key);
        Node *node = find_in_bucket(key, idx);
        if (!node) {
            throw index_out_of_bound();
        }
        return node->data->second;
    }
 
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
        size_t idx = get_bucket_index(key);
        Node *node = find_in_bucket(key, idx);
        if (node) {
            return node->data->second;
        }
        
        // Insert new element with default value
        auto result = insert(value_type(key, T()));
        return result.first->second;
    }
 
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
        return at(key);
    }
 
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
        return iterator(head->next, this);
    }
    
	const_iterator cbegin() const {
        return const_iterator(head->next, this);
    }
 
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
        return iterator(tail, this);
    }
    
	const_iterator cend() const {
        return const_iterator(tail, this);
    }
 
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
        return element_count == 0;
    }
 
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
        return element_count;
    }
 
	/**
	 * clears the contents
	 */
	void clear() {
        Node *node = head->next;
        while (node != tail) {
            Node *next = node->next;
            delete node;
            node = next;
        }
        head->next = tail;
        tail->prev = head;
        
        for (size_t i = 0; i < bucket_count; ++i) {
            buckets[i] = nullptr;
        }
        element_count = 0;
    }
 
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
        size_t idx = get_bucket_index(value.first);
        Node *existing = find_in_bucket(value.first, idx);
        
        if (existing) {
            return pair<iterator, bool>(iterator(existing, this), false);
        }
        
        // Check if we need to rehash
        if (element_count + 1 > bucket_count * LOAD_FACTOR) {
            rehash(bucket_count * 2);
            idx = get_bucket_index(value.first);
        }
        
        // Create new node
        Node *new_node = new Node(value);
        
        // Add to bucket
        new_node->next_in_bucket = buckets[idx];
        buckets[idx] = new_node;
        
        // Add to insertion order list (before tail)
        insert_to_order_list(new_node, tail);
        
        ++element_count;
        return pair<iterator, bool>(iterator(new_node, this), true);
    }
 
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
        if (pos.map != this || pos.node == tail || pos.node == head) {
            throw invalid_iterator();
        }
        
        Node *node = pos.node;
        const Key &key = node->data->first;
        size_t idx = get_bucket_index(key);
        
        // Remove from bucket
        Node **bucket_ptr = &buckets[idx];
        while (*bucket_ptr && *bucket_ptr != node) {
            bucket_ptr = &((*bucket_ptr)->next_in_bucket);
        }
        if (*bucket_ptr) {
            *bucket_ptr = node->next_in_bucket;
        }
        
        // Remove from insertion order list
        remove_from_order_list(node);
        
        delete node;
        --element_count;
    }
 
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 */
	size_t count(const Key &key) const {
        size_t idx = get_bucket_index(key);
        return find_in_bucket(key, idx) ? 1 : 0;
    }
 
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
        size_t idx = get_bucket_index(key);
        Node *node = find_in_bucket(key, idx);
        return node ? iterator(node, this) : end();
    }
    
	const_iterator find(const Key &key) const {
        size_t idx = get_bucket_index(key);
        Node *node = find_in_bucket(key, idx);
        return node ? const_iterator(node, this) : cend();
    }
};

}

#endif
