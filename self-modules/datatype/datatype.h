/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DATATYPE_H
#define DATATYPE_H


#include <iostream>
#include <map>
#include <initializer_list>
#include <string>


namespace ns3 {

/* ... */

    template<typename K, typename V>
    class Dict {
    public:
        // Constructor using initializer list
        Dict(std::initializer_list<std::pair<const K, V>> init) : dict(init) {}

        // Overload operator[] to access or insert elements
        V& operator[](const K& key) {
            return dict[key];
        }

        // Find element
        typename std::map<K, V>::iterator find(const K& key) {
            return dict.find(key);
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

}

#endif /* DATATYPE_H */

