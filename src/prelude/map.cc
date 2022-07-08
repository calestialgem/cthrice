// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "list.cc"
#include "view.cc"

#include <algorithm>
#include <cmath>

namespace cthrice
{
    /* Hash type. */
    using hash = u64;

    /* Fast value access using hashed keys. */
    template<typename K, typename V>
    struct Map {
        /* Pairs of key and value. */
        struct Pair {
            K key;
            V val;
        };

        /* Pair indicies corresponding to hashes. */
        List<ix> ixs;
        /* Pairs. */
        List<Pair> prs;

        /* Pairs corresponding to the hash. */
        static View<Pair> view(Map<K, V> map, hash hsh)
        {
            ix sze = List<ix>::size(map.ixs);

            // If the indicies are empty, probably the pairs are empty too.
            if (sze == 0) {
                // Actually it is supposed to be.
                debug(
                    List<Pair>::size(map.prs) == 0,
                    "Indicies are empty but not the pairs!");
                return List<Pair>::view(map.prs);
            }

            // Find the indicies that correspond to current and the next hashes.
            ix* current = map.ixs.bgn + hsh % sze;
            ix* next    = current + 1;

            // The current hash might be the last one; thus, check the next one.
            // If the next hash is valid, limit the search view from the end.
            if (next < map.ixs.end) {
                return List<Pair>::view_part(map.prs, *current, *next);
            }

            return List<Pair>::view_end(map.prs, *current);
        }

        /* Pointer to the value that corresponds to the key. Returns nullptr if
         * the value does not exist. */
        static V* at(Map<K, V> map, K key)
        {
            View<Pair> prs = view(map, K::hash(key));

            const Pair* pos = View<Pair>::find_fit(prs, [key](Pair p) {
                return K::equal(p.key, key);
            });

            if (pos == prs.end) {
                return nullptr;
            }
            return const_cast<V*>(&pos->val);
        }

        /* Deallocate the memory. */
        [[nodiscard]] static Map<K, V> free(Map<K, V> map)
        {
            map.ixs = List<ix>::free(map.ixs);
            map.prs = List<Pair>::free(map.prs);
            return map;
        }

        /* Maximum allowed amount of keys whose hashes give the same index after
         * modulus operation. Decreases the speed of the hashmap, but also
         * the memory usage. */
        static constexpr ix MAX_COLLISION = 4;

        /* Whether the map should be rehashed to fit the maximum collision
         * restriction. */
        static bool need_rehash(Map<K, V> map)
        {
            for (ix i = 0; i < List<ix>::size(map.ixs); i++) {
                if (View<Pair>::size(view(map, i)) > MAX_COLLISION) {
                    return true;
                }
            }
            return false;
        }

        /* Rehash until the maximum collision restriction is obeyed. */
        [[nodiscard]] static Map<K, V> rehash(Map<K, V> map)
        {
            do {
                // Grow the indicies.
                map.ixs = List<ix>::reserve(
                    map.ixs,
                    std::max((ix)1, List<ix>::capacity(map.ixs)));

                // Sort the pairs by hash.
                std::sort(map.prs.bgn, map.prs.end, [](Pair lhs, Pair rhs) {
                    return K::hash(lhs.key) < K::hash(rhs.key);
                });

                // Recalculate indicies.
                ix hsh  = -1;
                ix isze = List<ix>::size(map.ixs);
                ix psze = List<Pair>::size(map.prs);
                for (ix i = 0; i < psze; i++) {
                    ix chsh = (ix)(K::hash(map.prs.bgn[i].key) % isze);
                    debug(chsh >= hsh, "Pairs are not sorted correctly!");
                    while (chsh > hsh) {
                        map.ixs.bgn[++hsh] = i;
                    }
                }
                while (hsh < isze) {
                    map.ixs.bgn[++hsh] = psze;
                }
                debug(hsh != isze, "Did not correctly calculate indicies!");
            } while (need_rehash(map));

            return map;
        }

        /* Add the value to the map with the new key. If the key already exists
         * terminates. */
        [[nodiscard]] static Map<K, V> add(Map<K, V> map, K key, V val)
        {
            // Get the corresponding view for the hash.
            hash       hsh = K::hash(key);
            View<Pair> prs = view(map, hsh);

            // Check if it already exists.
            check(
                !View<Pair>::contains_fit(
                    prs,
                    [key](Pair p) {
                        return K::equal(p.key, key);
                    }),
                "Key already exists!");

            Pair p = {.key = key, .val = val};

            // If it would not create too many collisions.
            if (View<Pair>::size(prs) <= MAX_COLLISION) {
                // Put at the end of the view.
                ix end  = prs.end - map.prs.bgn;
                map.prs = List<Pair>::put(map.prs, p, end);

                // Increase indicies.
                ix sze = List<ix>::size(map.ixs);
                for (ix* i = map.ixs.bgn + hsh % sze + 1; i < map.ixs.end;
                     i++) {
                    (*i)++;
                }
            } else {
                // Just add to the end; rehash will handle the rest.
                map.prs = List<Pair>::add(map.prs, p);
                map     = rehash(map);
            }

            return map;
        }
    };
} // namespace cthrice
