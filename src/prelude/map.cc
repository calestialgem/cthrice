// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "array.cc"
#include "error.cc"
#include "list.cc"
#include "view.cc"

#include <algorithm>
#include <cmath>

namespace cthrice
{
    namespace map
    {
        /* Pairs of key and value. */
        template<typename K, typename V>
        struct Pair {
            K key;
            V val;
        };
    } // namespace map

    /* Mapping from a unique, hashable and comparible key to a value. Fast to
     * get a value and traverse all the values, slow to add a new value. */
    template<typename K, typename V>
    struct Map {
        /* Pair indicies corresponding to hashes. */
        Array<Ix> ixs;
        /* Pairs. */
        List<map::Pair<K, V>> prs;
    };

    namespace map
    {
        /* Pairs corresponding to the hash. */
        template<typename K, typename V>
        View<Pair<K, V>> view(Map<K, V> map, Hash hsh)
        {
            Ix sze = array::size(map.ixs);

            // If the indicies are empty, probably the pairs are empty too.
            if (sze == 0) {
                // Actually they are supposed to be.
                debug(
                    list::size(map.prs) == 0,
                    "Indicies are empty but not the pairs!");
                return list::view(map.prs);
            }

            // Find the indicies that correspond to current and the next hashes.
            Ix* current = map.ixs.bgn + hsh % sze;
            Ix* next    = current + 1;

            // The current hash might be the last one; thus, check the next one.
            // If the next hash is valid, limit the search view from the end.
            if (next < map.ixs.end) {
                return list::view_part(map.prs, *current, *next);
            }

            return list::view_end(map.prs, *current);
        }

        /* Reference to the value that corresponds to the key. */
        template<typename K, typename V>
        V& at(Map<K, V> map, K key)
        {
            View<Pair<K, V>> prs = view(map, hash(key));

            const Pair<K, V>* pos = view::find_fit(prs, [key](Pair<K, V> p) {
                return equal(p.key, key);
            });

            return const_cast<V&>(pos->val);
        }

        /* Pointer to the value that corresponds to the key. Returns nullptr if
         * the value does not exist. */
        template<typename K, typename V>
        V* get(Map<K, V> map, K key)
        {
            View<Pair<K, V>> prs = view(map, hash(key));

            const Pair<K, V>* pos = view::find_fit(prs, [key](Pair<K, V> p) {
                return equal(p.key, key);
            });

            if (pos == prs.end) {
                return nullptr;
            }
            return const_cast<V*>(&pos->val);
        }

        /* Deallocate the memory. */
        template<typename K, typename V>
        [[nodiscard]] Map<K, V> free(Map<K, V> map)
        {
            map.ixs = array::free(map.ixs);
            map.prs = list::free(map.prs);
            return map;
        }

        /* Maximum allowed amount of keys whose hashes give the same index after
         * modulus operation. Decreases the speed of the hashmap, but also
         * the memory usage. */
        constexpr Ix MAX_COLLISION = 4;

        /* Whether the map should be rehashed to fit the maximum collision
         * restriction. */
        template<typename K, typename V>
        bool need_rehash(Map<K, V> map)
        {
            for (Ix i = 0; i < array::size(map.ixs); i++) {
                if (view::size(view(map, i)) > MAX_COLLISION) {
                    return true;
                }
            }
            return false;
        }

        /* Rehash until the maximum collision restriction is obeyed. */
        template<typename K, typename V>
        [[nodiscard]] Map<K, V> rehash(Map<K, V> map)
        {
            do {
                // Grow the indicies; double, start from 1.
                map.ixs =
                    array::grow(map.ixs, std::max((Ix)1, array::size(map.ixs)));

                // Sort the pairs by hash.
                std::sort(
                    map.prs.bgn,
                    map.prs.end,
                    [](Pair<K, V> lhs, Pair<K, V> rhs) {
                        return hash(lhs.key) < hash(rhs.key);
                    });

                // Recalculate indicies.
                Ix hsh  = -1;
                Ix isze = array::size(map.ixs);
                Ix psze = list::size(map.prs);
                for (Ix i = 0; i < psze; i++) {
                    Ix chsh = (Ix)(hash(map.prs.bgn[i].key) % isze);
                    debug(chsh >= hsh, "Pairs are not sorted correctly!");
                    while (chsh > hsh) {
                        map.ixs.bgn[++hsh] = i;
                    }
                }
                while (++hsh < isze) {
                    map.ixs.bgn[hsh] = psze;
                }
                debug(hsh == isze, "Did not correctly calculate indicies!");
            } while (need_rehash(map));

            return map;
        }

        /* Add the value to the map with the new key. If the key already exists
         * terminates. */
        template<typename K, typename V>
        [[nodiscard]] Map<K, V> add(Map<K, V> map, K key, V val)
        {
            // Get the corresponding view for the hash.
            Hash             hsh = hash(key);
            View<Pair<K, V>> prs = view(map, hsh);

            // Check if it already exists.
            check(
                !view::contains_fit(
                    prs,
                    [key](Pair<K, V> p) {
                        return equal(p.key, key);
                    }),
                "Key already exists!");

            Pair<K, V> p   = {.key = key, .val = val};
            Ix         sze = array::size(map.ixs);

            // If it would not create too many collisions.
            if (view::size(prs) <= MAX_COLLISION && sze != 0) {
                if (prs.end < map.prs.end) {
                    // Put at the end of the view.
                    Ix end  = prs.end - map.prs.bgn;
                    map.prs = list::put(map.prs, end, p);
                } else {
                    // Add to the end.
                    map.prs = list::add(map.prs, p);
                }

                // Increase indicies.
                for (Ix* i = map.ixs.bgn + hsh % sze + 1; i < map.ixs.end;
                     i++) {
                    (*i)++;
                }
            } else {
                // Just add to the end; rehash will handle the rest.
                map.prs = list::add(map.prs, p);
                map     = rehash(map);
            }

            return map;
        }
    } // namespace map
} // namespace cthrice
