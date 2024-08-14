/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DATATYPE_H
#define DATATYPE_H

#include <iostream>
#include <map>
#include <initializer_list>
#include <string>

namespace ns3 {

    template<typename K, typename V>
    class Dict {
    public:
        // Constructor using initializer list
        Dict(std::initializer_list<std::pair<const K, V>> init) : dict(init) {}

        // Overload operator[] to access or insert elements
        V& operator[](const K& key) {
            return dict[key];
        }

        // Add or update element
        void set(const K& key, const V& value) {
            dict[key] = value;
        }

        // Remove element by key
        bool remove(const K& key) {
            auto it = dict.find(key);
            if (it != dict.end()) {
                dict.erase(it);
                return true;
            }
            return false;
        }

        // Find element
        typename std::map<K, V>::iterator find(const K& key) {
            return dict.find(key);
        }

        typename std::map<K, V>::const_iterator find(const K& key) const {
            return dict.find(key);
        }

        // Check if element exists
        bool contains(const K& key) const {
            return dict.find(key) != dict.end();
        }

        // Get size of the dictionary
        size_t size() const {
            return dict.size();
        }

        // Clear all elements
        void clear() {
            dict.clear();
        }

        // Print all elements
        void print() const {
            for (const auto& pair : dict) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }
        }

        // Comparison operators
        bool operator==(const Dict& other) const {
            return this->dict == other.dict;
        }

        bool operator!=(const Dict& other) const {
            return this->dict != other.dict;
        }

    private:
        std::map<K, V> dict;
    };

    // Helper function to simplify initialization
    template<typename K, typename V>
    Dict<K, V> dict(std::initializer_list<std::pair<const K, V>> init) {
        return Dict<K, V>(init);
    }

    struct value
    {
        std::string  Key  ;
        int          INT  ;
        float       FLOAT ;
        double      DOUBLE;
        std::string STRING;
    };
}

#endif /* DATATYPE_H */
